/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Engine.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：epoll引擎模块
 ** 注  意：
 ********************************************************************/
#include <cerrno>

extern "C" {
#include <unistd.h>
#include <sys/epoll.h>
}

#include "Mem.h"
#include "Log.h"
#include "Engine.h"

#define CB(args)   (((handle*)(args))->cb)
#define ARG(arg)   (((handle*)(arg))->args)
#define FD(args)   (((handle*)(args))->fd)
#define MOD(args)  (((handle*)(args))->mod)

int Engine::init()
{
    epollfd = epoll_create(32000);
    if (epollfd == -1) {
        SYSERR("epoll create failed!");
        return -1;
    }

    size = 3200;
    events = (epoll_event*)MemAlloc(size * sizeof(epoll_event));
    return 0;
}

int Engine::add(int fd, int mod, callback cb, void *ptr)
{
    int ret;
    epoll_event ev;

    if (mod == ET) {
        ev.events = EPOLLET | EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
    } else if (mod == LT) {
        ev.events = EPOLLIN;
    } else {
        ev.events = EPOLLOUT;
    }
    ev.data.ptr = MemAlloc(sizeof(handle));
    FD(ev.data.ptr) = fd;
    ARG(ev.data.ptr) = ptr;
    CB(ev.data.ptr) = cb;
    MOD(ev.data.ptr) = mod;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    if (ret == -1) {
        if (errno == EEXIST) {
            epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        }
        SYSERR("epoll_ctl add failure!");
    }
    return 0;
}

int Engine::mod(int fd, int mod, callback cb, void *ptr)
{
    int ret;
    epoll_event ev;

    if (mod == ET) {
        ev.events = EPOLLET | EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
    } else if (mod == LT) {
        ev.events = EPOLLIN;
    } else {
        ev.events = EPOLLOUT;
    }
    ev.data.ptr = MemAlloc(sizeof(handle));
    FD(ev.data.ptr) = fd;
    ARG(ev.data.ptr) = ptr;
    CB(ev.data.ptr) = cb;
    MOD(ev.data.ptr) = mod;
    ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    if (ret == -1) {
        if (errno == ENOENT) {
            epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
        }
        SYSERR("epoll_ctl mod failure!");
    }
    return 0;
}

void Engine::del(int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

int Engine::loop()
{
    int nfd, ret, isclose = 0;
    struct epoll_event ev;

    for (;;) {
        nfd = epoll_wait(epollfd, events, size, -1);

        if (nfd == -1) {
            if (errno == EINTR)
                continue;
            SYSERR("epoll_wait error!");
            return -1;
        }

        for (int i = 0; i < nfd; i++) {
            DBG("fd: %d, events: %02X\n",
                FD(events[i].data.ptr), events[i].events);

            if (events[i].events & EPOLLRDHUP) {
                /**
                 * 对端描述符关闭触发EPOLLRDHUP
                 * 本端描述符触发EPOLLERR, EPOLLHUP
                 * 异常关闭（拔网线）。。。不触发事件
                 */
                DBG("peer conn close: %d\n", FD(events[i].data.ptr));
                isclose = 1;
            }

            ret = CB(events[i].data.ptr)
                (FD(events[i].data.ptr), isclose, ARG(events[i].data.ptr));

            if (ret == DELETE) {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, FD(events[i].data.ptr), 0);
                continue;
            }

            if (MOD(events[i].data.ptr) == ET) {
                ev.events = EPOLLET | EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
                ev.data.ptr = events[i].data.ptr;
                epoll_ctl(epollfd,
                          EPOLL_CTL_MOD,
                          FD(events[i].data.ptr),
                          &ev);
                continue;
            }
            isclose = 0;
        }

        if (nfd == size) {
            MemFree(events, size * sizeof(epoll_event));
            // extend
            size <<= 1;
            events = (epoll_event*)MemAlloc(size);
        }
    }
}

void Engine::destory()
{
    close(epollfd);
    MemFree(events, size * sizeof(epoll_event));
}
