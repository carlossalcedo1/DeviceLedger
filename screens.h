#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <chrono>

#include "user.h"

using namespace std;



class ScreenManager {
    sf::RenderWindow window;
    sf::Font font;
    float width = 900;
    float height = 600;

    bool error = false;
    std::chrono::steady_clock::time_point errorStartTime;

    enum class ButtonState {
        None,
        User,
        Password,
        Quit,
    };

    enum class State {
        Login,
        CreateAccount,
        ForgotPassword,
        Dashboard,
        Store,
        CheckConsole,
        listDevices,
        currentDeviceScreen,
        transferCode,
        transferCodeBuyer,
        itemList,
        marketplace,
        storeCurrentDevice,
        settings,
        Exit,
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

    static inline string& textFormatter(string &text);

    shared_ptr<User> inline getCurrentUser() const {return !currentUser.expired() ? currentUser.lock() : nullptr;}

    void run();

    void loginScreen();

    void createAccountScreen();

    void forgotPasswordScreen();

    void dashboardScreen();

    void listDevices();

    void checkConsoleScreen();

    void currentDeviceScreen();

    void transferCodeScreenSeller();

    void transferCodeScreenBuyer();

    void itemList();

    void store();

    void storeCurrentDevice();

    void settings();
};



