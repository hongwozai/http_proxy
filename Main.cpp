/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Main.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-15
 **
 ** 描  述：主函数
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Log.h"
#include "Stat.h"
#include "Server.h"
#include "Fileutils.h"

int main(int argc, char *argv[])
{
    Server server;

    if (-1 == server.init()) {
        return -1;
    }
    server.up();
    return 0;
}