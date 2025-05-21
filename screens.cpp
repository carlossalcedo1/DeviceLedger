#pragma once
#include "screens.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>

#include "textures.h"
#include "user.h"

//center and position text function
void setText(sf::Text& text, float x ,float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x,y));

}

inline string& ScreenManager::textFormatter(string &text) {
    if (!text.empty()) {
        text[0] = toupper(text[0]);
        std::transform(text.begin() + 1, text.end(), text.begin() + 1, ::tolower);
        return text;
    }
    return text;
}


void ScreenManager::run() {
    window.create(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "Gym Tracker [app name]");
    font.loadFromFile("files/font.ttf");

    while (window.isOpen() && currentState != State::Exit ) {
        if (currentState == State::Login) {
            loginScreen();
        } else if (currentState == State::CreateAccount) {
            createAccountScreen();
        } else if (currentState == State::Dashboard) {
            dashboardScreen();
        } else if (currentState == State::CheckConsole) {
            checkConsoleScreen();
        } else if (currentState == State::listDevices) {
            listDevices();
        } else if (currentState == State::currentDeviceScreen) {
            currentDeviceScreen();
        }
            // implement rest
    }
}


void ScreenManager::loginScreen() {
    bool success = false;
    string username = "[Username]";
    string password = "[Password]";

    //setup texts
    const ButtonManager welcomeText("Login", font, 24, width/2.0f, height/2.0f - 200, "welcome_text", false, true);
    ButtonManager usernameText(username, font, 24, width/2.0f, height/2.0f - 20, "username_text");
    ButtonManager passwordText(password, font, 24, width/2.0f, height/2.0f + 80, "password_text");
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    const ButtonManager newAccountText("Create Account", font, 18,width/2.0f-150, height/2.0f+200, "error_text", true);
    const ButtonManager forgotPasswordText("Forgot Password?", font, 18,width/2.0f+150, height/2.0f+200, "error_text", true);
    warningText.text.setFillColor(sf::Color::Red);

    //setup buttons
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f-50, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f+50, "password_button");
    const ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+250, height/2.0f-50, "enter_button", 0.2, 0.2);


    while (window.isOpen()) {

        if (error) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - errorStartTime).count() >= 3) {
                if (success) {
                    currentState = State::Dashboard;
                    return;
                }
                error = false;
            }
        }


        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            //mouse button events
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (usernameButton.buttonClicked(mousePos)) {
                    buttonState = ButtonState::User;
                    username = "|";

                } else if (passwordButton.buttonClicked(mousePos)) {
                    buttonState = ButtonState::Password;
                    password = "|";
                } else if (enterButton.buttonClicked(mousePos)) {

                    if (username.size() <= 13 && username.size() >=4 && password.size() <= 13 && password.size() >=9 && username != "[Username]" && password != "[Password]") {
                        //format the username correctly
                        string formattedPassword = password.substr(0, password.size() - 1);
                        string formattedUser = username.substr(0, username.size() - 1);
                        formattedUser[0] = toupper(formattedUser[0]);
                        std::transform(formattedUser.begin() + 1, formattedUser.end(), formattedUser.begin() + 1, ::tolower);


                        if (!User::userExists(formattedUser) || !User::correctPassword(formattedUser, formattedPassword)) {
                            warningText.text.setString("Username or password incorrect.");
                        } else {
                            currentUser = User::getUser(formattedUser);
                            success = true;
                            warningText.text.setString("Account Credentials Correct! Moving to Dashboard Screen.");
                        }
                    } else if (username.size() < 4) {warningText.text.setString("Username is too short. 3 Character Minimum.");
                    } else if (password.size() < 9) {warningText.text.setString("Password is too short. 8 Character Minimum.");
                    } else {warningText.text.setString("Fill both fields before continuing.");}

                    warningText.recenter();
                    errorStartTime = std::chrono::steady_clock::now();
                    error = true;

                } else if (newAccountText.buttonClicked(mousePos)) {
                    cout << "New account button clicked" << endl;
                    currentState = State::CreateAccount;
                    return;
                } else if (forgotPasswordText.buttonClicked(mousePos)) {
                    cout << "Forgot button clicked" << endl;
                    currentState = State::ForgotPassword;
                    return;
                }


                usernameButton.sprite.setTexture(TextureManager::getUIElement(buttonState == ButtonState::User ? "button_green" : "button_red"));
                passwordButton.sprite.setTexture(TextureManager::getUIElement(buttonState == ButtonState::Password ? "button_green" : "button_red"));
            }


            //keyboard press events
            if (event.type == sf::Event::TextEntered) {
                std::string& target = (buttonState == ButtonState::User) ? username : password;

                const char ch = static_cast<char>(event.text.unicode);
                //letters A-Z, upper and lower, numbers, and three symbols
                if (isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '.' || ch == '-') {

                    if (target.size() <= 13) {
                        target.insert(target.length() - 1, 1, ch);
                    } else {
                        warningText.text.setString("Field cannot contain over 12 characters.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
                //backspace key
                if (event.text.unicode == 8) {
                    if (target.size() > 1) {
                        target.erase(target.length() - 2, 1);
                    } else {
                        warningText.text.setString("Cannot delete empty field.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
            }
            usernameText.text.setString(username);
            passwordText.text.setString(password);
        }

        window.clear(sf::Color::White);

        //draw buttons and text
        welcomeText.draw(window);
        usernameText.draw(window);
        passwordText.draw(window);
        newAccountText.draw(window);
        forgotPasswordText.draw(window);
        if (error) {warningText.draw(window);}
        usernameButton.draw(window);
        passwordButton.draw(window);
        enterButton.draw(window);
        window.display();
    }
}

void ScreenManager::createAccountScreen() {
    string username = "[Username]";
    string password = "[Password]";
    //string question = "[Question (1 Character)]";
    bool success = false;

    //setup texts
    const ButtonManager welcomeText("Create Account", font, 24, width/2.0f, height/2.0f - 200, "welcome_text", false, true);
    ButtonManager usernameText(username, font, 24, width/2.0f, height/2.0f - 20, "username_text");
    ButtonManager passwordText(password, font, 24, width/2.0f, height/2.0f + 80, "password_text");
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    const ButtonManager backToLoginText("Back to Login", font, 18,width/2.0f-150, height/2.0f+200, "error_text", true);
    warningText.text.setFillColor(sf::Color::Red);

    //setup buttons
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f-50, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f+50, "password_button");
    ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+250, height/2.0f-50, "enter_button", 0.2,0.2);

    while (window.isOpen()) {
        if (error) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - errorStartTime).count() >= 3) {
                if (success) {
                    currentState = State::Login;
                    return;
                }
                error = false;
            }
        }


        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            //mouse button events
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (usernameButton.buttonClicked(mousePos)) {
                    buttonState = ButtonState::User;
                    username = "|";

                } else if (passwordButton.buttonClicked(mousePos)) {
                    buttonState = ButtonState::Password;
                    password = "|";
                } else if (backToLoginText.buttonClicked(mousePos)) {
                    currentState = State::Login;
                    cout << "Back to login screen" << endl;
                    return;
                } else if (enterButton.buttonClicked(mousePos)) {
                    if (username.size() <= 13 && username.size() >=4 && password.size() <= 13 && password.size() >=9 && username != "[Username]" && password != "[Password]") {
                        cout << "Enter clicked, all conditions valid" << endl;

                        //format the username correctly
                        string formattedPassword = password.substr(0, password.size() - 1);
                        string formattedUser = username.substr(0, username.size() - 1);
                        formattedUser[0] = toupper(formattedUser[0]);
                        std::transform(formattedUser.begin() + 1, formattedUser.end(), formattedUser.begin() + 1, ::tolower);


                        //user already exists do not create
                        if (User::userExists(formattedUser)) {
                            warningText.text.setString("User already exists.");
                            username = "[Username]";
                            password = "[Password]";
                            usernameButton.sprite.setTexture(TextureManager::getUIElement("button_red"));
                            passwordButton.sprite.setTexture(TextureManager::getUIElement("button_red"));
                        } else {
                            //create a shared pointer and add it to the list
                            //currentUser is a weak_ptr and only references newUser

                            shared_ptr<User> newUser = std::make_shared<User>(formattedUser, formattedPassword);
                            User::addUser(newUser, formattedUser);
                            currentUser = newUser;

                            warningText.text.setString("User Created, login with username and password now.");
                            success = true;
                        }
                    } else if (username.size() < 4) {warningText.text.setString("Username is too short. 3 Character Minimum.");
                    } else if (password.size() < 9) {warningText.text.setString("Password is too short. 8 Character Minimum");
                    } else {warningText.text.setString("Fill both fields before continuing.");}

                    warningText.recenter();
                    errorStartTime = std::chrono::steady_clock::now();
                    error = true;
                }


                usernameButton.sprite.setTexture(TextureManager::getUIElement(buttonState == ButtonState::User ? "button_green" : "button_red"));
                passwordButton.sprite.setTexture(TextureManager::getUIElement(buttonState == ButtonState::Password ? "button_green" : "button_red"));
            }


            //keyboard press events
            if (event.type == sf::Event::TextEntered) {

                std::string& target = (buttonState == ButtonState::User) ? username : password;
                char ch = static_cast<char>(event.text.unicode);
                //letters A-Z, upper and lower, numbers, and three symbols
                if (isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '.' || ch == '-') {


                    if (target.size() <= 13) {
                        target.insert(target.length() - 1, 1, ch);
                    } else {
                        warningText.text.setString("Field cannot contain over 12 characters.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
                //backspace key
                if (event.text.unicode == 8) {
                    if (target.size() > 1) {
                        target.erase(target.length() - 2, 1);
                    } else {
                        warningText.text.setString("Cannot delete empty field.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
            }
            usernameText.text.setString(username);
            passwordText.text.setString(password);
        }
        window.clear(sf::Color::White);

        //draw buttons and text
        welcomeText.draw(window);
        usernameText.draw(window);
        passwordText.draw(window);
        backToLoginText.draw(window);
        if (error) {warningText.draw(window);}
        usernameButton.draw(window);
        passwordButton.draw(window);
        enterButton.draw(window);
        window.display();
    }
}

// void ScreenManager::forgotPasswordScreen() {
//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             // Close window event
//             if (event.type == sf::Event::Closed) {
//                 window.close();
//             }
//        }
//
//       // Clear the window
//         window.clear(sf::Color::Black);
//
//         // Draw things here (optional)
//
//         // Display the frame
//         window.display();
//     }
// }

void ScreenManager::dashboardScreen() {

    //check if the weak_ptr user can be accessed to avoid use-after-free error
    const shared_ptr<User> user = getCurrentUser();
    const string username = user ? user->getUsername() : "unknown";

    //save all data because adding comes back to this screen when done and usernames and password from login
    user->saveUserData();

    //setup texts and buttons

    const ButtonManager helloText("Hello, " + username, font, 28, width/2.0f, height/2.0f - 250, "add_device_text");
    const ButtonManager addDeviceText("Add Device", font, 24, width/2.0f, height/2.0f - 120, "add_device_text");
    const ButtonManager listDevicesText("List Devices", font, 24, width/2.0f, height/2.0f +30, "list_device_text");
    const ButtonManager transferDeviceText("Transfer a Device", font, 24, width/2.0f, height/2.0f + 180, "transfer_device_text");

    //setup buttons
    const ButtonManager addDeviceButton(TextureManager::getUIElement("button_grey"), width/2.0f-125, height/2.0f-200, "add_device_button", 0.2, 0.2);
    const ButtonManager listDevicesButton(TextureManager::getUIElement("button_grey"), width/2.0f-125, height/2.0f-50, "list_devices_button", 0.2, 0.2);
    const ButtonManager transferDeviceButton(TextureManager::getUIElement("button_grey"), width/2.0f-125, height/2.0f+100, "transfer_device_button", 0.2, 0.2);

    const ButtonManager logoutText("Logout", font, 24, width/2.0f+350, height/2.0f + 280, "logout_text", true);


    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            //mouse button events
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                if (addDeviceButton.buttonClicked(mousePos)) {
                    cout << "Add device button pressed" << endl;
                    currentState = State::CheckConsole;
                    return;
                }
                if (listDevicesButton.buttonClicked(mousePos)) {
                    cout << "List device button pressed" << endl;
                    currentState = State::listDevices;
                    return;
                }
                if (transferDeviceButton.buttonClicked(mousePos)) {
                    cout << "Transfer device button pressed" << endl;
                }
                if (logoutText.buttonClicked(mousePos)) {
                    currentState = State::Login;
                    return;
                }
            }

        }
        // Clear the window
        window.clear(sf::Color::White);

        // Draw things here (optional)
        addDeviceButton.draw(window);
        listDevicesButton.draw(window);
        transferDeviceButton.draw(window);
        helloText.draw(window);
        logoutText.draw(window);
        addDeviceText.draw(window);
        listDevicesText.draw(window);
        transferDeviceText.draw(window);

        // Display the frame
        window.display();
    }
}

