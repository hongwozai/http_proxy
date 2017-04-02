/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Fileutils.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-19
 **
 ** 描  述：文件、目录、文件名、路径名等操作
 ** 注  意：1.
 ********************************************************************/
#ifndef FILEUTILS_H
#define FILEUTILS_H

namespace File
{
    // 获得文件长度
    long FileSize(const char *name);

    // 获取文件长度,
    // buf读取至的缓冲区，
    // len读取的长度(文件长度不超则读取文件长度), 小于等于0，则拉取文件长度
    // seek从seek处开始读取文件(SEEK_SET)
    long FileFetch(const char *name, char *buf, long len = 0, long seek = 0);

    // 写入文件
    long FileSave(const char *name, char *buf, long len);

    // 获得扩展名
    char *FilenameExtension(const char *name);

    // 路径的文件名
    char *PathFilename(const char *path);

    /***************************************************************
     * 操作系统相关
     ***************************************************************/
    // 是否为目录，0不是，1是，-1是错误（没有文件等）
    int IsDir(const char *path);

    // 如果路径中有目录没创建则创建
    long PathMkdir(const char *path);
}

#endif /* FILEUTILS_H */
