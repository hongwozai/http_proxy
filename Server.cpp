/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Server.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：服务器
 ** 注  意：1.
 ********************************************************************/
#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Log.h"
#include "Mem.h"
#include "Stat.h"
#include "Server.h"
#include "Http.h"

Engine Server::engine;
Http   Server::http;

int Server::read_config(const char *filename)
{
    int ret;
    int port;
    char *value;

    ret = ini.open((char*)filename);
    if (ret == -1) {
        SYSERR("%s open failed!", filename);
        exit(-1);
    }

    // ip
    ini.get((char*)"server", (char*)"ip", &value);
    ip = inet_addr(value);
    // port
    ini.getInt((char*)"server", (char*)"port", &port);
    this->port = (uint16_t)port;

    ini.close();
    return 0;
}

int Server::setnonblocking(int fd, int nonblock)
{
    int fdflags;
    fdflags = fcntl(fd, F_GETFL, 0);
    if (fdflags == -1) {
        SYSWARN("setnonblocking fcntl failure!");
        return (-1);
    }

    if (nonblock)
        fdflags |= O_NONBLOCK;
    else
        fdflags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, fdflags) == -1) {
        SYSWARN("setnonblocking fcntl failure!");
        return (-1);
    }
    return (0);
}

int Server::init()
{
    // 读配置
    read_config("config.ini");

    // 启动服务器
    if (-1 == (sock = start(ip, port))) {
        SYSERR("start server failure!");
        return -1;
    }
    // 初始化epoll
    if (-1 == engine.init()) {
        return -2;
    }
    DBG("sock: %d\n", sock);
    engine.add(sock, Engine::LT, listenfd_cb, this);
    return 0;
}

int Server::start(uint32_t ip, uint16_t port, int blacklog)
{
    int sockfd, optval;
    struct sockaddr_in sockaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return -1;
    }
    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) == -1){
        close(sockfd);
        return -1;
    }
    sockaddr.sin_family       =  AF_INET;
    sockaddr.sin_addr.s_addr  =  ip;
    sockaddr.sin_port         =  htons(port);
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }
    if (listen(sockfd, blacklog?blacklog:SOMAXCONN) == -1) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

void Server::up()
{
    signal(SIGPIPE, SIG_IGN);
    engine.loop();
}

int Server::listenfd_cb(int fd, int isclose, void *ptr)
{
    int conn;
    socklen_t addrlen = 0;
    Http::State *state;
    struct sockaddr_in addr;

    DBG("listen fd!\n");
    conn = accept(fd, (struct sockaddr*)&addr, &addrlen);
    if (conn == -1) {
        SYSERR("accept failure!");
        exit(-1);
    }
    setnonblocking(conn, 1);

    // 创建state
    state = (Http::State*)MemAlloc(sizeof(Http::State));
    state->state = Http::REQUEST_LINE_METHOD;
    state->serverfd = -1;
    state->clientfd = fd;
    state->ip       = 0;
    state->data     = 0;
    state->len      = 0;
    state->ptr      = 0;
    engine.add(conn, Engine::ET, clientfd_cb, state);

    STAT(proxy_conn_num);
    DBG("fd: %d, client ip: %s, client port: %d\n",
        fd, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return Engine::NORMAL;
}

int Server::clientfd_cb(int fd, int isclose, void *ptr)
{
    ssize_t s;
    char *buf = NULL;
    int serverfd, ret;
    struct sockaddr_in addr;
    Http::State *state = (Http::State*)ptr;
    Http::State *server_state;

    DBG("client fd\n");
    // 处理链接关闭
    if (isclose) {
        goto close;
    }
#define SEND_BUF 5120
    // 该buf在connect时会使用
    buf = (char*)MemAlloc(SEND_BUF);
    s = read(fd, buf, SEND_BUF);
    if (s == -1 || s == 0) {
        goto close;
    }

    DBG("len: %d\n%.*s", s, s, buf);

    // 获得地址，对请求进行修改等
    ret = http.HandleRequest(state, buf, &s);
    if (ret == Http::HANDLE_REQ_DROP) {
        goto close;
    }

    // 链接已建立
    if (state->serverfd != -1) {
        // 此处直接转发
        write(state->serverfd, buf, s);
        MemFree(buf, SEND_BUF);
        buf = NULL;
        return Engine::NORMAL;
    }

    // 链接未建立
    // 将连接加入epoll
    DBG("create server connection!\n");
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        goto close;
    }
    // 设置客户端描述符的serverfd
    state->serverfd = serverfd;

    // 连接 connect
    setnonblocking(serverfd, 1);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = state->ip;
    addr.sin_port = htons(state->port);
    ret = connect(serverfd, (struct sockaddr*)&addr, sizeof(addr));

    DBG("serverfd: %d\n", serverfd);
    // 加入connect
    server_state = (Http::State*)MemAlloc(sizeof(Http::State));
    server_state->data     = buf;
    server_state->len      = s;
    server_state->truelen  = SEND_BUF;
    server_state->clientfd = fd;
    server_state->serverfd = serverfd;
    server_state->ip       = state->ip;
    server_state->port     = state->port;
    server_state->ptr      = state;
    state->ptr             = server_state;
    // connect完成在epoll中是可写!!!
    engine.add(serverfd, Engine::OUT, connectfd_cb, server_state);
    return Engine::NORMAL;

 close:
    DBG("client is close\n");
    if (state) {
        close(fd);
        engine.del(fd);
        if (state->serverfd != -1) {
            // 关闭server端
            close(state->serverfd);
            engine.del(state->serverfd);
        }
        // 关闭client的handle
        MemFree(state->ptr, sizeof(Http::State));
        MemFree(ptr, sizeof(Http::State));
        MemFree(buf, SEND_BUF);
        buf = NULL;
        state = NULL;
    }
    return Engine::DELETE;
}

int Server::connectfd_cb(int fd, int isclose, void *ptr)
{
    Http::State *s = (Http::State*)ptr;

    DBG("connect fd, event out\n");

    // 链接成功，向server写数据
    write(fd, s->data, s->len);

    MemFree(s->data, s->truelen);
    engine.mod(fd, Engine::ET, serverfd_cb, s);
    return Engine::NORMAL;
}

int Server::serverfd_cb(int fd, int isclose, void *ptr)
{
#define READ_BUF 5120
    int ret;
    ssize_t s;
    char buf[READ_BUF];
    Http::State *state = (Http::State*)ptr;

    DBG("serverfd!\n");
    if (isclose) {
        goto close;
    }

    while (1) {
        s = read(fd, buf, READ_BUF);
        if (s == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                DBG("finished\n");
                break;
            }
            SYSERR("read err!");
            goto close;
        } else if (s == 0) {
            goto close;
        }
        ret = write(state->clientfd, buf, s);
        DBG("len: %d, ret: %d\n", s, ret);
    }
    return Engine::NORMAL;

 close:
    DBG("server close!\n");
    // 关闭双方链接
    // TODO：内存泄露问题，服务器端关闭没法关闭客户端方向，两个思路
    // 1. 在state加入关联项
    // 2. 研究epoll关闭描述符时发生的事件
    if (state) {
        close(state->serverfd);
        engine.del(state->serverfd);
        close(state->clientfd);
        engine.del(state->clientfd);
        MemFree(state->ptr, sizeof(Http::State));
        MemFree(ptr, sizeof(Http::State));
        state = NULL;
    }
    return Engine::DELETE;
}