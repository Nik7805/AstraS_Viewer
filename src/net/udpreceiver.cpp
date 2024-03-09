#include "udpreceiver.hpp"

UDPReceiver::UDPReceiver(const char *ip, uint16_t port)
{
    if ( (m_FD = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    {
        throw Exceptions::OPEN_SOCK_ERR;
    }

    memset(&m_ServAddr, 0, sizeof(m_ServAddr));
    m_ServAddr.sin_family = AF_INET;
    m_ServAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr(ip);
    m_ServAddr.sin_port = htons(port);

    if ( bind(m_FD, (const struct sockaddr *)&m_ServAddr,  sizeof(m_ServAddr)) < 0 )
    {
        close(m_FD);
        throw Exceptions::BIND_ERR;
    }
}

UDPReceiver::~UDPReceiver()
{
    close(m_FD);
}

int UDPReceiver::Receive(void *pBuffer, size_t buffSize)
{
    socklen_t len;
    sockaddr_in cliaddr;
    int n; 
   
    len = sizeof(cliaddr);  //len is value/result 
   
    n = recvfrom(m_FD, (char *)pBuffer, buffSize,  
                MSG_WAITFORONE, ( struct sockaddr *) &cliaddr, 
                &len);
    return n;
}
