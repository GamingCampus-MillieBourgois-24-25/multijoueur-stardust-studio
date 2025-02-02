#include <winsock2.h>
#include <string>
#include "sockets.h"
#include <thread>
#include <queue>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT_CLIENT "8081"
#define PORT_HOST "8080"
#define HAND_CHECK_BUFFER_SIZE 5
#define BUFFER_SIZE 1024
WSADATA wsaData;

Server_client host_socket;
Connection_client client_socket;
std::queue<std::string> send_events;
std::queue<std::string> recived_events;

std::vector<sf::Text> chatMessages;
sf::Font font;
sf::Text text(font);
sf::Text chat(font);
std::string userInput;

const sf::RenderStates sf::RenderStates::Default;



char last_player = '?';
char board[3][3] = { {'#', '#', '#'}, {'#', '#', '#'}, {'#', '#', '#'} };
void server_mode() {
    host_socket.setup("127.0.0.1", "8080");
    std::cout << "Waiting for client connection" << std::endl;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket;
    bool waiting_for_connection = true;
    while (waiting_for_connection == true) {
        clientSocket = accept(host_socket.sock, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            closesocket(host_socket.sock);
            cleanupWinSock();
            exit(1);
        }

        std::cout << "Client connected!" << std::endl;
        char buffer[HAND_CHECK_BUFFER_SIZE];

        // Retrieve and print client address
        char clientIP[INET_ADDRSTRLEN]; // For IPv4 addresses
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        // std::cout << "Client connected from IP: " << clientIP << ", Port: " << clientPort << "\n";

        while (waiting_for_connection == true) {
            int bytesReceived = recv(clientSocket, buffer, HAND_CHECK_BUFFER_SIZE, 0);
            if (bytesReceived > 0) {
                // Decode the received data
                unsigned char mode = buffer[0];
                char target_port[5];
                std::memcpy(target_port, buffer + 1, 4);
                // target_port[5] = '\0'; // Null-terminate the session name

                std::cout << "Mode: " << (int)mode << std::endl;
                // std::cout << "Size: " << (int)size << std::endl;
                std::cout << "Target Port: " << target_port << std::endl;
                if ((int)mode == 1) {
                    Sleep(1000);
                    client_socket.setup("127.0.0.1", target_port);

                    std::cout << "sent hand check answer" << std::endl;
                    char buffer[HAND_CHECK_BUFFER_SIZE];
                    buffer[0] = 2;
                    const char* status = "OK";
                    std::memcpy(buffer + 1, status, min(strlen(status), (size_t)4));
                    send(client_socket.sock, buffer, sizeof(buffer), 0);
                    waiting_for_connection = false;
                }
            }
        }
    }

    std::cout << "Main game loop" << std::endl;
    char buffer[BUFFER_SIZE];
    // Sleep(1000);
    // buffer[0] = '4';
    // char* message = "Hello";
    // std::memcpy(buffer + 1, message, strlen(message));
    // send(client_socket.sock, buffer, sizeof(buffer), 0);
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            // std::cout << "Recieved: " << buffer << std::endl;
            recived_events.push(buffer);
        }
    }


    closesocket(clientSocket);
}

