#include "screens.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>

#include "textures.h"
#include "transfers.h"
#include "user.h"
#include "transfers.h"

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
    window.create(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "Device Ledger BETA");
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
        } else if (currentState == State::transferCode) {
            transferCodeScreenSeller();
        } else if (currentState == State::transferCodeBuyer) {
            transferCodeScreenBuyer();
        } else if (currentState == State::itemList) {
            itemList();
        } else if (currentState == State::marketplace) {
            store();
        } else if (currentState == State::storeCurrentDevice) {
            storeCurrentDevice();
        } else if (currentState == State::settings) {
            settings();
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
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 150, height/2.0f-20, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 150, height/2.0f+80, "password_button");
    const ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+200, height/2.0f+25, "enter_button", 0.2, 0.2);


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
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 150, height/2.0f-20, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 150, height/2.0f+80, "password_button");
    ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+200, height/2.0f+25, "enter_button", 0.2,0.2);

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

    //load transfer codes
    user->manager.loadCodes();

    //save all data because adding comes back to this screen when done and usernames and password from login
    user->saveUserData();

    //setup texts and buttons

    const ButtonManager helloText("Hello, " + username, font, 42, width/2.0f, height/2.0f - 250, "add_device_text", true, true);
    const ButtonManager addDeviceText("Add Device", font, 18, width/2.0f-300, height/2.0f +100, "add_device_text");
    const ButtonManager listDevicesText("List Devices", font, 18, width/2.0f-100, height/2.0f +100, "list_device_text");
    const ButtonManager marketplaceText("Marketplace", font, 18, width/2.0f+100, height/2.0f + 100, "marketplace_text");
    const ButtonManager settingsText("Settings", font, 18, width/2.0f+300, height/2.0f + 100, "settings_text");

    //setup buttons
    const ButtonManager addDeviceButton(TextureManager::getUIElement("button_add"), width/2.0f-300, height/2.0f, "add_device_button", 0.2, 0.2);
    const ButtonManager listDevicesButton(TextureManager::getUIElement("button_list"), width/2.0f-100, height/2.0f, "list_devices_button", 0.2, 0.2);
    const ButtonManager marketplaceButton(TextureManager::getUIElement("button_post"), width/2.0f+100, height/2.0f, "marketplace_button", 0.2, 0.2);
    const ButtonManager settingsButton(TextureManager::getUIElement("button_settings"), width/2.0f+300, height/2.0f, "settings_button", 0.2, 0.2);

    const ButtonManager logoutText("Logout", font, 24, width/2.0f, height/2.0f-150, "logout_text", true);


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
                if (settingsButton.buttonClicked(mousePos)) {
                    cout << "Settings button pressed" << endl;
                    currentState = State::settings;
                    return;
                }

                if (marketplaceButton.buttonClicked(mousePos)) {
                    cout << "Marketplace button pressed" << endl;
                    currentState = State::marketplace;
                    return;
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
        settingsButton.draw(window);
        helloText.draw(window);
        logoutText.draw(window);
        addDeviceText.draw(window);
        listDevicesText.draw(window);
        settingsText.draw(window);
        marketplaceButton.draw(window);
        marketplaceText.draw(window);

        // Display the frame
        window.display();
    }
}

