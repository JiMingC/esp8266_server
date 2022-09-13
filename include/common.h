#include <stdlib.h>
#include "typedef.h"
#include "cJSON.h"
#include "myjson.h"
#include <time.h>
#include <string.h>
#include "myiconv.h"

#define HISTROY_NUM     10

typedef struct StockInfo {
    char s_name[50];
    float c_open_val;
    float l_close_val;
    float c_val;
    float max_val;
    float min_val;
    float gain;
}StockInfo_s;

typedef struct fundInfo {
    char f_name[50];
    char f_code[30];
    float l_val;
    float c_val;
    float gain;
    float g_val;
    float g_val_l;
    float holders;
    float bid_price;
    char* time;
    char status[30];
    float histroy[HISTROY_NUM];
    int total;
    char Stock_code[10][30];
    StockInfo_s stockInfo[10];
}fundInfo_s;


struct fundSet {
    int num;
    fundInfo_s fundInfo[30];
};
