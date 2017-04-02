/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Http.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-24
 **
 ** 描  述：http解析
 ** 注  意：
 ********************************************************************/
#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>

class Server;
class Http
{
public:

    // State中的状态记录
    enum {
        // 请求
        REQUEST_LINE_METHOD,
        REQUEST_LINE_URI,
        REQUEST_LINE_VERSION,
        REQUEST_HEADER,
        REQUEST_BODY,
        // TODO: 响应
    };

    // 请求处理完成之后的动作（handle request的返回值）
    enum {
        HANDLE_REQ_DROP,
        HANDLE_REQ_FORWARD,
    };

public:

    // clientfd与serverfd(包括connect时)都使用该结构体
    // connect时state无效，clientfd时为对请求的分析，serverfd时为对响应的分析
    struct State {
        // (client, server都使用)记录当前状态
        int state;
        // (serverfd时起作用)对于server关联的描述符
        int clientfd;
        // (clientfd时起作用)对于client关联的描述符
        int serverfd;
        // (clientfd时起作用，传出时使用)链接的网站ipv4,port
        uint32_t ip;
        uint16_t port;
        // (仅在connect时起作用)数据内容
        char *data;
        int   len;
        int   truelen;
        // 关联的ptr
        State *ptr;
    };

public:

    int HandleRequest(State *s, char *data, long *len);

    int HandleResponse(State *s, char *data, long len);
};

#endif /* HTTP_H */
