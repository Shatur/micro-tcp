#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <netinet/in.h>

using namespace std;

class Server
{
public:
    Server(unsigned short port = 6666);
    bool bindPort();
    void receiveMessage();
    void receiveFile();

private:
    struct sockaddr_in server;
    string message;
    int listener;
    int socketDescriptor;
};

#endif // SERVER_H