void ScreenManager::checkConsoleScreen() {
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager checkConsoleText("Check Console for Text Input", font, 28, width/2.0f, height/2.0f, "add_device_text", false, true);
    const ButtonManager helpText("(Enter information, type 'exit' to go back at any point)", font, 20, width/2.0f, height/2.0f +120, "transfer_help_text");
    //const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f, height/2.0f+100, "back_button", 0.2, 0.2);

     while (window.isOpen()) {
         sf::Event event;
         while (window.pollEvent(event)) {

             sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
             if (event.type == sf::Event::Closed) {
                 window.close();
                 currentState = State::Exit;
             }

             // if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
             //     if (backButton.buttonClicked(mousePos)) {
             //         currentState = State::Dashboard;
             //         return;
             //     }
             // }
             //read only and compile time
             constexpr std::array<std::string_view, 5> prompts = {"Name", "Brand", "Model", "IMEI Number", "Seller"};
             //const string prompts[5] = {"Name", "Brand", "Model", "IMEI Number", "Seller"};
             string responses[5];
             cout << "=================" << endl;

             for (int i = 0; i < 5; i++) {
                 string input;
                 cout << "Enter the " << prompts[i] << " :";
                 getline(cin, input);

                 //exit when prompted
                 if (input == "exit") {
                     currentState = State::Dashboard;
                     return;
                 }


                 //check if name exists to add
                 while (i == 0 && user->getDevices().find(textFormatter(input)) != user->getDevices().end()) {
                     cout << "The device by the name " << input << " already exists! Enter a new one." << endl;
                     getline(cin, input);
                 }

                 while (input.length() > 16 || input.empty()) {
                     cout << "Too long or Empty! Renter the " << prompts[i] << " :";
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
             for (const auto& it : knownBrands) {
                 if (responses[1].find(it.first) != string::npos) {
                     cout << "found" << endl;
                     responses[1] = it.second;
                 }
             }


             if (user != nullptr) {
                 user->addDevice(textFormatter(responses[0]), textFormatter(responses[1]), textFormatter(responses[2]),responses[3], textFormatter(responses[4]));
                 cout << "Device Added." << endl;
                 cout << endl;
                 currentState = State::Dashboard;
                 return;
             }


        }

         window.clear(sf::Color::White);
         checkConsoleText.draw(window);
         helpText.draw(window);
         //backButton.draw(window);
         window.display();
     }
}

void ScreenManager::listDevices() {
    //buttons (back, next page, previous page) and their respective texts below each.
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    const ButtonManager backPageButton(TextureManager::getUIElement("back_page_button"), width/2.0f+200, height/2.0f+215, "back_page_button", 0.5, 0.5);
    const ButtonManager previousText("Previous", font, 18, width/2.0f+200, height/2.0f +275, "previous_text");

    const ButtonManager nextPageButton(TextureManager::getUIElement("next_page_button"), width/2.0f+350, height/2.0f+215, "next_page_button", 0.5, 0.5);
    const ButtonManager nextText("Next", font, 18, width/2.0f+350, height/2.0f +275, "next_text");

    //logic
    const shared_ptr<User> user = getCurrentUser();
    const unsigned int totalPages = (user->getDevices().size() +2) / 3;
    //unsigned int currentPage = 0; //moved to users.h class member variable

    unsigned int placeCounter = 0;
    vector<ButtonManager> devices;
    devices.reserve(user->getDevices().size());

    const ButtonManager helloText("All Devices, Page: " + to_string(user->getPageNum()+1), font, 42, width/2.0f, height/2.0f -225, "all_devices", true, true);


    cout << "Total Pages, total devices: " << totalPages  << ", " << user->getDevices().size() << endl;


    //add all devices as buttons with name as the text displayed.
    for (const auto& [name,device] : user->getDevices()) {
        constexpr unsigned int spacer = 100;
        const ButtonManager newDevice(device->name, font, 40, width/2.0f, height/2.0f -100 + spacer * (placeCounter%3), name);
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
                     const size_t index = user->getPageNum() * 3 + i;
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
                 if (nextPageButton.buttonClicked(mousePos) && user->getPageNum() < totalPages-1) {
                     //currentPage++;
                     user->incrementPageNumber();
                 }
                 if (backPageButton.buttonClicked(mousePos) && user->getPageNum() > 0) {
                     //currentPage--;
                     user->decrementPageNumber();
                 }

                 //back button, back to dashboard
                 if (backButton.buttonClicked(mousePos)) {
                     user->zeroPageNumber();
                     currentState = State::Dashboard;
                     return;
                 }


             }
        }

         //clear window
         window.clear(sf::Color::White);

         //Draw on Screen
         if (user->getPageNum() > 0) {
             backPageButton.draw(window);
             previousText.draw(window);
         }
         if (user->getPageNum() < totalPages-1) {
             nextPageButton.draw(window);
             nextText.draw(window);
         }

         //draw the items on the selected page
         for (int i = 0; i < 3; ++i) {
             const size_t index = user->getPageNum() * 3 + i;
             if (index < devices.size()) {
                 devices[index].draw(window);
             }
         }
         backText.draw(window);
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

    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    const ButtonManager trashButton(TextureManager::getUIElement("button_trash"), width/2.0f+200, height/2.0f+225, "trash_button", 0.2, 0.2);
    const ButtonManager trashText("Delete Device", font, 16, width/2.0f+200, height/2.0f +290, "trash_text");

    const ButtonManager transferButton(TextureManager::getUIElement("button_transfer"), width/2.0f+350, height/2.0f+225, "transfer_button", 0.2, 0.2);
    const ButtonManager transferText("Start Transfer", font, 16, width/2.0f+350, height/2.0f +290, "transfer_text");

    const ButtonManager postButton(TextureManager::getUIElement("button_post"), width/2.0f+50, height/2.0f+225, "post_button", 0.2, 0.2);
    ButtonManager postText("List Device", font, 16, width/2.0f+50, height/2.0f +290, "post_text");
    postText.text.setFillColor(sf::Color::Green);

    //change name if already listed.
    if (user->getListedStatus(user->getCurrentDevice())) {
        postText.text.setString("De-List Device");
        postText.recenter();
        postText.text.setFillColor(sf::Color::Red);
    }



    ButtonManager phoneImage(TextureManager::getUIElement("logo_apple"), width/2.0f-250, height/2.0f, "phone_logo", 0.25, 0.25);
    //pick image based on brand
    if (user->getCurrentDevice()->brand == "Apple") {
        phoneImage.setTexture(TextureManager::getUIElement("logo_apple"));
    } else if (user->getCurrentDevice()->brand == "Samsung") {
        phoneImage.setTexture(TextureManager::getUIElement("logo_samsung"));
    } else {
        phoneImage.setTexture(TextureManager::getUIElement("button_hazard"));
    }




    //text relating to device specs
    //??-> for future if adding more specs can do a map lookup for dynamic amount of values but prob not needed
    const ButtonManager brand("Brand: " + user->getCurrentDevice()->brand, font, 32, width/2.0f+100, height/2.0f -120 + spacer*0, "brand");
    const ButtonManager model("Model: " + user->getCurrentDevice()->model, font, 32, width/2.0f+100,height/2.0f -120 + spacer*1, "model");
    const ButtonManager IMEI("IMEI Number: " + user->getCurrentDevice()->imeiNumber, font, 32, width/2.0f+100, height/2.0f -120 + spacer*2, "IMEI");
    const ButtonManager seller("Seller: " + user->getCurrentDevice()->seller, font, 32, width/2.0f+100, height/2.0f -120 + spacer*3, "seller");



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

                if (trashButton.buttonClicked(mousePos)) {
                    user->deleteCurrentDevice(user->getCurrentDevice()->name);
                    cout << "Device deleted." << endl;
                    currentState = State::listDevices;
                    return;
                }

                if (transferButton.buttonClicked(mousePos)) {
                    currentState = State::transferCode;
                    return;
                }

                if (postButton.buttonClicked(mousePos)) {
                    //only allow click if not listed.
                    if (!user->getListedStatus(user->getCurrentDevice())) {
                        currentState = State::itemList;
                        return;
                    } else {
                        //de-list the device.
                        //save codes and user data.
                        user->manager.deleteCode(user->getCurrentDevice()->listCode);
                        user->deListItem(user->getCurrentDevice()->name);
                        user->saveUserData();
                        user->manager.saveCodes();

                        //update screen
                        postText.text.setString("List Device");
                        postText.text.setFillColor(sf::Color::Green);
                        postText.recenter();

                        cout << "Device De-Listed. Code deleted." << endl;
                    }
                }
            }
        }

        //clear window
        window.clear(sf::Color::White);


        //draw text and buttons
        backButton.draw(window);
        backText.draw(window);
        helloText.draw(window);

        brand.draw(window);
        model.draw(window);
        IMEI.draw(window);
        seller.draw(window);

        trashButton.draw(window);
        trashText.draw(window);
        transferButton.draw(window);
        transferText.draw(window);
        postButton.draw(window);
        postText.draw(window);

        phoneImage.draw(window);

        // Display the frame
        window.display();
    }
}

