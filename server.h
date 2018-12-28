#ifndef SERVER_H
#define SERVER_H

#include <string>

#ifdef __linux__
#include <netinet/in.h>
#elif _WIN32
#include <windows.h>
#endif


using namespace std;

class Server
{
public:
    explicit Server(unsigned short port = 6666);
	~Server();
    bool bindPort();
    void receiveMessage();
    void receiveFile();

private:
    struct sockaddr_in server;
    string message;
    int listener = -1;
    int socketDescriptor = -1;
};

#endif // SERVER_H
