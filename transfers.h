#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

using namespace std;

//codes.dat code structure

//number of codes (int)

//code (string)
// -> previous user (string)
// -> device info (5 parameters) (strings)


class TransferManager {
public:

    struct TransferCode {
        string code;
        string previousUser;

        string deviceName;
        string deviceBrand;
        string deviceModel;
        string deviceIMEI;
        string deviceSeller = "Transfer";

        TransferCode(const string& code, const string &previousUser, const string& deviceName, const string& deviceBrand, const string& deviceModel, const string& deviceIMEI)
           :code(code), previousUser(previousUser), deviceName(deviceName), deviceBrand(deviceBrand), deviceModel(deviceModel), deviceIMEI(deviceIMEI) {
        }

        virtual ~TransferCode() = default;
    };

    struct Marketplace final : public TransferCode {
        string condition;
        int price = 0;
        Marketplace(const string& code,const string& previousUser,const string& deviceName,const string& deviceBrand,const string& deviceModel,const string& deviceIMEI,const string& condition,const int& price)
            : TransferCode(code, previousUser, deviceName, deviceBrand, deviceModel, deviceIMEI), condition(condition), price(price) {}
    };

    //map of all codes
    unordered_map<string, shared_ptr<TransferCode>> codes;

private:
    //private code generation functions
    static string generateRandomCode();
    string generateUniqueCode();

public:

    //add and delete codes
    inline string addCode(const string& prevUser, const string& name, const string& brand, const string& model, const string& imeiNumber) {
        auto ptr = make_shared<TransferCode>(generateUniqueCode(), prevUser, name, brand, model, imeiNumber);
        codes.emplace(ptr->code, ptr);
        return ptr->code;
    }

    inline bool deleteCode(const string& code) {
        if (const auto it = codes.find(code); it != codes.end()) {
            codes.erase(it);
            return true;
        }
        return false;
    }

    //add marketplace codes
    inline string addMarketplaceCode(const string& prevUser, const string& name, const string& brand, const string& model, const string& imeiNumber, const string& condition, const int& price) {
        auto ptr = make_shared<Marketplace>(generateUniqueCode(), prevUser, name, brand, model, imeiNumber, condition, price);
        codes.emplace(ptr->code, ptr);
        return ptr->code;
    }

    //returns device pointer if its found
    inline shared_ptr<TransferCode> getCode(const string& code) {
        const auto it = codes.find(code);
        return it != codes.end() ? it->second : nullptr;
    }

    //file system functions

    void saveCodes();
    static void clearFile();
    void loadCodes();
};