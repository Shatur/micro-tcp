#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <netinet/in.h>

using namespace std;

class Client
{
public:
    Client(const string address = "127.0.0.1", unsigned short port = 6666);
    void sendMessage(const string &message);
    void sendFile(const string &fileName);

private:
    struct sockaddr_in client;
    int socketDescriptor;
};

#endif // CLIENT_H
