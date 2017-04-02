/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Engine.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：epoll引擎模块
 ** 注  意：1.
 ********************************************************************/
#ifndef ENGINE_H
#define ENGINE_H

class Engine
{
public:

    // mod
    enum { LT, ET, OUT };

    // 回调返回值
    enum { NORMAL, DELETE };

public:

    typedef int (*callback)(int fd, int isclose, void *ptr);

public:

    struct handle {
        int fd;
        callback cb;
        void *args;
        int mod;
    };

public:

    int init();

    int add(int fd, int mod, callback cb, void *ptr);

    int mod(int fd, int mod, callback cb, void *ptr);

    void del(int fd);

    int loop();

    void destory();

private:

    int epollfd;

    int size;

    struct epoll_event *events;
};

#endif /* ENGINE_H */