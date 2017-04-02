/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Http.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-03-24
 **
 ** 描  述：Http
 ** 注  意：1.
 ********************************************************************/
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Mem.h"
#include "Log.h"
#include "Stat.h"
#include "Http.h"

/**
 * TODO: 暂时从简，需要整体进行修改
 */
int Http::HandleRequest(State *s, char *data, long *datalen)
{
    while (1) {
        switch(s->state) {
        case REQUEST_LINE_METHOD:
        case REQUEST_LINE_URI:
        case REQUEST_LINE_VERSION: {
            // TODO: 这里暂时按请求行不会跨包来处理
#define URL_BUF 5120
            int len, domainlen;
            char url[URL_BUF];
            char *start, *end, *middle;
            struct hostent *h;

            DBG("request line\n");
            start = strchr(data, ' ');
            if (!start) return HANDLE_REQ_DROP;
            end   = strchr(start + 1, ' ');
            if (!end) return HANDLE_REQ_DROP;
            len = end - start;
            if (len >= URL_BUF) return HANDLE_REQ_DROP;

            // 提取domain以及获取ip
            memcpy(url, start + 1, len);
            url[len] = 0;
            DBG("url: %s\n", url);
            // 跳过http://
            middle = strchr(&url[7], '/');
            if (!middle)
                return HANDLE_REQ_DROP;
            *middle = '\0';
            domainlen = strlen(url);
            // TODO: 有可能是ip
            h = gethostbyname(&url[7]);
            if (!h) return HANDLE_REQ_DROP;

            s->ip = *(uint32_t*)h->h_addr_list[0];
            s->port = 80;
            DBG("domain: %s, domainlen: %d\n", url, domainlen);
            DBG("--> dns ip: %s, port: %d\n",
                inet_ntoa(*(struct in_addr*)&(s->ip)), s->port);

            // 去掉domain
            memmove(start + 1, start + domainlen + 1,
                    *datalen - (start - data));
            *datalen -= domainlen;
            DBG("len: %d, data: \n%.*s\n", *datalen, *datalen, data);

            s->state = REQUEST_BODY;
            return HANDLE_REQ_FORWARD;
        }
        case REQUEST_HEADER:
        case REQUEST_BODY: {
            // 找method
            // TODO:这里正常情况下应该用transfer-encoding content-length来判断结尾
            if (strncmp(data, "GET ", 4) == 0 ||
                strncmp(data, "POST ", 5) == 0) {
                s->state = REQUEST_LINE_METHOD;
            }
            return HANDLE_REQ_FORWARD;
        }
        }
    }
    return 0;
}

int Http::HandleResponse(State *s, char *buf, long len)
{
    return 0;
}