#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "user.h"

using namespace std;

void User::addUser(std::shared_ptr<User> user, const std::string& username) {
    cout << "adding user to map: " << username << endl;
    loadedUsers.emplace(username, std::move(user));
}

bool User::deleteUser(const string& username) {
    if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
        loadedUsers.erase(it);
        return true;
    }
    return false;
}

bool User::userExists(const string& username) {
    if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
        return true;
    }
    return false;
}

shared_ptr<User> User::getUser(const string& username){
    if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
        return it->second;
    }
    return nullptr;
}

bool User::correctPassword(const string& username, const string& password) {
    if (const auto it = loadedUsers.find(username); it != loadedUsers.end()) {
        return it->second->getPassword() == password;
    }
    return false;
}



unsigned int User::loadAllUsers() {
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
        // Username String Length (int), Char[] of string username

        // Password String Length (int), Char[] of string password

    // DEVICES SECTION PER USER
        // number of total devices (int)

        //for each device
            // Name Length (int), char[] of string name
            // Brand Length (int), char[] of string brand
            // Model Length (int), char[] of string model
            // IMEI Length (int), char[] of string IMEI
            // Seller Length (int), char[] of string seller

    unsigned int numUsers = 0;
    usersFile.read(reinterpret_cast<char*>(&numUsers), sizeof(numUsers));
    uint8_t fieldLength;// 0 - 12
    //char* tempCopy;


    for (int i = 0; i < numUsers; i++) {

        //read username data
        usersFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
        string username(fieldLength, '\0');
        usersFile.read(&username[0], fieldLength);


        //read password data
        usersFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
        string password(fieldLength, '\0');
        usersFile.read(&password[0], fieldLength);

        //setup User pointer and add to map
        auto user = std::make_shared<User>(username, password);
        loadedUsers.emplace(username, user);
        std::cout << "Saving user: " << username << std::endl;

        //loading user devices

        std::cout << "Loaded user: " << username << "'s devices" <<std::endl;

        //get total amount of devices per user
        unsigned int devicesCount =0;
        usersFile.read(reinterpret_cast<char*>(&devicesCount), sizeof(devicesCount));

        //loop through their devices and get 5 string from each
        for (int j = 0; j < devicesCount; j++) {
            string prompts[5];
            for (string& prompt : prompts) {
                usersFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                prompt.resize(fieldLength, '\0');
                usersFile.read(&prompt[0], fieldLength);


            }
            user->addDevice(prompts);
        }

    }
    return numUsers;
}



//write usernames and passwords and devices per user.
void User::saveUserData() {
    ofstream usersFile("files/users.dat", ios::binary);
    if (!usersFile.is_open()) {
        cout << "File could not be opened to save." << endl;
        return;
    }
    uint8_t fieldLength;
    cout << "Saving users data.." << endl;

    //the new user was already added to loadedUsers Map when createAccount is called.

    //save the size of loaded users map
    unsigned int size = loadedUsers.size();
    usersFile.write(reinterpret_cast<char*>(&size), sizeof(size));

    //loop through all users saving username, password, devices
    for (const auto&[name, user] : loadedUsers) {

        //write username size and its characters
        fieldLength = static_cast<uint8_t>(user->getUsername().size());
        usersFile.write(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
        usersFile.write(user->getUsername().c_str(), fieldLength);

        //write password size and its characters
        fieldLength = static_cast<uint8_t>(user->getPassword().size());
        usersFile.write(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
        usersFile.write(user->getPassword().c_str(), fieldLength);

        //write devices

        //devices count
        unsigned int numDevices = user->devices.size();
        cout << "Saving devices.., amount: " << numDevices << endl;
        usersFile.write(reinterpret_cast<char*>(&numDevices), sizeof(numDevices));

        for (const auto&[name, device] : user->devices) {
            string prompts[5] = {device->name, device->model, device->brand, device->imeiNumber, device->seller};
            cout << "Saving device: " << name << endl;
            for (const auto& prompt : prompts) {
                //write length and the characters of string
                fieldLength = static_cast<uint8_t>(prompt.size());
                usersFile.write(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                usersFile.write(prompt.c_str(), fieldLength);
            }
        }
    }
}
