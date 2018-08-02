#include <iostream>

#include "server.h"
#include "client.h"

using namespace std;

int main()
{
    cout << "Enter [s] to start as a server or [c] to start as a client (enter = [c]): ";
    string input;
    getline(cin, input);

    if (input == "s") {
        cout << "Enter port of the server (enter = \"6666\"): ";
        string port;
        getline(cin, port);
        if (port == "")
            port = "6666";

        Server server(static_cast<unsigned short>(stoul(port)));
        if (server.bindPort())
            server.receiveFile();
        return 0;
    }

    if (input == "c" || input == "") {
        cout << "Enter IP address of the server (enter = \"127.0.0.1\"): ";
        string ip;
        getline(cin, ip);
        if (ip == "")
            ip = "127.0.0.1";

        cout << "Enter port of the server (enter = \"6666\"): ";
        string port;
        getline(cin, port);
        if (port == "")
            port = "6666";

        cout << "Enter file path: ";
        string fileName;
        getline(cin, fileName);

        Client client(ip, static_cast<unsigned short>(stoul(port)));
        client.sendFile(fileName);

        return 0;
    }

    cout << "Wrong command! You must to enter [s] or [c]." << endl;
    return 0;

}
