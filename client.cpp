#include "client.h"

#include <fstream>

#include <cstring>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(const string address, unsigned short port)
{
    client.sin_family = AF_INET; // IPv4
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(address.c_str());
}

void Client::sendMessage(const string &message)
{
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
        cout << "Error: " << strerror(errno) << endl;
        return;
    }

    if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&client), sizeof(client)) < 0) {
        cout << "Error: " << strerror(errno) << endl;
        return;
    }

    send(socketDescriptor, message.c_str(), message.size(), 0);
    close(socketDescriptor);
}

void Client::sendFile(const string &filePath)
{
    ifstream fileStream(filePath, ios::binary);
    if (!fileStream.is_open()) {
        cout << "Error: " << strerror(errno) << endl;
        return;
    }

    // Conect to server
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
        cout << "Error: " << strerror(errno) << endl;
        return;
    }
    if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&client), sizeof(client)) < 0) {
        cout << "Error: " << strerror(errno) << endl;
        return;
    }

    // Send file name by parts
    string fileName = filesystem::path(filePath).filename();
    send(socketDescriptor, fileName.c_str(), 255, 0);

    // Send file data by parts
    int part = 1;
    long totalBytes = 0;
    while (fileStream) {
        // Send part
        char buffer[64];
        fileStream.read(buffer, 64);
        long readedBytes = fileStream.gcount();
        totalBytes+= readedBytes;
        cout << "Sending bytes: " << readedBytes << ", Total: " << totalBytes << ", Part: "  << part << ", Status: ";
        send(socketDescriptor, buffer, static_cast<unsigned long>(readedBytes), 0);

        // Receive status
        char status[64];
        recv(socketDescriptor, status, 64, 0);
        cout << status << endl;
        part++;
    }
    cout << "Sended " << totalBytes << " bytes" << endl;
}
