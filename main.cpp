#include "textures.h"
#include "screens.h"
#include "user.h"

using namespace std;
std::unordered_map<std::string, std::shared_ptr<User>> User::loadedUsers;

int main() {
    //static load functions

    User::loadAllUsers();
    TextureManager::loadUIElements();

    //run the app by initialing a ScreenManager object then using the run method.
    ScreenManager app;
    app.run();

    //save all user data
    User::saveAllUsers();
    return 0;
}
