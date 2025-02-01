#include <iostream>
#include <SFML/Graphics.hpp>
#include "network.h"

int ChooseMode()
{
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
    return 1;
}

int game()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Online - TicTacToe");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
    return 1;
}

int main()
{
    std::thread thread_B(game);
    std::thread thread_A(ChooseMode);

    thread_B.join();
    thread_A.join();

    return 0;
}