#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include "transfers.h"

using namespace std;
//codes.dat code structure

//number of codes (int)

//code (string)
// -> previous user (string)
// -> device info (5 parameters) (strings)

//CODE GENERATION FUNCTIONS

string TransferManager::generateRandomCode() {
    static const std::string charset = "abcdefghijklmnopqrstuvwxyz0123456789";
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<int> dist(0, charset.size() - 1);

    std::string code;
    for (int i = 0; i < 4; ++i) {
        code += charset[dist(rng)];
    }
    return code;
}

string TransferManager::generateUniqueCode() {
    std::string code;
    do {
        code = generateRandomCode();
    } while (codes.find(code) != codes.end());
    return code;
}

//file system functions

void TransferManager::saveCodes() {
    ofstream codesFile("files/codes.dat", ios::binary);
    if (!codesFile.is_open()) {
        cout << "File could not be opened to save codes." << endl;
        return;
    }
    cout << "Saving codes..." << endl;

    uint8_t fieldLength;

    //size of map which is all codes
    unsigned int size = codes.size();
    codesFile.write(reinterpret_cast<char*>(&size), sizeof(size));

    //for all codes in map of shared_ptr<Transfer>
    for (const auto& [code, objectPointer] : codes) {
        if (const auto marketplacePtr = std::dynamic_pointer_cast<Marketplace>(objectPointer)) {
        //marketplace type of codes
            //write code, previous user and its 5 device parameters, + condition and price
            string prompts[8] = {
                "Marketplace",
                marketplacePtr->code,
                marketplacePtr->previousUser,
                marketplacePtr->deviceName,
                marketplacePtr->deviceBrand,
                marketplacePtr->deviceModel,
                marketplacePtr->deviceIMEI,
                marketplacePtr->condition,
            };
            int price = marketplacePtr->price;
            for (const auto& prompt : prompts) {
                //write length and the characters of string
                fieldLength = static_cast<uint8_t>(prompt.size());
                codesFile.write(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                codesFile.write(prompt.c_str(), fieldLength);
            }
            //write the price
            codesFile.write(reinterpret_cast<char*>(&price), sizeof(price));

            //output
            cout << prompts[0] << prompts[1] << prompts[2] << prompts[3] << prompts[4] << prompts[5] << prompts[6] << endl;
            cout << "Saved Marketplace code: " << code << endl;

        //Transfer Type
        } else {
            //write code, previous user and its 5 device parameters
            string prompts[7] = {
                "Transfer",
                objectPointer->code,
                objectPointer->previousUser,
                objectPointer->deviceName,
                objectPointer->deviceBrand,
                objectPointer->deviceModel,
                objectPointer->deviceIMEI,
            };
            for (const auto& prompt : prompts) {
                //write length and the characters of string
                fieldLength = static_cast<uint8_t>(prompt.size());
                codesFile.write(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                codesFile.write(prompt.c_str(), fieldLength);
            }
            cout << prompts[0] << prompts[1] << prompts[2] << prompts[3] << prompts[4] << prompts[5] << endl;
            cout << "Saved Transfer code: " << code << endl;
        }

    }
    cout << "All codes saved." << endl << endl;
}


void TransferManager::loadCodes() {
    ifstream codesFile("files/codes.dat", ios::binary);
    if (!codesFile.is_open()) {
        cout << "Could not open file to load codes." << endl;
        return;
    }
    cout << "Loading codes..." << endl;

    //clear existing to sync with file
    codes.clear();

    //count total codes
    unsigned int codesCount = 0;
    codesFile.read(reinterpret_cast<char*>(&codesCount), sizeof(codesCount));

    for (unsigned int i = 0; i < codesCount; i++) {
        uint8_t fieldLength;
        string type;
        //read type of code
        codesFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
        type.resize(fieldLength);
        codesFile.read(&type[0],fieldLength);

        if (type == "Transfer") {
            string fields[6];

            for (auto & field : fields) {
                codesFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                field.resize(fieldLength);
                codesFile.read(&field[0], fieldLength);
            }

            cout << "Transfer Code Loaded -> " << fields[0] << endl;

            const auto codeObject = make_shared<TransferCode>(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
            //codeObject->code = fields[0]; // overwrite randomly generated code with saved one

            codes[fields[0]] = codeObject; // use code as key
        //marketplace type
        } else if (type == "Marketplace") {
            string fields[7];
            int price;
            for (auto & field : fields) {
                codesFile.read(reinterpret_cast<char*>(&fieldLength), sizeof(fieldLength));
                field.resize(fieldLength);
                codesFile.read(&field[0], fieldLength);
            }
            codesFile.read(reinterpret_cast<char*>(&price), sizeof(price));

            cout << "Marketplace Code Loaded -> " << fields[0] << endl;

            const auto codeObject = make_shared<Marketplace>(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], price);
            //codeObject->code = fields[0]; // overwrite randomly generated code with saved one

            codes[fields[0]] = codeObject; // use code as key
        }
    }
    cout << "All codes loaded." << endl << endl;
}

void TransferManager::clearFile() {
    std::ofstream codesFile("files/codes.dat", std::ios::binary | std::ios::out | std::ios::trunc);
    codesFile.close();
    cout << "Clearing all codes..." << endl;
}
