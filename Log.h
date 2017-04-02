/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Log.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：日志模块
 ** 注  意：1.
 ********************************************************************/
#ifndef LOG_H
#define LOG_H

class Log
{
public:

    enum { DEBUG, NORMAL, WARN, ERROR, FATAL, };

public:

    static Log *instance() { static Log log; return &log; }

    int init() { return 0; }

    // 按等级输出
    int log(int level, const char *fmt, ...);

    // 仅接收WARNING, ERROR, FATAL
    // 会将errno的错误语句输出出来
    int sys(int level, const char *fmt, ...);

};

#define SYSWARN(fmt, args...)                       \
    Log::instance()->sys(Log::WARN, "[%s %d]"fmt, __FILE__, __LINE__, ##args)

#define SYSERR(fmt, args...)                        \
    Log::instance()->sys(Log::ERROR, "[%s %d]"fmt, __FILE__, __LINE__, ##args)

#define SYSFATAL(fmt, args...)                      \
    Log::instance()->sys(Log::FATAL, "[%s %d]"fmt, __FILE__, __LINE__, ##args)

#define FATAL(fmt, args...)                         \
    Log::instance()->log(Log::FATAL, fmt, ##args)

#define ERR(fmt, args...)                           \
    Log::instance()->log(Log::ERROR, fmt, ##args)

#define LOG(fmt, args...)                           \
    Log::instance()->log(Log::NORMAL, fmt, ##args)

#ifdef PROXY_DEBUG
#define DBG(fmt, args...)                                               \
    Log::instance()->log(Log::DEBUG, "[%s %d]"fmt, __FILE__, __LINE__, ##args)
#else
#define DBG(fmt, args...)
#endif

#endif /* LOG_H */
