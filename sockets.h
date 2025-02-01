#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
void cleanupWinSock() {
    WSACleanup();
}

class Socket_binding {
public:
    SOCKET listenSocket;
    SOCKET clientSocket;

    const char* address;
    const char* port_in;
    const char* port_out;

    void Rebind_listen_socket() {
        sockaddr_in listenAddr = { 0 };
        listenAddr.sin_family = AF_INET;
        inet_pton(AF_INET, address, &listenAddr.sin_addr);
        listenAddr.sin_port = htons(atoi(port_in));

        int result = bind(listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            // closesocket(listenSocket);
            // WSACleanup();
            // exit(1);
        }

        // Listen for incoming connections
        result = listen(listenSocket, SOMAXCONN);
        if (result == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
            // closesocket(listenSocket);
            // WSACleanup();
            // exit(1);
        }
    }

    Socket_binding(const char* address_arg, const char* port_in_arg, const char* port_out_arg) {
        address = address_arg;
        port_in = port_in_arg;
        port_out = port_out_arg;

        // Create a listening socket for receiving
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }

        // Bind the listening socket to a specific IP and port
        sockaddr_in listenAddr = { 0 };
        listenAddr.sin_family = AF_INET;
        inet_pton(AF_INET, address, &listenAddr.sin_addr);
        listenAddr.sin_port = htons(atoi(port_in));

        int result = bind(listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            // closesocket(listenSocket);
            // WSACleanup();
            // exit(1);
        }

        // Listen for incoming connections
        result = listen(listenSocket, SOMAXCONN);
        if (result == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
            // closesocket(listenSocket);
            // WSACleanup();
            // exit(1);
        }



        // Create a sending socket
        SOCKET sendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sendSocket == INVALID_SOCKET) {
            std::cerr << "Send socket creation failed: " << WSAGetLastError() << std::endl;
            // closesocket(clientSocket);
            // WSACleanup();
            // exit(1);
        }

        // Bind the sending socket to a specific IP and port
        sockaddr_in sendAddr = { 0 };
        sendAddr.sin_family = AF_INET;
        inet_pton(AF_INET, address, &sendAddr.sin_addr);
        sendAddr.sin_port = htons(atoi(port_out));

        result = bind(sendSocket, (sockaddr*)&sendAddr, sizeof(sendAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Send socket bind failed: " << WSAGetLastError() << std::endl;
            // closesocket(clientSocket);
            // closesocket(sendSocket);
            // WSACleanup();
            // exit(1);
        }
        std::cout << "Server is listening on " << address << "   In:" << port_in << " Out:" << port_out << std::endl;


    }
};

class Connection_client {
public:
    SOCKET sock;
    struct sockaddr_in serverAddr;

    void setup(const char* address_arg, const char* port_arg) {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }


        // Set up server address structure
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(address_arg);
        serverAddr.sin_port = htons(atoi(port_arg));

        // Connect to server
        if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Connect failed. Error Code: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(1);
        }
    }

    Connection_client() {}
};

class Server_client {
public:
    SOCKET sock;
    struct sockaddr_in serverAddr;

    void setup(const char* address_arg, const char* port_arg) {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket creation failed. Error Code: " << WSAGetLastError() << std::endl;
            WSACleanup();
            exit(1);
        }

        // Set up server address structure
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(atoi(port_arg));

        // Bind socket
        if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed. Error Code: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(1);
        }

        // Listen for incoming connections
        if (listen(sock, 1) == SOCKET_ERROR) {
            std::cerr << "Listen failed. Error Code: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(1);
        }
    }
    Server_client() {}
};