cmake_minimum_required(VERSION 3.30)
project(gymapp)

set(CMAKE_CXX_STANDARD 20)

# Tell CMake where SFML is
set(SFML_DIR "C:/Users/cman2/CLionProjects/gymapp/SFML-2.5.1/lib/cmake/SFML")

# Find the SFML libraries
find_package(SFML 2.5.1 COMPONENTS graphics window system audio REQUIRED)

add_executable(gymapp main.cpp records.cpp screens.cpp textures.cpp user.h
        user.cpp
        transfers.h
        store.h
        transfers.cpp)

# Link SFML to your executable
target_link_libraries(gymapp
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)