/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Mime.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-01-19
 **
 ** 描  述：mime解析
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Mime.h"

/* 六类输入, (:), (\r), (\n), (\t ), (a-zA-Z) 其他 */
enum {
    K_COLON = 0,
    K_ENTER,
    K_NEWLINE,
    K_SPACE,
    K_ALPHA,
    K_OTHER
};

/* 五种状态,name,content,enter,newline,finishing */
enum {
    S_NAME = 0,
    S_CONT,
    S_ENTE,
    S_NEWL,
    S_FINI,
    S_END
};

/* 错误代码状态 */
#define S_ER   0xE0

/* 六类输入的映射（把字符映射为对应的类） */
unsigned char Mime::kind[1 << 8];

/* 五种状态的转移矩阵，第一项是状态，第二项是当前的输入，其值是转移到的状态 */
unsigned char Mime::grid[5][6] = {
    /*           冒号，   回车，   换行，   续行，   字母，   其他字符 */
    /* 标签 */   {S_CONT, S_ER+0, S_NAME, S_NAME, S_NAME, S_NAME,},
    /* 内容 */   {S_CONT, S_ENTE, S_CONT, S_CONT, S_CONT, S_CONT,},
    /* 回车 */   {S_CONT, S_ENTE, S_NEWL, S_CONT, S_CONT, S_CONT,},
    /* 换行 */   {S_ER+1, S_FINI, S_ER+2, S_CONT, S_NAME, S_ER+3,},
    /* 完结 */   {S_ER+4, S_ER+5, S_END , S_ER+6, S_ER+7, S_ER+8,}
};

/**
 * Lookup, Assoc, Hash用于散列
 * 该散列表的目的是对字符串与标识进行映射，线性定址法
 * 可以根据字符串取出标识
 * 限制是字符串大小写不敏感
 * 大小写字母低五位相同，第六位不同（第7位相同）
 */
int Mime::stricmp(char *str1, int size1, char *str2, int size2)
{
    int i;

    if (size1 != size2) return -1;

    for (i = 0; i + 8 < size1; i += 8) {
        if (((*(((unsigned long*)str1) + i)) & 0x1F1F1F1F1F1F1F1Ful) !=
            ((*(((unsigned long*)str2) + i)) & 0x1F1F1F1F1F1F1F1Ful))
            return -1;
    }

    for (; i + 4 < size1; i += 4) {
        if (((*(((unsigned int*)str1) + i)) & 0x1F1F1F1Ful) !=
            ((*(((unsigned int*)str2) + i)) & 0x1F1F1F1Ful))
            return -1;
    }

    for (; i + 2 < size1; i += 2) {
        if (((*(((unsigned short*)str1) + i)) & 0x1F1Ful) !=
            ((*(((unsigned short*)str2) + i)) & 0x1F1Ful))
            return -1;
    }

    for (; i < size1; i += 1) {
        if (((*(((unsigned char*)str1) + i)) & 0x1Ful) !=
            ((*(((unsigned char*)str2) + i)) & 0x1Ful))
            return -1;
    }
    return 0;
}

int Mime::hash(char *word, int size)
{
    int i;
    int index = 0;

    for (i = 0; i < size; i++) {
        index = index * 131 + (word[i] & 0x1F);
    }
    return index & (REGISTER_NUMBER - 1);
}

int Mime::lookup(char *word, int size)
{
    int index = hash(word, size);

    while (dict[index].size != 0) {
        if (stricmp(word, size, dict[index].name, dict[index].size) == 0)
            return dict[index].sign;
        index = (index + 1) & (REGISTER_NUMBER - 1);
    }
    /* 没匹配到 */
    return -1;
}

void Mime::assoc(char *word, int size, int sign)
{
    int index = hash(word, size);

    while (dict[index].size != 0)
        index = (index + 1) & (REGISTER_NUMBER - 1);

    dict[index].name = word;
    dict[index].size = size;
    dict[index].sign = sign;

}

