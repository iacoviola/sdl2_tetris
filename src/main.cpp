#include <iostream>

#include "App.hpp"

int main(int argc, char* argv[]){
    try {
        App app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}