void ScreenManager::checkConsoleScreen() {
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager checkConsoleText("Check Console for Text Input", font, 28, width/2.0f, height/2.0f, "add_device_text", false, true);

     while (window.isOpen()) {
         sf::Event event;
         while (window.pollEvent(event)) {
             //read only and compile time
             constexpr std::array<std::string_view, 5> prompts = {"Name", "Brand", "Model", "IMEI Number", "Seller"};
             //const string prompts[5] = {"Name", "Brand", "Model", "IMEI Number", "Seller"};
             string responses[5];
             cout << "=================" << endl;

             for (int i = 0; i < 5; i++) {
                 string input;
                 cout << "Enter the " << prompts[i] << " :";
                 getline(cin, input);

                 //check if name exists to add
                 while (i == 0 && user->getDevices().find(textFormatter(input)) != user->getDevices().end()) {
                     cout << "The device by the name " << input << " already exists! Enter a new one." << endl;
                     getline(cin, input);
                 }

                 while (input.length() > 16 || input.empty()) {
                     cout << "Too long or Empty! Renter the " << prompts[i] << " :";\
                     getline(cin, input);
                 }

                 responses[i] = input;
             }

             //set brand name to apple if recognized
             const unordered_map<string, string> knownBrands = {
                 {"pple", "Apple"},
                 {"sung", "Samsung"},
             };

             //brand recognition for picture of phone
             if (auto it = knownBrands.find(responses[2]); it != knownBrands.end()) {
                 responses[2] = it->second;
             }

             if (user != nullptr) {
                 user->addDevice(textFormatter(responses[0]), textFormatter(responses[1]), textFormatter(responses[2]),responses[3], textFormatter(responses[4]));
                 cout << "Device Added." << endl;
                 cout << endl;
                 currentState = State::Dashboard;
                 return;
             }


             if (event.type == sf::Event::Closed) {
                 window.close();
             }
        }

         window.clear(sf::Color::White);
         checkConsoleText.draw(window);
         window.display();
     }
}

