#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "screens.h"


using namespace std;

struct TextureManager {
    static unordered_map<string, sf::Texture> UIElements;
    static void loadUIElements();
    static sf::Texture& getUIElement(const string &name);

};


struct ButtonManager {
    sf::Sprite sprite;
    sf::Text text;
    bool isText;
    pair<float, float> position;
    string name;

    ButtonManager(const sf::Texture &texture, const float &x, const float &y, const string &name, const float& scaleX = 0.1f, const float& scaleY = 0.1f) {
        sprite.setTexture(texture);
        sprite.setScale(scaleX, scaleY);
        this->name = name;
        isText = false;

        const auto bounds = sprite.getGlobalBounds();
        sprite.setOrigin(bounds.width / 2, bounds.height / 2);

        sprite.setPosition(x, y);
    }

    ButtonManager(const string& message, const sf::Font& font, const int& fontSize, const float &x, const float &y, const string &name, const bool& underline = false, const bool& bold = false) {
        text.setFont(font);
        text.setString(message);
        text.setCharacterSize(fontSize);
        text.setFillColor(sf::Color::Black);
        position = make_pair(x, y);
        this->name = name;
        isText = true;
        if (underline) {text.setStyle(sf::Text::Underlined);}
        if (bold) {text.setStyle(sf::Text::Bold);}

        const sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x,y));
    }

    string inline getMessage() const {return name;}

    bool buttonClicked(const sf::Vector2f& mousePos) const {
        if (isText) {
            return text.getGlobalBounds().contains(mousePos);
        } else {
            return sprite.getGlobalBounds().contains(mousePos);
        }

    }

    void recenter() {
        const sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
        textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(position.first,position.second));
    }

    void draw(sf::RenderWindow& window) const {
        if (isText) {
            window.draw(text);
        } else {
            window.draw(sprite);

            sf::FloatRect bounds = sprite.getGlobalBounds();
            sf::RectangleShape debugBox;
            debugBox.setSize({ bounds.width, bounds.height });
            debugBox.setPosition({ bounds.left, bounds.top });
            debugBox.setFillColor(sf::Color::Transparent);
            debugBox.setOutlineColor(sf::Color::Red);
            debugBox.setOutlineThickness(1.f);
            window.draw(debugBox);
        }
    }

};