void ScreenManager::transferCodeScreenSeller() {
    const shared_ptr<User> user = getCurrentUser();

    //generate transfer code
    string code = user->manager.addCode(user->getUsername(),user->getCurrentDevice()->name,user->getCurrentDevice()->brand,user->getCurrentDevice()->model, user->getCurrentDevice()->imeiNumber);

    user->manager.saveCodes();
    cout << "Code created -> " << code << endl;

    const ButtonManager helloText("Transfer Code for: " + user->getCurrentDevice()->name, font, 36, width/2.0f, height/2.0f -225, "transfer_code_text", true, true);
    const ButtonManager codeText(code, font, 72, width/2.0f, height/2.0f, "transfer_code_text", false, true);
    const ButtonManager helpText("(Go back and give this code to the buyer within 24 hours)", font, 20, width/2.0f, height/2.0f +120, "transfer_help_text");

    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");



    //device -> user->getCurrentDevice();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::currentDeviceScreen;
                    return;
                }
            }
        }


        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);

        helloText.draw(window);
        codeText.draw(window);
        helpText.draw(window);

        window.display();
    }
}

void ScreenManager::transferCodeScreenBuyer() {
    const ButtonManager helloText("Input Transfer Code", font, 36, width/2.0f, height/2.0f -225, "transfer_code_text", true, true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");
    const ButtonManager helpText("(Input 4 digit code and press Enter)", font, 20, width/2.0f, height/2.0f +120, "transfer_help_text");

    string code = "|";
    ButtonManager codeText(code, font, 72, width/2.0f, height/2.0f, "transfer_code_text", false, true);
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    warningText.text.setFillColor(sf::Color::Red);

    const shared_ptr<User> user = getCurrentUser();



    //device -> user->getCurrentDevice();
    while (window.isOpen()) {
        if (error) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - errorStartTime).count() >= 3) {
                error = false;
                if (code == "Done!") {
                    currentState = State::listDevices;
                    return;
                }
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::marketplace;
                    return;
                }
            }

            //keyboard event
            if (event.type == sf::Event::TextEntered) {
                //character
                char ch = static_cast<char>(event.text.unicode);
                //letters A-Z, upper and lower, numbers, and three symbols
                if (isalnum(static_cast<unsigned char>(ch))) {
                    if (code.size() <= 4) {
                        code.insert(code.length() - 1, 1, ch);
                    } else {
                        warningText.text.setString("Field cannot contain over 4 characters.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
                //backspace key
                if (event.text.unicode == 8) {
                    if (code.size() > 1) {
                        code.erase(code.length() - 2, 1);
                    } else {
                        warningText.text.setString("Cannot delete empty field.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }

                //enter key
                if (event.text.unicode == 13) {
                    //search for transfer codes with input
                    const auto transferCodeObject = user->manager.getCode(code.substr(0, code.size() - 1));
                    if (code.size() == 5 && transferCodeObject != nullptr && transferCodeObject->previousUser != user->getUsername()) {

                        cout << "Previous User: " << transferCodeObject->previousUser << endl;
                        cout << "Code: " << transferCodeObject->code << endl;
                        cout << "-- Device Details --" << endl;
                        cout << "Name of Device: " << transferCodeObject->deviceName << endl;
                        cout << "Brand, Model, Seller: " << transferCodeObject->deviceBrand << ", " << transferCodeObject->deviceModel << ", Secondhand" << endl;
                        cout << "IMEI: " << transferCodeObject->deviceIMEI << endl;
                        cout << "-- Transferring... --" << endl;

                        //delete transfer code from database.
                        user->manager.deleteCode(code.substr(0, code.size() - 1));
                        // add new device to current user.
                        user->addDevice(transferCodeObject->deviceName, transferCodeObject->deviceBrand, transferCodeObject->deviceModel, transferCodeObject->deviceIMEI, transferCodeObject->deviceSeller);
                        //delete device from old user.
                        if (const shared_ptr<User> previousUser = User::getUser(transferCodeObject->previousUser); previousUser != nullptr) {
                            previousUser->deleteTransferredDevice(transferCodeObject->deviceName);
                        }
                        //save codes file
                        user->manager.saveCodes();

                        cout << "Done. Check your devices :)" << endl;
                        code = "Done!";
                        warningText.text.setString("Transferring to Devices Screen...");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    } else {
                        warningText.text.setString("Code Not Found.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                        code = "|";
                    }
                }

                //find key if not show warning text
            }
            codeText.text.setString(code);
            codeText.recenter();
        }


        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);
        if (error) {warningText.draw(window);}
        codeText.draw(window);
        helloText.draw(window);
        helpText.draw(window);

        window.display();
    }
}

void ScreenManager::itemList() {
    string condition = "Used";
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager helloText("List Device", font, 36, width/2.0f, height/2.0f -225, "settings_hello_text", true, true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    ButtonManager conditionText("Toggle Condition: " + condition, font, 28, width/2.0f, height/2.0f+30, "condition_text", false, true);
    const ButtonManager helpText("(Input price/condition and press <Enter> to submit)", font, 20, width/2.0f, height/2.0f +120, "transfer_help_text");

    string price = "|";
    ButtonManager codeText("$" + price, font, 72, width/2.0f, height/2.0f-70, "transfer_code_text", false, true);
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    warningText.text.setFillColor(sf::Color::Red);

    while (window.isOpen()) {
        if (error) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - errorStartTime).count() >= 3) {
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

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::currentDeviceScreen;
                    return;
                }

                if (conditionText.buttonClicked(mousePos)) {
                    condition = (condition == "Used") ? "New" :
                    (condition == "New") ? "Refurbished" : "Used";
                }
            }

            //keyboard events
            if (event.type == sf::Event::TextEntered) {
                //character
                char ch = static_cast<char>(event.text.unicode);
                //letters A-Z, upper and lower, numbers, and three symbols
                if (isdigit(static_cast<unsigned char>(ch))) {
                    if (price.size() <= 4) {
                        price.insert(price.length() - 1, 1, ch);
                    } else {
                        warningText.text.setString("Field cannot contain over 4 integers.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }
                //backspace key
                if (event.text.unicode == 8) {
                    if (price.size() > 1) {
                        price.erase(price.length() - 2, 1);
                    } else {
                        warningText.text.setString("Cannot delete empty field.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    }
                }

                //enter key
                if (event.text.unicode == 13) {
                    if (price.size() <= 5 && price.size() > 1) {
                        //create code and show it to the user.
                        price.pop_back();
                        string code = user->manager.addMarketplaceCode(user->getUsername(),user->getCurrentDevice()->name,user->getCurrentDevice()->brand,user->getCurrentDevice()->model, user->getCurrentDevice()->imeiNumber, condition, stoi(price));
                        user->toggleListedStatus(user->getCurrentDevice());
                        user->setListCode(user->getCurrentDevice(), code);
                        user->manager.saveCodes();
                        user->saveUserData();
                        cout << "Code created -> " << code << endl;

                        //process is done person presses "Go Back" to go home.

                        warningText.text.setString("Device Listed! Click 'Go Back' to continue.");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                    } else {
                        warningText.text.setString("Price too small or too large. Range: $1-9999");
                        warningText.recenter();
                        errorStartTime = std::chrono::steady_clock::now();
                        error = true;
                        price = "|";
                    }
                }
            }
            codeText.text.setString("$" + price);
            codeText.recenter();
            conditionText.text.setString("Toggle Condition: " + condition);
            conditionText.recenter();
        }

        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);
        helloText.draw(window);

        conditionText.draw(window);
        helpText.draw(window);
        codeText.draw(window);
        if (error) {warningText.draw(window);}

        window.display();
    }
}

//SETTINGS

void ScreenManager::settings() {
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager helloText("Settings", font, 36, width/2.0f, height/2.0f -225, "settings_hello_text", true, true);
    const ButtonManager clearText("[DEBUG] Clear Global Transfer Codes", font, 24, width/2.0f, height/2.0f, "clear", true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::Dashboard;
                    return;
                }

                if (clearText.buttonClicked(mousePos)) {
                    user->manager.clearFile();
                    cout << "Cleared Global Transfer Codes." << endl;
                }
            }
        }

        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);
        clearText.draw(window);
        helloText.draw(window);

        window.display();
    }
}

