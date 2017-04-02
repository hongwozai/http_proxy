/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Fileutils.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-19
 **
 ** 描  述：文件、目录、文件名、路径名等操作
 ** 注  意：
 ********************************************************************/
#include <cstdio>
#include <cstring>
#include "Fileutils.h"

long File::FileSize(const char *name)
{
    long size;
    FILE *fp;
    fp = fopen(name, "rb");
    if (!fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

long File::FileFetch(const char *name, char *buf, long len, long seek)
{
    FILE *fp;

    if (len <= 0) {
        len = File::FileSize(name);
        if (len <= 0) {
            return -1;
        }
    }
    fp = fopen(name, "rb");
    if (!fp) {
        return -1;
    }
    fseek(fp, seek, SEEK_SET);
    len = fread(buf, len, 1, fp);
    fclose(fp);
    return len;
}

long File::FileSave(const char *name, char *buf, long len)
{
    FILE *fp;
    fp = fopen(name, "wb");
    if (!fp) {
        return -1;
    }
    fwrite(buf, len, 1, fp);
    fclose(fp);
    return len;
}

char *File::FilenameExtension(const char *name)
{
    const char *p = strrchr(name, '.');
    if (!p) return NULL;
    return (char*)(p + 1);
}

char *File::PathFilename(const char *path)
{
    const char *p = strrchr(path, '/');
    if (!p) {
        return (char*)path;
    }
    return (char*)(p + 1);
}

/***************************************************************
 * 操作系统相关
 ***************************************************************/
extern "C" {
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
}

int IsDir(const char *path)
{
    struct stat st;
    if (-1 == stat(path, &st)) {
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        return 1;
    }
    return 0;
}

long File::PathMkdir(const char *path)
{
    return 0;
}
