#include <unordered_map>
#include <string>
#include "textures.h"
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;

unordered_map<string, sf::Texture> TextureManager::UIElements;

void TextureManager::loadUIElements() {
    //Optimization-> want to use constexpr, make a compile-time key lookup with a constexpr function
    const std::unordered_map<std::string, std::string> filePaths = {
        {"button_red",        "images/button_red.png"},
        {"button_green",     "images/button_green.png"},
        {"button_enter",     "images/button_enter.png"},
        {"button_home",     "images/button_home.png"},
        {"button_back",     "images/button_back.png"},
        {"button_settings",     "images/button_settings.png"},
        {"button_list",     "images/button_list.png"},
        {"button_add",     "images/button_add.png"},
        {"back_page_button", "images/back_page_button.png"},
        {"next_page_button", "images/next_page_button.png"},
        {"button_hazard", "images/button_hazard.png"},
        {"button_trash", "images/trash_button.png"},
        {"button_buy", "images/button_buy.png"},
        {"button_post", "images/button_post.png"},
        {"button_transfer", "images/button_transfer.png"},
        {"logo_apple", "images/logo_apple.png"},
        {"logo_samsung", "images/logo_samsung.png"},
    };

    for (const auto &[name, directory] : filePaths) {
        if (UIElements[name].loadFromFile(directory)) {
            cout << "Loaded Image -> " << name << endl;
        }
    }
    cout << "All images and buttons loaded." << endl << endl;
}

sf::Texture& TextureManager::getUIElement(const string &name) {
    return UIElements[name];
}