void client_mode() {
    host_socket.setup("127.0.0.1", PORT_CLIENT);
    std::cout << "Trying server connection" << std::endl;
    client_socket.setup("127.0.0.1", PORT_HOST);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    char buffer_[HAND_CHECK_BUFFER_SIZE];

    // Prepare data to send
    unsigned char mode = 1; // Sync up sockets
    const char* listen_port = PORT_CLIENT; // Session name (max 30 chars)

    buffer_[0] = mode;
    std::memcpy(buffer_ + 1, listen_port, min(strlen(listen_port), (size_t)4));
    send(client_socket.sock, buffer_, sizeof(buffer_), 0);
    SOCKET clientSocket;

    bool waiting_for_responce = true;
    while (waiting_for_responce == true) {
        clientSocket = accept(host_socket.sock, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            closesocket(host_socket.sock);
            cleanupWinSock();
            exit(1);
        }

        char buffer[HAND_CHECK_BUFFER_SIZE];
        std::cout << "Client connected!" << std::endl;
        while (waiting_for_responce == true) {
            int bytesReceived = recv(clientSocket, buffer, HAND_CHECK_BUFFER_SIZE, 0);
            if (bytesReceived > 0) {
                unsigned char mode = buffer[0];
                char state[2];
                std::memcpy(state, buffer + 1, 2);

                std::cout << "Mode: " << (int)mode << std::endl;
                std::cout << "State: " << state << std::endl;
                if ((int)mode == 2) {
                    waiting_for_responce = false;
                }
            }
        }
    }
    std::cout << "Main game loop" << std::endl;
    char buffer[BUFFER_SIZE];
    // Sleep(1000);
    // char* message = "Hello";
    // std::memcpy(buffer + 1, message, strlen(message));
    // send(client_socket.sock, buffer, sizeof(buffer), 0);

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            // std::cout << "Recieved: " << buffer << std::endl;
            recived_events.push(buffer);
        }
    }
}

char check_win() {
    for (int y = 0; y < 3; y++) {
        if (board[0][y] == board[1][y] && board[1][y] == board[2][y] && board[2][y] != '#') return board[2][y];
        if (board[y][0] == board[y][1] && board[y][1] == board[y][2] && board[y][2] != '#') return board[y][2];
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[2][2] != '#') return board[2][2];

    if (board[2][0] == board[1][1] && board[1][1] == board[0][2] && board[0][2] != '#') return board[0][2];
    return '#';
}

void send_message(std::string message) {
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = '\0';
    }

    unsigned char mode = 6;
    buffer[0] = mode;

    std::memcpy(buffer + 1, message.c_str(), message.size());
    send(client_socket.sock, buffer, sizeof(buffer), 0);

    sf::Text messageText(font);
    messageText.setString(message);
    messageText.setCharacterSize(10);
    messageText.setPosition(sf::Vector2f(825, 770 - chatMessages.size() * 20));
    chatMessages.push_back(messageText);
}

void send_board(char board[3][3]) {
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = '\0';
    }

    unsigned char mode = 4;
    buffer[0] = mode;

    buffer[1] = board[0][0];
    buffer[2] = board[0][1];
    buffer[3] = board[0][2];
    buffer[4] = board[1][0];
    buffer[5] = board[1][1];
    buffer[6] = board[1][2];
    buffer[7] = board[2][0];
    buffer[8] = board[2][1];
    buffer[9] = board[2][2];
    buffer[10] = '\0';


    // std::cout << "Flatten board:" << buffer << std::endl;


    // std::memcpy(buffer + 1, flattened_board, 9);
    send(client_socket.sock, buffer, sizeof(buffer), 0);
}

void send_last_player(char player_char) {
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = '\0';
    }
    buffer[0] = 5;
    buffer[1] = player_char;
    send(client_socket.sock, buffer, sizeof(buffer), 0);
}

