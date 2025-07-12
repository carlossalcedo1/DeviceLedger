#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "transfers.h"

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
        bool listed = false;
        string listCode;

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
    unsigned int pageNumber = 0;

public:
    TransferManager manager;
    User(const string& username, const string& password) : username(username), password(password) {}

    //getters
    const std::unordered_map<std::string, std::shared_ptr<Device>>& getDevices() const {return devices;}
    shared_ptr<Device> inline getCurrentDevice() const {return !currentDevice.expired() ? currentDevice.lock() : nullptr;}

    [[nodiscard]] inline bool getListedStatus(const shared_ptr<Device>& device) const {return device->listed;}
    [[nodiscard]] inline string getUsername() const {return username;}
    [[nodiscard]] inline string getPassword() const {return password;}
    [[nodiscard]] inline unsigned int  getPageNum() const {return pageNumber;}

    //toggles listed status and returns current state after change
    inline bool toggleListedStatus(const shared_ptr<Device>& device) {
        if (device->listed) {
            device->listed = false;
        } else {
            device->listed = true;
        }
        return device->listed;
    }

    inline void setListCode(const shared_ptr<Device>& device, const string& code) {
        if (device->listed) {
            device->listCode = code;
        }
    }

    void inline setCurrentDevice(const weak_ptr<Device> &device) {
        currentDevice = device;
    }
    void inline deleteCurrentDevice(const string &deviceName) {
        devices.erase(deviceName);
    }


    //functions - DEVICES
    inline void addDevice(const string& name, const string& brand, const string& model, const string& imeiNumber, const string &seller) {
        Device newDevice(name, model, brand, imeiNumber, seller);
        devices.emplace(name, make_shared<Device>(newDevice));
    }
    inline void addDevice(const string responses[5]) {
        Device newDevice(responses);
        devices.emplace(responses[0], make_shared<Device>(newDevice));
    }

    inline void addDevice(const string responses[5], const bool listed, const string& listCode) {
        Device newDevice(responses);
        newDevice.listed = listed;
        newDevice.listCode = listCode;
        devices.emplace(responses[0], make_shared<Device>(newDevice));
    }

    inline void deleteTransferredDevice(const string& deviceName) {
        const auto it = devices.find(deviceName);
        if (it != devices.end()) {
            devices.erase(it);
        }
    }

    inline void deListItem(const string& deviceName) {
        const auto it = devices.find(deviceName);
        if (it != devices.end()) {
            it->second->listed = false;
            it->second->listCode.clear();
        }
    }


    inline void incrementPageNumber() {++pageNumber;}
    inline void decrementPageNumber() {--pageNumber;}
    inline void zeroPageNumber() {pageNumber = 0;}

    //functions - USER ACCOUNT
    static bool userExists(const string& username);
    static shared_ptr<User> getUser(const string& username);
    static bool correctPassword(const string& username, const string& password);

    static void addUser(std::shared_ptr<User> user, const std::string& username);
    static bool deleteUser(const string& username);

    static unsigned int loadAllUsers();
    void saveUserData();




};



