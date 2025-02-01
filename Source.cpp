#include <iostream>
#include <SFML/Graphics.hpp>
#include "server.h"
#include "client.h"


int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Chat Application");
    // Ask user if they want to run as server or client
    std::cout << "Run as (s)erver or (c)lient? ";
    char choice;
    std::cin >> choice;

    if (choice == 's' || choice == 'S') {
        RunServer();
    }
    else if (choice == 'c' || choice == 'C') {
        RunClient();
    }
    else {
        std::cout << "Invalid choice. Exiting." << std::endl;
    }
}