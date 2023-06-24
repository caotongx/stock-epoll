#ifndef TCPSERVER_H
#define TCPSERVER_H

#pragma once
#include <netinet/in.h>
#include "TcpSocket.h"
#include <cstring>

using namespace std;
class TcpServer
{
public:
    TcpServer();
    ~TcpServer();
    int setListen(unsigned short port);
    TcpSocket* acceptConn(struct sockaddr_in* addr = nullptr);

private:
    int m_fd;	// 监听的套接字
};


#endif