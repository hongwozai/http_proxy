/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Mime.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-01-19
 **
 ** 描  述：Mime解析
 ** 注  意：1.
 ********************************************************************/
#ifndef RESOURCE_MIME_H
#define RESOURCE_MIME_H

class Mime
{
public:

    /**
     * 最多注册该设置个name及回调，标识同样不能超过该设置，且只能为正数
     * 必须是2的幂次
     */
    enum { REGISTER_NUMBER = 128 };

public:

    // 流状态保存
    struct stream {
        int node;                   /* 当前所处状态 */
        int text;                   /* 文本指针 */
        int name;                   /* NAME起始位置指针 */
        int nlen;                   /* 当前name的长度
                                       这个是为了多行value进行回调的 */
        int cont;                   /* :之后的第一个字节的指针 */
    };

    // value对应的回调
    typedef int (callback)(int sign, char *data, int len, void *arg);

public:

    // 初始化参数
    void init(int option);

    // 注册回调
    int setup(int sign, char *word, int size, callback *cb);

    // 开始分析
    int parse(stream *s, char *data, int size, void *arg = 0);

private:

    // 回调的标记
    struct sign {
        char *name;
        int   size;
        int   sign;
    };

private:

    // hash函数
    int hash(char *word, int size);

    // hash表的查看
    int lookup(char *word, int size);

    // hash表的插入
    void assoc(char *word, int size, int sign);

    // hash表的匹配
    int stricmp(char *str1, int size1, char *str2, int size2);

private:

    /* 六类输入的映射（把字符映射为对应的类） */
    static unsigned char kind[256];

    /* 五种状态的转移矩阵，第一项是状态，第二项是当前的输入，其值是转移到的状态 */
    static unsigned char grid[5][6];

private:

    // 选项，对多行value是否多次调用
    int option;

    // 名称与符号的映射
    sign dict[REGISTER_NUMBER];

    // 回调函数数组，标识索引回调
    callback *array[REGISTER_NUMBER];
};

#endif /* RESOURCE_MIME_H */
