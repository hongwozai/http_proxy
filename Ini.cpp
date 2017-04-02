/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Ini.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-06
 **
 ** 描  述：简易Ini文件格式读取
 ** 注  意：1.
 ********************************************************************/
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "Ini.h"

int Ini::open(char *file)
{
    fp = fopen(file, "r");
    if (!fp)
        return -1;
    return 0;
}

void Ini::close()
{
    if (fp)
        fclose(fp);
}

int Ini::getline()
{
    char *p = fgets(line, MAXLINE, fp);
    if (p == NULL) {
        if (feof(fp))
            return -1;
        if (ferror(fp))
            return -2;
        return -3;
    }
    llen = strlen(line);
    if (llen >= MAXLINE - 1) {
        // 认为这行超长
        tooLong = 1;
        return -4;
    }
    if (tooLong == 1) {
        // 超长行终于完结了
        tooLong = 0;
        return -5;
    }
    // 将\n换为\0，p为空应该是到文件末尾的一行
    p = strrchr(line, '\n');
    if (p) {
        *p = '\0';
        llen--;
    }
    return 0;
}

int Ini::get(char *sec, char *key, char **value)
{
    char *p, buf[MAXLINE];
    // 是否匹配到节名
    int issec = 0;
    int start = 0;

    rewind(fp);
    while (1) {
        int ret = getline();
        if (ret == -1) {
            // 文件结束，正常退出(没匹配中)
            return -1;
        } else if (ret == -2 || ret == -3) {
            // 出错，错误退出返回错误码
            return -2;
        } else if (ret == -4 || ret == -5) {
            // 继续循环
            continue;
        }

        // 正常
        for (; line[start] == ' ' || line[start] == '\t'; start++);
        if (start >= llen) {
            // 空行，继续循环
            continue;
        }

        if (line[start] == '#') {
            continue;
        } else if (line[start] == '[') {
            // section
            if (1 != sscanf(&line[start], "%*[ []%[^] ]]", buf)) {
                // 语法错误
                return -3;
            }
            if (strcmp(buf, sec) == 0) {
                issec = 1;
            } else {
                issec = 0;
            }
        } else {
            char c[MAXLINE];
            if (issec == 0)
                continue;
            if (2 != sscanf(&line[start], "%[^ =]%[ =]", buf, c)) {
                // 语法错误
                return -4;
            }
            if (strcmp(buf, key) == 0) {
                // 匹配中
                goto result;
            }
        }
    }
    return -5;
result:
    p = strchr(&line[start], '=');
    // 语法错误
    if (!p) { return 0; }
    for (p++; (*p == ' ' || *p == '\t'); p++);
    if (value)
        *value = p;
    return 1;
}

int Ini::getInt(char *sec, char *key, int *num)
{
    char *value;
    int ret = get(sec, key, &value);

    if (ret <= 0) {
        return ret;
    }

    if (num)
        *num = atoi(value);
    return 1;
}