void process_event(std::string event) {
    // std::cout << "Mode: " << (int)event[0] << std::endl; 
    char data[BUFFER_SIZE - 1];
    std::memcpy(data, event.c_str() + 1, BUFFER_SIZE - 1);
    // std::cout << "Data: " << data << std::endl;
    if ((int)event[0] == 6) {
        // Display the received message in the chat window
        std::string message(data);  // Assuming the message is in the data array
        std::cout << "Received message: " << message << std::endl;

        // Add the received message to the chat window
        sf::Text messageText(font);
        messageText.setString(message);
        messageText.setCharacterSize(10);
        messageText.setPosition(sf::Vector2f(825, 770 - chatMessages.size() * 20));
        chatMessages.push_back(messageText);
    }
    if ((int)event[0] == 5) {
        last_player = event[1];
    }
    else if ((int)event[0] == 4) {
        int idx = 1;
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                board[x][y] = event[idx];
                idx += 1;
            }
        }

        char winner = check_win();

        if (winner != '#') {
            std::cout << "### System ###: Winner " + std::string(1, winner) + " !" << std::endl;
            send_message("### System ###: Winner " + std::string(1, winner) + " !");
            for (int x = 0; x < 3; x++) {
                for (int y = 0; y < 3; y++) {
                    board[x][y] = '#';
                }
            }
            send_board(board);
        }
    }
    else if ((int)event[0] == 6) {
        std::cout << data << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::string mode;
    if (argc == 2) {
        mode = argv[1];
        std::cout << "Mode: " << mode << std::endl;
    }
    else {
        std::cout << "Usage: mode (client/server)" << std::endl;
        return 1;
    }

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    if (mode == "server") {
        std::thread(server_mode).detach();
    }
    else if (mode == "client") {
        std::thread(client_mode).detach();
    }
    else {
        std::cout << "Invalid mode (" << mode << ")" << std::endl;
        return 1;
    }

    if (!font.openFromFile("Ac437_IBM_BIOS.ttf"))
    {
        std::cerr << "Wasn't able to load font";
        return 1;
    }

    char current_player_char;

    if (mode == "server") {
        current_player_char = 'x';
    }
    else if (mode == "client") {
        current_player_char = 'o';
    }

    //font.loadFromFile("Ac437_IBM_BIOS.ttf");
    sf::RenderWindow window(sf::VideoMode({1020,800}), "Tic Tac Toe");

    const int BOARD_SIZE = 3;
    const int ROW_SPACING = 800 / 3;
    const int COLUMN_SPACING = 800 / 3;

    chat.setFont(font);
    chat.setCharacterSize(10);
    chat.setString("Start typing, and press enter to send");
    chat.setPosition(sf::Vector2f(685, 770));


    while (true) 
    {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                int x = sf::Mouse::getPosition(window).x / COLUMN_SPACING;
                int y = sf::Mouse::getPosition(window).y / ROW_SPACING;
                std::cout << "Played at: (" << x << ", " << y << ")" << std::endl;
                if (last_player != current_player_char) {
                    if (board[x][y] == '#') {
                        board[x][y] = current_player_char;
                        last_player = current_player_char;
                        send_board(board);
                        send_last_player(last_player);
                    }
                }

            }
            else if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                char32_t unicodeChar = textEvent->unicode;

                if (unicodeChar >= 32 && unicodeChar != 127) {
                    userInput += unicodeChar;
                }
                // Handle backspace
                else if (unicodeChar == 8 && !userInput.empty()) {
                    userInput.pop_back();
                }

                chat.setString(userInput);
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                {

                    send_message(userInput);
                    userInput.clear();
                    //chat.setString("");
                    
                }
            }

        }


        if (recived_events.size() != 0) {
            std::cout << "Recieved: " << recived_events.front() << std::endl;

            process_event(recived_events.front());
            recived_events.pop();
        }

        window.clear();
        if (last_player != current_player_char) {
            text.setFillColor(sf::Color::Green);
        }
        else {
            text.setFillColor(sf::Color::White);
        }
        text.setFont(font);
        text.setCharacterSize(24);
        if (last_player == 'o') {
            text.setString("Turn: x");
        }
        else {
            text.setString("Turn: o");
        }

        text.setPosition(sf::Vector2f(0, 800 - (24 * 1.25)));

        window.draw(text);
        for (auto& message : chatMessages)
            window.draw(message);
        window.draw(chat);
    

        text.setFillColor(sf::Color::White);
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                text.setFont(font);
                text.setCharacterSize(800 / 3);
                text.setString(board[x][y]);
                text.setPosition(sf::Vector2f((300 / 3)* (x) * 3, (300 / 3)* y * 2.5));
                window.draw(text);
            }
        }
        window.display();
    }
}

