#pragma once
#include <string>
#include <unordered_map>
#include <fstream>

using namespace std;

class User {
    static std::unordered_map<string, shared_ptr<User>> loadedUsers;
    string username;
    string password;

public:
    User(const string& username, const string& password) : username(username), password(password) {
        std::cout << "Creating user: " << username << std::endl;
    };

    // ~User() {
    //     for (auto it = loadedUsers.begin(); it != loadedUsers.end(); ++it) {
    //         delete it->second;
    //     }
    //     loadedUsers.clear();
    // };

    [[nodiscard]] string getUsername() const {
        return username;
    }

    [[nodiscard]] string getPassword() const {
        return password;
    }


    // bool correctQuestion(const char& question) const {
    //     return question == this->securityNumber;
    // }

    static bool userExists(const string& username) {
        if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
            return true;
        }
        return false;
    }

    static shared_ptr<User> getUser(const string& username) {
        if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
            return it->second;
        }
        return nullptr;
    }

    static bool correctPassword(const string& username, const string& password) {
        if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
            return it->second->getPassword() == password;
        }
        return false;
    }

    static void addUser(std::shared_ptr<User> user, const std::string& username) {
        cout << "adding user to map: " << username << endl;
        loadedUsers.emplace(username, std::move(user));
    }

    static bool deleteUser(const string& username) {
        if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
            loadedUsers.erase(it);
            return true;
        }
        return false;
    }

    static unsigned int loadAllUsers() {
        ifstream usersFile("files/users.dat", ios::binary);
        if (!usersFile.is_open()) {
            //create file if it doesn't exist
            ofstream createFile("files/users.dat", ios::binary);
            createFile.close();

            usersFile.open("files/users.dat", ios::binary);
        }

        if (!usersFile.is_open()) {
            cout << "File could not be opened, read-binary, users.data" << endl;
            return 0;
        }

        //total number of users in file

        // FILE STRUCTURE (For each user)
        // Username String Length (int)
        // Char[] of string username

        // Password String Length (int)
        // Char[] of string password
        unsigned int numUsers = 0;
        usersFile.read(reinterpret_cast<char*>(&numUsers), sizeof(numUsers));
        for (int i = 0; i < numUsers; i++) {
            char fieldLength;// 0 - 12
            char* tempCopy;


            usersFile.read(&fieldLength, sizeof(fieldLength));
            tempCopy = new char[fieldLength];
            usersFile.read(tempCopy, fieldLength);
            string username(tempCopy, fieldLength);
            delete[] tempCopy;

            usersFile.read(&fieldLength, sizeof(fieldLength));
            tempCopy = new char[fieldLength];
            usersFile.read(tempCopy, fieldLength);
            string password(tempCopy, fieldLength);
            delete[] tempCopy;

            loadedUsers.emplace(username, std::make_shared<User>(username, password));
            std::cout << "Loading user: " << username << std::endl;

        }
        return numUsers;
    }

    static bool saveAllUsers() {
        if (loadedUsers.empty()) {
            std::cout << "No users to save." << std::endl;
            return false;
        }


        ofstream usersFile("files/users.dat", ios::binary);
        if (!usersFile.is_open()) {
            cout << "File could not be opened to save." << endl;
            return false;
        }

        //the new user was already added to loadedUsers Map when createAccount is called.

        unsigned int size = loadedUsers.size();
        usersFile.write(reinterpret_cast<char*>(&size), sizeof(size));

        for (const auto& user : loadedUsers) {
            char fieldLength = static_cast<char>(user.second->getUsername().size());
            usersFile.write(&fieldLength, sizeof(fieldLength));

            for (unsigned int i = 0; i < user.second->getUsername().size(); i++) {
                char temp = user.second->getUsername()[i];
                usersFile.write(&temp, sizeof(temp));
            }

            fieldLength = static_cast<char>(user.second->getPassword().size());
            usersFile.write(&fieldLength, sizeof(fieldLength));
            for (unsigned int i = 0; i < user.second->getPassword().size(); i++) {
                char temp = user.second->getPassword()[i];
                usersFile.write(&temp, sizeof(temp));
            }
        }

        return true;
    }

};