void Mime::init(int option)
{
    memset(array, 0, sizeof(array));
    memset(dict, 0, sizeof(dict));
    // 默认输入类别K_OTHER
    memset(kind, K_OTHER, sizeof(char) * 256);

    this->option = option;

    kind[':']  = K_COLON;
    kind['\r'] = K_ENTER;
    kind['\n'] = K_NEWLINE;
    kind['\t'] = K_SPACE;
    kind[' ']  = K_SPACE;

    for (int i = 'A'; i <= 'Z'; i++)
        kind[i] = K_ALPHA;

    for (int i = 'a'; i <= 'z'; i++)
        kind[i] = K_ALPHA;

}

/**
 * setup
 * 注册MIME回调
 * @param mime
 * @param sign 标识
 * @param word 字符串
 * @param size 字符串长度
 * @param callback
 * @return -1 参数不合条件，大于0 返回sign
 */
int Mime::setup(int sign, char *word, int size, callback *cb)
{
    if (sign < 0  || sign > REGISTER_NUMBER ||
        !cb || !word || size < 0)
        return -1;

    assoc(word, size, sign);
    array[sign] = cb;
    return sign;
}

/**
 * parse
 * mime解析主函数
 * @param mime
 * @param data  所有字符的个数
 * @param size  当前灌入的所有字符，包括新添加的，之前保留的
 * @return 非-1,-2的负数 文本给出有错误；0 正在匹配，还没结束；大于0，返回mime总长度
 *         -1,-2 立即停止解析并返回
 */
int Mime::parse(stream *stream, char *data, int size, void *arg)
{
    int i, ret, call = 0;       /* ret,call返回值，i当前处理字符的指针 */
    int node = stream->node;    /* 当前状态 */
    int text = stream->text;    /* 当前已处理的指针 */
    int name = stream->name;    /* 当前name开始的指针 */
    int nlen = stream->nlen;    /* 当前name的长度，主要针对多行value的情况 */
    int cont = stream->cont;    /* 当前内容开始的指针 */

    for (i = 0; i < size - text; i++) {
        register int k = kind[(unsigned char)data[text + i]];

        switch(node) {
        case S_NAME:
            node = grid[node][k];
            if (node == S_CONT) {
                cont = text + i + 1;
                nlen = cont - name - 1;
            }
            continue;

        case S_CONT:
        case S_ENTE:
            node = grid[node][k];
            continue;

        case S_NEWL:
            node = grid[node][k];
            if (k == K_COLON || k == K_NEWLINE || k == K_OTHER)
                continue;

            /* 多行value，只调用一次时 */
            if (node == S_CONT && option == 0) continue;

            ret = lookup(data + name, nlen);

            /* 开始下一个name */
            if (node == S_NAME) { name = text + i; }

            /* 没有回调的 */
            if (ret == -1) continue;

            /* 调用回调，对于负数返回值进行处理 */
            ret = array[ret](1, data + cont, text + i - cont - 2, arg);
            if (ret <= -1 && ret >= -9) {
                call = ret;
                goto exit;
            }

            /* 多行value调用行数次 */
            if (node == S_CONT) { cont = text + i; }

            continue;

        case S_FINI:
            node = grid[node][k];
            if (node == S_END) {
                call = text + i + 1;
                goto exit;
            }
            continue;

        default:
            call = -node;
            goto exit;
        }
    }
    text += size;

exit:
    stream->node = node;
    stream->text = text;
    stream->name = name;
    stream->nlen = nlen;
    stream->cont = cont;
    return call;
}

#if 0
static char test[] =
    "abc: 345612341234\r\n"
    " asdfasdfasdfa\r\n"
    "dcb: 345234523452345asdfqwer\r\n"
    "\r\n";
int cb(int sign, char *word, int size, void *arg)
{
    int i;

    printf("sign: %d\n", sign);
    printf("word: %d ", size);
    for (i = 0; i < size; i++)
        printf("%c", word[i]);
    printf(" END\n\n");
    return 0;
}
int main(void)
{
    int ret;
    Mime mime;
    Mime::stream stream;

    memset(&stream, 0, sizeof(Mime::stream));
    mime.init(1);
    mime.setup(1, "abc", 3, cb);
    ret = mime.parse(&stream, test, sizeof(test));
    printf("\nsizeof: %lu, ret: %d\n", sizeof(test), ret);
    return 0;
}
#endif