/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Server.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-16
 **
 ** 描  述：服务器
 ** 注  意：1. 主体流程
 ********************************************************************/
#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include "Ini.h"
#include "Http.h"
#include "Engine.h"

class Server
{
public:

    int init();

    void up();

private:

    int start(uint32_t ip, uint16_t port, int blacklog = 0);

    int read_config(const char *filename);

    static int setnonblocking(int fd, int nonblock);

private:

    static int listenfd_cb(int fd, int isclose, void *ptr);

    static int clientfd_cb(int fd, int isclose, void *ptr);

    // TODO:
    // 同serverfd是同一个fd，在连接成功的时候mod fd为serverfd
    static int connectfd_cb(int fd, int isclose, void *ptr);

    static int serverfd_cb(int fd, int isclose, void *ptr);

private:

    // 服务器
    uint32_t ip;
    uint16_t port;

    // listenfd
    int sock;

private:

    Ini ini;
    static Http http;
    static Engine engine;
};

#endif /* SERVER_H */
