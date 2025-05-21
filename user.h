#pragma once
#include <string>
#include <unordered_map>
#include <memory>

using namespace std;

class User {
    static std::unordered_map<string, shared_ptr<User>> loadedUsers;
    string username;
    string password;


    struct Device {
        string name;
        string model;
        string brand;
        string imeiNumber;
        string seller;

        Device(const string& name, const string& model, const string& brand, const string& imeiNumber, const string& seller) : name(name),
        model(model), brand(brand), imeiNumber(imeiNumber), seller(seller) {}

        explicit Device(const string responses[5]) {
            name = responses[0];
            model = responses[1];
            brand = responses[2];
            imeiNumber = responses[3];
            seller = responses[4];
        }
    };


    std::unordered_map<string, shared_ptr<Device>> devices;
    weak_ptr<Device> currentDevice;

public:
    User(const string& username, const string& password) : username(username), password(password) {}

    //getters
    const std::unordered_map<std::string, std::shared_ptr<Device>>& getDevices() const {return devices;}

    void inline setCurrentDevice(const weak_ptr<Device> &device) {currentDevice = device;}
    shared_ptr<Device> inline getCurrentDevice() const {return !currentDevice.expired() ? currentDevice.lock() : nullptr;}

    [[nodiscard]] inline string getUsername() const {return username;}
    [[nodiscard]] inline string getPassword() const {return password;}

    //functions - DEVICES
    //string inline getProperty(const string &name, const string &property) const;
    inline void addDevice(const string& name, const string& model, const string& brand, const string& imeiNumber, const string &seller) {
        Device newDevice(name, model, brand, imeiNumber, seller);
        devices.emplace(name, make_shared<Device>(newDevice));
    }
    inline void addDevice(const string responses[5]) {
        Device newDevice(responses);
        devices.emplace(responses[0], make_shared<Device>(newDevice));
    }

    //functions - USER ACCOUNT
    static bool userExists(const string& username);
    static shared_ptr<User> getUser(const string& username);
    static bool correctPassword(const string& username, const string& password);

    static void addUser(std::shared_ptr<User> user, const std::string& username);
    static bool deleteUser(const string& username);

    static unsigned int loadAllUsers();
    void saveUserData();




};



