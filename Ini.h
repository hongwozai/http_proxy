/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Ini.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-06
 **
 ** 描  述：简易Ini文件格式读取
 ** 注  意：1.
 ********************************************************************/
#ifndef RESOURCE_INI_H
#define RESOURCE_INI_H

#include <cstdio>

class Ini
{
public:

    enum { MAXLINE = 4096 };

public:

    int open(char *file);

    void close();

    int get(char *sec, char *key, char **value);

    int getInt(char *sec, char *key, int *num);

private:

    int getline();

    // 找第一个非空字符
    int strnospace(char *p, int len, int start = 0);

private:

    FILE *fp;

    // 当前行
    char  line[MAXLINE];

    // 当前行长度
    int   llen;

    // 当前行是否超长
    int   tooLong;
};

#endif /* RESOURCE_INI_H */
