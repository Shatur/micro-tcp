#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#ifdef __linux__
#include <netinet/in.h>
#elif _WIN32
#include <windows.h>
#endif

using namespace std;

class Client
{
public:
    Client(const string address = "127.0.0.1", unsigned short port = 6666);
    ~Client();
    void sendMessage(const string &message);
    void sendFile(const string &fileName);

private:
    struct sockaddr_in client;
    int socketDescriptor;
};

#endif // CLIENT_H
