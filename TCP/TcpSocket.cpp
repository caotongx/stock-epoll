#include "TcpSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
using namespace std;


TcpSocket::TcpSocket()
{
    m_fd = socket(AF_INET,SOCK_STREAM,0); // 创建一个 TCP 套接字
}

TcpSocket::TcpSocket(int socket)
{
    m_fd = socket; // 将传入的套接字设置为成员变量
}

TcpSocket::~TcpSocket(){
    if (m_fd>0){
        close(m_fd); // 关闭套接字
    }
}

int TcpSocket::connectToHost(string ip,unsigned short port){
    struct sockaddr_in saddr; // 套接字地址结构体
    saddr.sin_family = AF_INET; // 地址族，指定为 IPv4
    saddr.sin_port = htons(port); // 网络字节序的端口号，使用传入的参数
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr); // 转换 IP 地址为网络字节序，并存放到地址结构体中
    int ret = connect(m_fd, (struct sockaddr*)&saddr, sizeof(saddr)); // 连接服务器
    if (ret == -1) // 连接失败
    {
        perror("connect"); // 输出错误信息
        return -1; // 返回错误码
    }
    cout << "成功和服务器建立连接..." << endl; // 连接成功，输出信息
    return ret; // 返回成功码
}

int TcpSocket::sendMsg(string msg)
{
    // 申请内存空间: 数据长度 + 包头4字节(存储数据长度)
    char* data = new char[msg.size() + 4]; // 动态申请内存，存放数据和数据长度
    int bigLen = htonl(msg.size()); // 将数据长度转换为网络字节序，并存放到 bigLen 中
    memcpy(data, &bigLen, 4); // 将 bigLen 拷贝到 data 的前 4 字节中
    memcpy(data + 4, msg.data(), msg.size()); // 将数据拷贝到 data 的第 5 字节开始的空间中
    // 发送数据
    int ret = writen(data, msg.size() + 4); // 根据写入数据的大小返回的值，如果发生错误则返回 -1
    delete[]data; // 释放内存空间，防止内存泄漏
    return ret; // 返回写入数据的大小
}

string TcpSocket::recvMsg()
{
    // 接收数据
    // 1. 读数据头
    int len = 0; // 数据长度
    readn((char*)&len, 4); // 读取数据头，并将数据转换为 int 类型的 len
    len = ntohl(len); // 将 len 从网络字节序转换为主机字节序
    cout << "数据块大小: " << len << endl; // 输出数据块大小

    // 根据读出的长度分配内存
    char* buf = new char[len + 1]; // 根据读出的长度再次动态申请内存空间，大小为数据长度 + 1
    int ret = readn(buf, len); // 读取数据块，并返回读取的字节数
    if (ret != len) // 如果读取的字节数不等于数据长度
    {
        return string(); // 返回空字符串
    }
    buf[len] = '\0'; // 在字符串结尾添加 '\0'
    string retStr(buf); // 将 buf 转换为 string 类型
    delete[]buf; // 释放内存空间
    return retStr; // 返回读取的字符串
}

int TcpSocket::readn(char* buf, int size)
{
    int nread = 0; // 已经读取的字节数
    int left = size; // 剩余未读取的字节数
    char* p = buf; // 读取的指针

    while (left > 0) // 当剩余未读取的字节数大于 0 时
    {
        if ((nread = read(m_fd, p, left)) > 0) // 读取数据，并返回读取的字节数，如果发生错误则返回 -1
        {
            p += nread; // 将读指针后移
            left -= nread; // 将未读取的字节数减去已经读取的字节数
        }
        else if (nread == -1) // 如果读取出错
        {
            return -1; // 返回错误码
        }
    }
    return size; // 返回读取的字节数
}

int TcpSocket::writen(const char* msg, int size)
{
    int left = size; // 剩余未读取的字节数
    int nwrite = 0; // 已经写入的字节数
    const char* p = msg; // 写入的指针

    while (left > 0) // 当剩余未写入的字节数大于 0 时
    {
        if ((nwrite = write(m_fd, msg, left)) > 0) // 写入数据，并返回写入的字节数，如果发生错误则返回 -1
        {
            p += nwrite; // 将写指针后移
            left -= nwrite; // 将未写入的字节数减去已经写入的字节数
        }
        else if (nwrite == -1) // 如果写入出错
        {
            return -1; // 返回错误码
        }
    }
    return size; // 返回写入的字节数
}
/*
int main()
{
    // 1. 创建通信的套接字
    TcpSocket tcp;

    // 2. 连接服务器IP port
    int ret = tcp.connectToHost("192.168.237.131", 10000);
    if (ret == -1)
    {
        return -1;
    }

    // 3. 通信
    int fd1 = open("english.txt", O_RDONLY);
    int length = 0;
    char tmp[100];
    memset(tmp, 0, sizeof(tmp));
    while ((length = read(fd1, tmp, sizeof(tmp))) > 0)
    {
        // 发送数据
        tcp.sendMsg(string(tmp, length));

        cout << "send Msg: " << endl;
        cout << tmp << endl << endl << endl;
        memset(tmp, 0, sizeof(tmp));

        // 接收数据
        usleep(300);
    }

    sleep(10);

    return 0;
}
*/