void ScreenManager::listDevices() {
    const ButtonManager helloText("All Devices", font, 44, width/2.0f, height/2.0f -225, "all_devices", true, true);
    const ButtonManager backPageButton(TextureManager::getUIElement("back_page_button"), width/2.0f+200, height/2.0f+200, "back_page_button", 0.5, 0.5);
    const ButtonManager nextPageButton(TextureManager::getUIElement("next_page_button"), width/2.0f+350, height/2.0f+200, "next_page_button", 0.5, 0.5);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-420, height/2.0f+200, "back_button", 0.15, 0.15);

    const shared_ptr<User> user = getCurrentUser();
    const unsigned int totalPages = (user->getDevices().size() +2) / 3;
    unsigned int currentPage = 0;

    unsigned int placeCounter = 0;
    vector<ButtonManager> devices;
    devices.reserve(user->getDevices().size());


    cout << "Total Pages, total devices: " << totalPages  << ", " << user->getDevices().size() << endl;


    //add all devices as buttons with name as the text displayed.
    for (const auto& [name,device] : user->getDevices()) {
        constexpr unsigned int spacer = 100;
        const ButtonManager newDevice(device->name, font, 40, width/2.0f, height/2.0f -50 + spacer * (placeCounter%3), name);
        placeCounter++;
        devices.push_back(newDevice);
    }

