#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <chrono>

#include "user.h"

using namespace std;

enum class State {
    Login,
    CreateAccount,
    ForgotPassword,
    Dashboard,
    waterTracker,
    calorieTracker,
    Records,
    Exit,
};


class ScreenManager {
    sf::RenderWindow window;
    sf::Font font;

    bool error = false;
    std::chrono::steady_clock::time_point errorStartTime;

    float width = 900;
    float height = 600;

    enum class ButtonState {
        None,
        User,
        Password,
        Quit,
    };

    State currentState = State::Login;
    ButtonState buttonState = ButtonState::None;
    weak_ptr<User> currentUser;

public:
    ScreenManager() {
        if (!font.loadFromFile("files/font.ttf")) {
            cout << "Failed to load font." << endl;\
        }
    }

    ~ScreenManager() {
        cout << "Screen Manager object deleted, automatic memory management through smart pointers" << endl;
    }

    void run();

    void loginScreen();

    void createAccountScreen();

    void forgotPasswordScreen();

    void dashboardScreen();

    void calorieScreen();

    void waterScreen();


};



