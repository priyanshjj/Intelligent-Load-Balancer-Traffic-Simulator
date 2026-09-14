#include "TcpServer.h"

namespace lb {

TcpServer::TcpServer(int port) : port_(port) {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
        listening_ = false;
    }
#endif
}

TcpServer::~TcpServer() {
    close();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool TcpServer::listen() {
#ifdef _WIN32
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<u_short>(port_));
    address.sin_addr.s_addr = INADDR_ANY;

    if (::bind(socket_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }

    if (::listen(socket_, SOMAXCONN) != 0) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }

    listening_ = true;
    return true;
#else
    socketFd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ < 0) {
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<uint16_t>(port_));
    address.sin_addr.s_addr = INADDR_ANY;

    if (::bind(socketFd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
        ::close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    if (::listen(socketFd_, SOMAXCONN) != 0) {
        ::close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    listening_ = true;
    return true;
#endif
}

void TcpServer::close() {
#ifdef _WIN32
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
#else
    if (socketFd_ >= 0) {
        ::close(socketFd_);
        socketFd_ = -1;
    }
#endif
    listening_ = false;
}

bool TcpServer::isListening() const {
    return listening_;
}

} // namespace lb