//place three per page based on vector

     while (window.isOpen()) {
         sf::Event event;
         while (window.pollEvent(event)) {
             sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
             if (event.type == sf::Event::Closed) {
                 window.close();
                 currentState = State::Exit;
             }

             //mouse button events
             if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                 for (int i = 0; i < 3; ++i) {
                     const size_t index = currentPage * 3 + i;
                     if (index < devices.size()) {
                         if (devices[index].buttonClicked(mousePos)) {

                             user->setCurrentDevice(user->getDevices().at(devices[index].getMessage()));
                             cout << devices[index].getMessage() << endl;
                             currentState = State::currentDeviceScreen;
                             return;
                         }
                     }
                 }

                 //switch pages
                 if (nextPageButton.buttonClicked(mousePos) && currentPage < totalPages-1) {
                     currentPage++;
                 }
                 if (backPageButton.buttonClicked(mousePos) && currentPage > 0) {
                     currentPage--;
                 }

                 //back button, back to dashboard
                 if (backButton.buttonClicked(mousePos)) {
                     currentState = State::Dashboard;
                     return;
                 }


             }
        }

         //clear window
         window.clear(sf::Color::White);

         //Draw on Screen
         if (currentPage > 0) {backPageButton.draw(window);}
         if (currentPage < totalPages-1) {nextPageButton.draw(window);}

         //draw the items on the selected page
         for (int i = 0; i < 3; ++i) {
             const size_t index = currentPage * 3 + i;
             if (index < devices.size()) {
                 devices[index].draw(window);
             }
         }

         backButton.draw(window);
         helloText.draw(window);

         // Display the frame
         window.display();
     }
}

