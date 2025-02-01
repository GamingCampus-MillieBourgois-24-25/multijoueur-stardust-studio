#include "client.h"


void handle_client(SOCKET clientSocket, sockaddr_in clientAddr, int clientAddrSize) {
    char buffer[HAND_CHECK_BUFFER_SIZE];
    std::cout << "Client connected!" << std::endl;
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, HAND_CHECK_BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            unsigned char mode = buffer[0];
            char state[4];
            std::memcpy(state, buffer + 1, 4);

            std::cout << "Mode: " << (int)mode << std::endl;
            std::cout << "State: " << state << std::endl;
            // if ()
        }
    }
}
int RunClient() {

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    host_socket.setup("127.0.0.1", "8081");
    client_socket.setup("127.0.0.1", "8080");

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    char buffer[HAND_CHECK_BUFFER_SIZE];

    // Prepare data to send
    unsigned char mode = 1; // Sync up sockets
    const char* listen_port = "8081"; // Session name (max 30 chars)

    buffer[0] = mode;
    std::memcpy(buffer + 1, listen_port, min(strlen(listen_port), (size_t)4));
    send(client_socket.sock, buffer, sizeof(buffer), 0);

    while (true) {
        SOCKET clientSocket = accept(host_socket.sock, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            closesocket(host_socket.sock);
            cleanupWinSock();
            return 1;
        }

        std::thread(handle_client, clientSocket, clientAddr, clientAddrSize).detach();  // Handle client in a new thread

    }

    // Clean up
    closesocket(client_socket.sock);
    WSACleanup();
    return 0;
}