void ScreenManager::store() {
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager helloText("Marketplace", font, 36, width/2.0f, height/2.0f -225, "marketplace", true, true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    const ButtonManager transferButton(TextureManager::getUIElement("button_transfer"), width/2.0f+350, height/2.0f+225, "transfer_button", 0.2, 0.2);
    const ButtonManager transferText("Input Code", font, 16, width/2.0f+350, height/2.0f +290, "transfer_text");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::Dashboard;
                    return;
                }

                if (transferButton.buttonClicked(mousePos)) {
                    currentState = State::transferCodeBuyer;
                    return;
                }
            }
        }

        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);
        helloText.draw(window);
        transferButton.draw(window);
        transferText.draw(window);

        window.display();
    }
}

void ScreenManager::storeCurrentDevice() {
    const shared_ptr<User> user = getCurrentUser();
    const ButtonManager helloText("Device", font, 36, width/2.0f, height/2.0f -225, "settings_hello_text", true, true);
    const ButtonManager backButton(TextureManager::getUIElement("button_back"), width/2.0f-350, height/2.0f+225, "back_button", 0.2, 0.2);
    const ButtonManager backText("Go Back", font, 18, width/2.0f-350, height/2.0f +290, "back_page");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (event.type == sf::Event::Closed) {
                window.close();
                currentState = State::Exit;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (backButton.buttonClicked(mousePos)) {
                    currentState = State::Dashboard;
                    return;
                }
            }
        }

        window.clear(sf::Color::White);

        backButton.draw(window);
        backText.draw(window);
        helloText.draw(window);

        window.display();
    }
}
