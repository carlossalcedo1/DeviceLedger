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


void ScreenManager::run() {
    window.create(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "Gym Tracker [app name]");
    font.loadFromFile("files/font.ttf");

    while (window.isOpen() && currentState != State::Exit ) {
        if (currentState == State::Login) {
            loginScreen();
        } else if (currentState == State::CreateAccount) {
            createAccountScreen();
        } else if (currentState == State::ForgotPassword) {
            forgotPasswordScreen();
        } else if (currentState == State::Dashboard) {
            dashboardScreen();
        } // implement rest
    }
}


void ScreenManager::loginScreen() {
    bool success = false;
    string username = "[Username]";
    string password = "[Password]";

    //setup texts
    ButtonManager welcomeText("Login", font, 24, width/2.0f, height/2.0f - 200, "welcome_text");
    ButtonManager usernameText(username, font, 24, width/2.0f, height/2.0f - 20, "username_text");
    ButtonManager passwordText(password, font, 24, width/2.0f, height/2.0f + 80, "password_text");
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    ButtonManager newAccountText("Create Account", font, 18,width/2.0f-150, height/2.0f+200, "error_text");
    ButtonManager forgotPasswordText("Forgot Password?", font, 18,width/2.0f+150, height/2.0f+200, "error_text");
    welcomeText.text.setStyle(sf::Text::Bold);
    warningText.text.setFillColor(sf::Color::Red);
    newAccountText.text.setStyle(sf::Text::Underlined);
    forgotPasswordText.text.setStyle(sf::Text::Underlined);

    //setup buttons
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f-50, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f+50, "password_button");
    ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+250, height/2.0f-50, "enter_button");
    usernameButton.sprite.setScale(0.1f, 0.1f);
    passwordButton.sprite.setScale(0.1f, 0.1f);
    enterButton.sprite.setScale(0.2f, 0.2f);


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
    std::cout << "Create Account Screen Loaded" << std::endl;
    string username = "[Username]";
    string password = "[Password]";
    //string question = "[Question (1 Character)]";
    bool success = false;

    //setup texts
    ButtonManager welcomeText("Create Account", font, 24, width/2.0f, height/2.0f - 200, "welcome_text");
    ButtonManager usernameText(username, font, 24, width/2.0f, height/2.0f - 20, "username_text");
    ButtonManager passwordText(password, font, 24, width/2.0f, height/2.0f + 80, "password_text");
    ButtonManager warningText("Hello", font, 24,width/2.0f, height/2.0f+250, "error_text");
    welcomeText.text.setStyle(sf::Text::Bold);
    warningText.text.setFillColor(sf::Color::Red);

    //setup buttons
    ButtonManager usernameButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f-50, "username_button");
    ButtonManager passwordButton(TextureManager::getUIElement("button_red"), width/2.0f - 200, height/2.0f+50, "password_button");
    ButtonManager enterButton(TextureManager::getUIElement("button_enter"), width/2.0f+250, height/2.0f-50, "enter_button");
    usernameButton.sprite.setScale(0.1f, 0.1f);
    passwordButton.sprite.setScale(0.1f, 0.1f);
    enterButton.sprite.setScale(0.2f, 0.2f);

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
        if (error) {warningText.draw(window);}
        usernameButton.draw(window);
        passwordButton.draw(window);
        enterButton.draw(window);
        window.display();
    }
}



void ScreenManager::dashboardScreen() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window event
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw things here (optional)

        // Display the frame
        window.display();
    }
}

void ScreenManager::forgotPasswordScreen() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window event
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Clear the window
        window.clear(sf::Color::Black);

        // Draw things here (optional)

        // Display the frame
        window.display();
    }
}