void ScreenManager::currentDeviceScreen() {
    const shared_ptr<User> user = getCurrentUser();
    //weak ptr currentDevice: accessible by user->getCurrentDevice();
    constexpr unsigned int spacer = 75;

    //declare buttons
    const ButtonManager helloText("Selected Device: " + user->getCurrentDevice()->name, font, 36, width/2.0f, height/2.0f -225, "selected_device", true, true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-420, height/2.0f+200, "back_button", 0.15, 0.15);
    //const ButtonManager phoneImage(TextureManager::getUIElement("phone"), width/2.0f-420, height/2.0f+200, "back_button", 0.15, 0.15);

    //text relating to device specs
    //??-> for future if adding more specs can do a map lookup for dynamic amount of values but prob not needed
    const ButtonManager brand("Brand: " + user->getCurrentDevice()->brand, font, 32, width/2.0f+100, height/2.0f -100 + spacer*0, "brand");
    const ButtonManager model("Model: " + user->getCurrentDevice()->model, font, 32, width/2.0f+100,height/2.0f -100 + spacer*1, "model");
    const ButtonManager IMEI("IMEI Number: " + user->getCurrentDevice()->imeiNumber, font, 32, width/2.0f+100, height/2.0f -100 + spacer*2, "IMEI");
    const ButtonManager seller("Seller: " + user->getCurrentDevice()->seller, font, 32, width/2.0f+100, height/2.0f -100 + spacer*3, "seller");

    //actions to do with device (coming soon!) (prob 3)

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            //mouse button events
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::listDevices;
                    return;
                }
            }
        }

        //clear window
        window.clear(sf::Color::White);


        //draw text and buttons
        backButton.draw(window);
        helloText.draw(window);

        brand.draw(window);
        model.draw(window);
        IMEI.draw(window);
        seller.draw(window);

        // Display the frame
        window.display();
    }
}