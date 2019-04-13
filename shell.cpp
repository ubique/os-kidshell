//
// Created by daniil on 13.04.19.
//

#include <iostream>

using namespace std;

int main() {
    while (true) {
        std::cout << "$";
        std::cout.flush();
        std::string command;
        getline(std::cin, command);
        if (command == "exit") {
            break;
        }
    }
    
    return 0;
}
