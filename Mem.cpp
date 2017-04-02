/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Mem.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：内存模块
 ** 注  意：1.
 ********************************************************************/
#include <cstdlib>
#include "Mem.h"
#include "Log.h"
#include "Stat.h"

void *MemAlloc(unsigned long size)
{
    void *ptr = malloc(size);
    if (!ptr) {
        SYSFATAL("malloc failure!");
        exit(-1);
    }
    STAT_ADD(mem_amount, size);
    STAT(mem_count);
    return ptr;
}

void MemFree(void *ptr, unsigned long size)
{
    free(ptr);
    if (ptr) {
        STAT_DEL(mem_amount, size);
        STAT_DEL(mem_count, 1);
    }
}