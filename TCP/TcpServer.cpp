#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TcpServer.h"
#include "TcpSocket.h"

using namespace std;

TcpServer::TcpServer()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);    // 创建套接字
}

TcpServer::~TcpServer()
{
    close(m_fd);    // 关闭套接字
}

int TcpServer::setListen(unsigned short port)
{
    /*
     * 创建一个IPv4的地址结构体，指定监听端口
     * INADDR_ANY表示监听所有本地IP地址
     */
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;     // AF_INET表示IPv4协议
    saddr.sin_port = htons(port);   // 网络字节序转换
    saddr.sin_addr.s_addr = INADDR_ANY;  // 监听所有IP地址

    // 将套接字和地址绑定
    int ret = bind(m_fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("bind");     // 输出错误信息
        return -1;  // 返回错误标志
    }
    cout << "套接字绑定成功, ip: "
        << inet_ntoa(saddr.sin_addr)    // 将网络字节序IP地址转换成点分十进制形式
        << ", port: " << port << endl;

    // 开始监听端口，等待客户端连接
    ret = listen(m_fd, 128);    // 同时监听的客户端连接请求的最大数目为128
    if (ret == -1)
    {
        perror("listen");   // 输出错误信息
        return -1;  // 返回错误标志
    }
    cout << "设置监听成功..." << endl;

    return ret;
}

TcpSocket* TcpServer::acceptConn(sockaddr_in* addr)
{
    // 如果用户不关心客户端地址信息，则返回空指针
    if (addr == NULL)
    {
        return nullptr;
    }

    socklen_t addrlen = sizeof(struct sockaddr_in);
    // 接受客户端连接请求，并获取客户端地址信息
    int cfd = accept(m_fd, (struct sockaddr*)addr, &addrlen);
    if (cfd == -1)
    {
        perror("accept");   // 输出错误信息
        return nullptr;     // 返回空指针表示接受连接失败
    }
    printf("成功和客户端建立连接...\n");

    // 返回指向已连接TcpSocket对象的指针
    return new TcpSocket(cfd);
}

struct SockInfo
{
    TcpServer* s;
    TcpSocket* tcp;
    struct sockaddr_in addr;
};

void* working(void* arg)
{
    struct SockInfo* pinfo = static_cast<struct SockInfo*>(arg);
    // 连接建立成功, 打印客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP: %s, 端口: %d\n",
        inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
        ntohs(pinfo->addr.sin_port));

    // 5. 通信
    while (1)
    {
        printf("接收数据: .....\n");
        string msg = pinfo->tcp->recvMsg();
        if (!msg.empty())
        {
            cout << msg << endl << endl << endl;
        }
        else
        {
            break;
        }
    }
    delete pinfo->tcp;
    delete pinfo;
    return nullptr;
}

int main()
{
    // 1. 创建监听的套接字
    TcpServer s;
    // 2. 绑定本地的IP port并设置监听
    s.setListen(10000);
    // 3. 阻塞并等待客户端的连接
    while (1)
    {
        SockInfo* info = new SockInfo;
        TcpSocket* tcp = s.acceptConn(&info->addr);
        if (tcp == nullptr)
        {
            cout << "重试...." << endl;
            continue;
        }
        // 创建子线程
        pthread_t tid;
        info->s = &s;
        info->tcp = tcp;

        pthread_create(&tid, NULL, working, info);
        pthread_detach(tid);
    }

    return 0;
}



