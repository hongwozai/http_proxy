/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：TermColor.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-03
 **
 ** 描  述：终端颜色
 ** 注  意：1. 定义NO_TERMCOLOR宏可以取消颜色
 ********************************************************************/
#ifndef RESOURCE_COLOR_H
#define RESOURCE_COLOR_H

#ifndef NO_TERMCOLOR
#define BRIGHTER(str)       "\e[1m"str"\e[0m"
#define UNDERLINE(str)      "\e[4m"str"\e[0m"
#define FLASHING(str)       "\e[5m"str"\e[0m"

#define BLACK(str)         "\e[30m"str"\e[0m"
#define RED(str)           "\e[31m"str"\e[0m"
#define GREEN(str)         "\e[32m"str"\e[0m"
#define YELLOW(str)        "\e[33m"str"\e[0m"
#define BLUE(str)          "\e[34m"str"\e[0m"
#define PURPLE(str)        "\e[35m"str"\e[0m"
#define CYAN(str)          "\e[36m"str"\e[0m"
#define WHITE(str)         "\e[37m"str"\e[0m"

#define BACK_BLACK(str)    "\e[40m"str"\e[0m"
#define BACK_RED(str)      "\e[41m"str"\e[0m"
#define BACK_GREEN(str)    "\e[42m"str"\e[0m"
#define BACK_YELLOW(str)   "\e[43m"str"\e[0m"
#define BACK_BLUE(str)     "\e[44m"str"\e[0m"
#define BACK_PURPLE(str)   "\e[45m"str"\e[0m"
#define BACK_CYAN(str)     "\e[46m"str"\e[0m"
#define BACK_WHITE(str)    "\e[47m"str"\e[0m"
#else
#define BRIGHTER(str)      str
#define UNDERLINE(str)     str
#define FLASHING(str)      str

#define BLACK(str)         str
#define RED(str)           str
#define GREEN(str)         str
#define YELLOW(str)        str
#define BLUE(str)          str
#define PURPLE(str)        str
#define CYAN(str)          str
#define WHITE(str)         str

#define BACK_BLACK(str)    str
#define BACK_RED(str)      str
#define BACK_GREEN(str)    str
#define BACK_YELLOW(str)   str
#define BACK_BLUE(str)     str
#define BACK_PURPLE(str)   str
#define BACK_CYAN(str)     str
#define BACK_WHITE(str)    str
#endif

#endif /* RESOURCE_COLOR_H */
