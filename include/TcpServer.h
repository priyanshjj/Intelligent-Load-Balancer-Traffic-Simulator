#pragma once

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace lb {

class TcpServer {
public:
    explicit TcpServer(int port = 8080);
    ~TcpServer();

    bool listen();
    void close();
    bool isListening() const;

private:
    int port_;
    bool listening_ = false;

#ifdef _WIN32
    SOCKET socket_ = INVALID_SOCKET;
#ifdef _WIN32
    WSADATA wsaData_{};
#endif
#else
    int socketFd_ = -1;
#endif
};

} // namespace lb
