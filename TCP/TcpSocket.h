#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#pragma once
#include <iostream>

#include <string>
using namespace std;
class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int socket);
    ~TcpSocket();
    int connectToHost(string ip, unsigned short port);
    int sendMsg(string msg);
    string recvMsg();


private:
    int readn(char* buf, int size);
    int writen(const char* msg, int size);
private:
    int m_fd;	// 通信的套接字


};

#endif