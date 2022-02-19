#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
#include <stdarg.h>

#define INFO_OUTPUT      3
#define WARNING_OUTPUT   2
#define DEBUG_OUTPUT     1
#define ERROR_OUTPUT     0

#define DEBUG_LEVEL     INFO_OUTPUT 
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

#endif
