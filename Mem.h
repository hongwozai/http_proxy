/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Mem.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：内存模块
 ** 注  意：1.
 ********************************************************************/
#ifndef MEM_H
#define MEM_H

void *MemAlloc(unsigned long size);

void MemFree(void *ptr, unsigned long size);

#endif /* MEM_H */
