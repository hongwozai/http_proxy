/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Stat.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：统计模块
 ** 注  意：1.
 ********************************************************************/
#ifndef STAT_H
#define STAT_H

#include <stdint.h>

namespace Stat
{
    extern void show();

    extern uint64_t mem_amount;
    extern uint64_t mem_count;
    extern uint64_t mem_real;

    extern uint64_t proxy_request_num;
    extern uint64_t proxy_response_num;
    extern uint64_t proxy_conn_num;

    extern uint64_t http_gzip_num;
    extern uint64_t http_chunk_num;
};

#define STAT(var)          (Stat::var ++)
#define STAT_ADD(var, num) (Stat::var += num)
#define STAT_DEL(var, num) (Stat::var -= num)

#endif /* STAT_H */
