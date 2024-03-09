#include "udpsender.hpp"

UDPSender::UDPSender(const std::string ip, uint16_t port)
{
    m_FD = socket(AF_INET, SOCK_DGRAM, 0);

    if(m_FD < 0)
        throw Exceptions::OPEN_SOCK_ERR;
    
    bzero(&m_ServAddr, sizeof(m_ServAddr));

    m_ServAddr.sin_family = AF_INET;
    m_ServAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_ServAddr.sin_port = htons(port);
}

UDPSender::~UDPSender()
{
    close(m_FD);
}

int UDPSender::Send(std::string &msg) const
{
    auto result = sendto(m_FD,
                             msg.c_str(), 
                             msg.length(), 
                             0,
                             (sockaddr*)(&m_ServAddr),
                             sizeof(m_ServAddr));
    return static_cast<int>(result);
}
