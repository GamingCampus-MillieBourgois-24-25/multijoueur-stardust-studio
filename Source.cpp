#include <iostream>
#include <SFML/Graphics.hpp>
#include "network.h"



int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Chat Application");
    // Ask user if they want to run as server or client
    std::cout << "Run as (s)erver or (c)lient? ";
    char choice;
    std::cin >> choice;

    if (choice == 's' || choice == 'S') {
        std::cout << "Run as server." << std::endl;
        RunServer();
        return 0;
    }
    else if (choice == 'c' || choice == 'C') {
        
        std::cout << "Run as client." << std::endl;
        RunClient();
        return 1;
    }
    else {
        std::cout << "Invalid choice. Exiting." << std::endl;
    }
}