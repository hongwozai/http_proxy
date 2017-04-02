/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Stat.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：统计模块
 ** 注  意：1.
 ********************************************************************/
#include "Log.h"
#include "Stat.h"

using namespace Stat;

uint64_t Stat::mem_amount;
uint64_t Stat::mem_count;
uint64_t Stat::mem_real;

uint64_t Stat::proxy_request_num;
uint64_t Stat::proxy_response_num;
uint64_t Stat::proxy_conn_num;

uint64_t Stat::http_gzip_num;
uint64_t Stat::http_chunk_num;

void Stat::show()
{
}