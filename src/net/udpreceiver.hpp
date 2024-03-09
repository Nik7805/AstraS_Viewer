#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

class UDPReceiver
{
    public:
    enum class Exceptions
    {
        OPEN_SOCK_ERR,
        BIND_ERR,
    };

    UDPReceiver(const char* ip, uint16_t port);
    ~UDPReceiver();
    int Receive(void* pBuffer, size_t buffSize);

    private:
    int m_FD;
    sockaddr_in m_ServAddr;
};