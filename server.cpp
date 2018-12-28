#include "server.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <filesystem>

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <io.h>
#endif

Server::Server(unsigned short port)
{
    server.sin_family = AF_INET; // IPv4
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(0); // localhost

#ifdef _WIN32
	WSADATA wsdata;
	WSAStartup(0x0101, &wsdata);
#endif
}

Server::~Server()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

bool Server::bindPort()
{
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return false;
    }

    if (bind(listener, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)) < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return false;
    }

    return true;
}

void Server::receiveMessage()
{
    listen(listener, 1);

    // Accept for connections
    socketDescriptor = accept(listener, nullptr, nullptr);
    if (socketDescriptor < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    // Receive message
    message.clear();
    char partOfMessage[64];
    while (true) {
        long receivedBytes = recv(socketDescriptor, partOfMessage, 64, 0);
        if (receivedBytes <= 0)
            break;
        message.append(partOfMessage);
    }

#ifdef __linux__
	close(socketDescriptor);
#elif _WIN32
	closesocket(socketDescriptor);
#endif
    cout << "Received message: " << message << endl;
}

void Server::receiveFile()
{
    // Check "Downloads" directory
    if (!filesystem::exists("Downloads")) {
        if (!filesystem::create_directory("Downloads")) {
#ifdef __linux__
			cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
			cout << "Error: " << WSAGetLastError() << endl;
#endif
            return;
        }
    }

    // Accept for connections
    listen(listener, 1);
    socketDescriptor = accept(listener, nullptr, nullptr);
    if (socketDescriptor < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    // Receive file name
    char fileName[255];
    recv(socketDescriptor, fileName, 255, 0);

    // Open file for writing
    ofstream outfile("Downloads/" + string(fileName), ios::binary);
    if (!outfile.is_open()) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
        close(socketDescriptor);
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
        closesocket(socketDescriptor);
#endif
        return;
    }

    // Receive file
    int part = 1;
    long totalBytes = 0;
    while (true) {
        // Receive data
        char buffer[64];
        long receivedBytes = recv(socketDescriptor, buffer, 64, 0);
        if (receivedBytes <= 0) {
            cout << "Received " << totalBytes << " bytes" << endl;
            break;
        }
        outfile.write(buffer, receivedBytes);

        // Print message
        totalBytes += receivedBytes;
        cout << "Received bytes: " << receivedBytes << ", Total: " << totalBytes << ", Part: " << part << endl;
        send(socketDescriptor, "success", 8 * sizeof(char), 0);
        part++;
    }

#ifdef __linux__
    close(socketDescriptor);
#elif _WIN32
    closesocket(socketDescriptor);
#endif
}

