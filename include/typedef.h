#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
#include <stdarg.h>

#define INFO_OUTPUT      3
#define WARNING_OUTPUT   2
#define DEBUG_OUTPUT     1
#define ERROR_OUTPUT     0

#define DEBUG_LEVEL     DEBUG_OUTPUT 
//需在使用以下打印宏的.c文件中定义“DEBUG_LEVEL”变量
#define LOGI(info,...)  \
do{ \
    if(DEBUG_LEVEL>=INFO_OUTPUT){\
        printf("[Info]  %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGW(info,...)  \
do{ \
    if(DEBUG_LEVEL>=WARNING_OUTPUT){\
        printf("[Warn]  %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGD(info,...)  \
do{ \
    if(DEBUG_LEVEL>=DEBUG_OUTPUT){\
        printf("[Debug] %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define LOGE(info,...)  \
do{ \
    if(DEBUG_LEVEL>=ERROR_OUTPUT){\
        printf("[Error] %s,%d:"info"",__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

#define NONE         "\033[0m"
#define HIGH_LIGHT   "\033[1m"
#define SUB_LINE     "\033[4m"
#define FLASH        "\033[5m"
#define INVERT       "\033[7m"

#define COLOR1       "\033[0;33m"
#define COLOR2       "\033[0;34m"

#define CLEAR        "\033[2J"

#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define GreenLog(fomat,args...)  do{printf(LIGHT_GREEN);printf(fomat,##args);printf(NONE);}while(0)
#define RedLog(fomat,args...)    do{printf(LIGHT_RED);printf(fomat,##args);printf(NONE);}while(0)
#define BlueLog(fomat,args...)   do{printf(LIGHT_BLUE);printf(fomat,##args);printf(NONE);}while(0)
#define YellowLog(fomat,args...) do{printf(YELLOW);printf(fomat,##args);printf(NONE);}while(0)
#define BrownLog(fomat,args...)  do{printf(BROWN);printf(fomat,##args);printf(NONE);}while(0)
#define CyanLog(fomat,args...)   do{printf(LIGHT_CYAN);printf(fomat,##args);printf(NONE);}while(0)
#define GrayLog(fomat,args...)   do{printf(DARY_GRAY);printf(fomat,##args);printf(NONE);}while(0)
#define LightPurpleLog(fomat,args...)   do{printf(LIGHT_PURPLE);printf(fomat,##args);printf(NONE);}while(0)
#endif
