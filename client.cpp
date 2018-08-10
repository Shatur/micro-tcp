#include "client.h"

#include <fstream>

#include <cstring>
#include <iostream>
#include <filesystem>

#ifdef __linux__
#include <arpa/inet.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#include <io.h>
#endif

Client::Client(const string address, unsigned short port)
{
    client.sin_family = AF_INET; // IPv4
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr(address.c_str());

#ifdef _WIN32
	WSADATA wsdata;
	WSAStartup(0x0101, &wsdata);
#endif
}

Client::~Client()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void Client::sendMessage(const string &message)
{
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&client), sizeof(client)) < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    send(socketDescriptor, message.c_str(), message.size(), 0);
#ifdef __linux__
	close(socketDescriptor);
#elif _WIN32
	closesocket(socketDescriptor);
#endif

}

void Client::sendFile(const string &filePath)
{
#ifdef _WIN32
	WSADATA wsdata;
	WSAStartup(0x0101, &wsdata);
#endif

    ifstream fileStream(filePath, ios::binary);
    if (!fileStream.is_open()) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    // Conect to server
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }
    if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&client), sizeof(client)) < 0) {
#ifdef __linux__
		cout << "Error: " << strerror(errno) << endl;
#elif _WIN32
		cout << "Error: " << WSAGetLastError() << endl;
#endif
        return;
    }

    // Send file name by parts
#ifdef __linux__
	string fileName = filesystem::path(filePath).filename();
#elif _WIN32
	wstring fileNameW = filesystem::path(filePath).filename();

	// Convert to UTF-8 string
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &fileNameW[0], fileNameW.size(), NULL, 0, NULL, NULL);
	std::string  fileName(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &fileNameW[0], fileNameW.size(), &fileName[0], size_needed, NULL, NULL);
#endif
    send(socketDescriptor, fileName.c_str(), 255, 0);

    // Send file data by parts
    int part = 1;
    long totalBytes = 0;
    while (fileStream) {
        // Send part
        char buffer[64];
        fileStream.read(buffer, 64);
        int readedBytes = fileStream.gcount();
        totalBytes+= readedBytes;
        cout << "Sending bytes: " << readedBytes << ", Total: " << totalBytes << ", Part: "  << part << ", Status: ";
        send(socketDescriptor, buffer, static_cast<unsigned>(readedBytes), 0);

        // Receive status
        char status[64];
        recv(socketDescriptor, status, 64, 0);
        cout << status << endl;
        part++;
    }
    cout << "Sended " << totalBytes << " bytes" << endl;
}
