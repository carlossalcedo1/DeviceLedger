#pragma once
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
        {"button_grey",     "images/button_grey.png"},
        {"button_back",     "images/button_back.png"},
        {"back_page_button", "images/back_page_button.png"},
        {"next_page_button", "images/next_page_button.png"}
    };

    for (auto it = filePaths.begin(); it != filePaths.end(); ++it) {
        if (UIElements[it->first].loadFromFile(it->second)) {
            cout << "Loaded Image -> " << it->first << endl;
        }
    }
}

sf::Texture& TextureManager::getUIElement(const string &name) {
    return UIElements[name];
}
