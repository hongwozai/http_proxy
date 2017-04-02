/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Log.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：日志模块
 ** 注  意：1. TODO: strerror非线程安全，使用strerror_r
 ********************************************************************/
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include "Log.h"
#include "TermColor.h"

int Log::log(int level, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    switch (level) {
    case WARN:
        printf(YELLOW("Warning: "));
        break;
    case ERROR:
        printf(RED("Error: "));
        break;
    case FATAL:
        printf(RED("Fatal: "));
        break;
    }
    ret = vprintf(fmt, ap);
    va_end(ap);
    return ret;
}

int Log::sys(int level, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    switch (level) {
    case WARN:
        printf(YELLOW("Warning: "));
        break;
    case ERROR:
        printf(RED("Error: "));
        break;
    case FATAL:
        printf(RED("Fatal: "));
        break;
    }
    ret = vprintf(fmt, ap);
    printf(RED("(%s)")"\n", strerror(errno));
    va_end(ap);
    return ret;
}