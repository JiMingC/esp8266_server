#include <stdio.h>
#include "include/common.h"
#include <curl/curl.h>
#include <string.h>
#include "include/myxml.h"
#include <unistd.h>
#include "include/typedef.h"
#include "include/cJSON.h"
#include "include/myjson.h"
#include "include/myiconv.h"

int f_num = 0;
long int Get_time() {
    time_t t;
    time(&t);
    //int ii = time(&t);
    struct tm *tl;
    tl = localtime(&t); 
    printf("%d-%d-%d-%d-%d-%d\n",tl->tm_year+1900,tl->tm_mon+1,tl->tm_mday,tl->tm_hour,tl->tm_min,tl->tm_sec);
    return t;
}

fundInfo_s fundInfo[30];
char data_buf[1024];
static int count = 1;
//输出到字符串再打印到屏幕上
ssize_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    strcat(stream, (char *)ptr);
    puts(stream);
    //memcpy(data_buf, (char*)stream, size*nmemb);
    return size*nmemb;
}
 
//输出到文件
/*
ssize_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}
*/

char *res_buf = NULL;
int shift;
size_t copy_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    int res_size;

    res_size = size * nmemb;
    res_buf = realloc(res_buf, shift+res_size + 1);
    memcpy(res_buf + shift, ptr, res_size);
    shift += res_size;
    return size * nmemb;
}

int fundGetDataByCode(CURL *curl, char* str, char *buf) {
    char tmp_str[2048] = {0};
    sprintf(tmp_str, "http://fund.eastmoney.com/pingzhongdata/%s.js?v=20160518155842", str);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, tmp_str);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);//-k 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);//-k
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//数据请求到以后的回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);//选择输出到字符串
        curl_easy_perform(curl);//这里是执行请求
        curl_easy_cleanup(curl);
        memcpy(buf, data_buf, sizeof(data_buf));
        return sizeof(buf);
    }
    return 0;
}

int fundGetObjFromBuf(char* curl_data, char* obj_str, char *obj_val, short Opt) {
    int num = 0;
    int step = 3;
    if (strstr(obj_str, "stockCodes"))
        step = 1;
    else if (strstr(obj_str, "Data_netWorthTrend"))
        step = 3;

    if (curl_data) {
        char *ptr = NULL;
        char *end = NULL;
        ptr = strstr(curl_data, obj_str);
        if (ptr) {
            ptr += strlen(obj_str)+step;
            if (Opt == 1)
                ptr++;
            end = ptr;
            char c = *ptr;
            unsigned char a = 1;
            while (*end != ';') {
                end++;
                num++;
            }
        }

        num = Opt ? num-1 : num;
        memset(obj_val, 0, num);
        memcpy(obj_val, ptr, num);
        //printf("end:%s\n",end);
    }
    return num;
}

/*
 *  Opt 0:funddata 1:stockcode/price 2:stockcodedata
 */
int curlDataToJson(char* src_buf, char* json_out, int Opt, char* obj_str) {
    if (Opt == 0) {
        char *ptr = src_buf;
        char *end = NULL;
        ptr += strlen("jsonpgz")+1;
        if (*ptr == ')')
            return 0;
        end = ptr;
        int num = 0;
        while(*end != ';') {
            end++;
            num++;
        }
        num--;
        memset(json_out, 0, num);
        memcpy(json_out, ptr, num);
        return num;
    } else if (Opt == 1) {
        return fundGetObjFromBuf(src_buf, obj_str, json_out, 0);
    } else if (Opt == 2) {
        char *ptr = src_buf;
        char *end = NULL;
        //ptr += 20;
        if (*ptr == '\0')
            return 0;
        end = ptr;
        int num = 0;
        while(*end != ';') {
            end++;
            num++;
        }
        //num--;
        memset(json_out, 0, num);
        memcpy(json_out, ptr, num);
        return num;
        
    }
}


void fundGetInfobyKey (char *str) {
    char *obj = (char*)malloc(1000);
    memset(obj, 0, 1000);
    fundGetObjFromBuf(res_buf, str, obj, 1);
    printf("%s\t", obj);
    free(obj);
}

void fundPriTittle(void) {
    //printf("Num\tCode\tname\t\t\t\tl_val\tc_val\t  gain\t  g_val\t get\tbid-pri  holders  total\t          histroy\n");
    printf("Code_name\t\t\tl_val\tc_val\tg_val\t\t\t\t\tgain\t get\t holders\thistroy\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------\n");
}

void fundPriSummart(void) {
    int i = 0;
    float total = 0;
    for(i = 0; i < f_num; i++) {
        total += fundInfo[i].g_val*fundInfo[i].holders;
    }
    printf("today total gain:%f\n", total);
}

#if 0
void fundGetInfo(char *buf, int len) {
    char * buf_obj = malloc(shift);
    memcpy(buf_obj, buf, len);
    fundPriTittle();
    fundGetInfobyKey("fS_code");
    fundGetInfobyKey("fS_name");
    cJSON *js;
    js = JsonParse_objectInArray(buf_obj, "y");
    printf("%.3f\t", js->valuedouble);
    js = JsonParse_objectInArray(buf_obj, "equityReturn");
    printf("%.3f\t", js->valuedouble);
    printf("\n");
    free(buf_obj);
}
#endif

int fundGetCurlData(CURL *curl, char* curl_addr) {
    if (!curl)
        return 1;
    static char str[20480];
    curl_easy_setopt(curl, CURLOPT_URL, curl_addr);//这是请求的url
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);//-k 
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);//-k
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, copy_data);//数据请求到以后的回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);//选择输出到字符串
    curl_easy_perform(curl);//这里是执行请求
//    curl_easy_cleanup(curl);
    return 0;
}

int fundStockDataparse(char* src_data, int fund_idx, int stock_idx) {
    char data[38][50];
    memset(data, 0, 38*50);
    int i = 0;
    const char s[2] = ",";
    if (src_data == NULL)
        return 1;

    int step = 0;
    if (strstr(src_data, "hk"))
        step = 1;
    char *token;
    token = strtok(src_data+20, s);
    while (token != NULL) {
        memcpy(data[i], token, strlen(token));
        i++;
        token = strtok(NULL, s);
    }
    int j;
    for (j = 0; j < i; j++) {
        
    //    LOGD("%s\n", data[j]);
    }
    //LOGD("%s\n", data[0]+1);
    char out[20] = {'\0'};
    memset(fundInfo[fund_idx].stockInfo[stock_idx].s_name, 0, 50);
    int len;
    if (step)
        len = gbk2utf8(data[1], fundInfo[fund_idx].stockInfo[stock_idx].s_name);
    else
        len = gbk2utf8(data[0]+1, fundInfo[fund_idx].stockInfo[stock_idx].s_name);

    fundInfo[fund_idx].stockInfo[stock_idx].s_name[12] = '\0';

    //printf("%s\t", fundInfo[fund_idx].stockInfo[stock_idx].s_name);
    //memcpy(fundInfo[fund_idx].stockInfo[stock_idx].s_name, out, len);
    fundInfo[fund_idx].stockInfo[stock_idx].c_open_val = atof(data[step + 1]);
    fundInfo[fund_idx].stockInfo[stock_idx].l_close_val = atof(data[step + 2]);

    fundInfo[fund_idx].stockInfo[stock_idx].c_val = atof(data[step + 3]);
    fundInfo[fund_idx].stockInfo[stock_idx].max_val = atof(data[step + 4]);
    fundInfo[fund_idx].stockInfo[stock_idx].min_val = atof(data[step + 5]);

    float gain = (fundInfo[fund_idx].stockInfo[stock_idx].c_val - fundInfo[fund_idx].stockInfo[stock_idx].l_close_val)/
                 fundInfo[fund_idx].stockInfo[stock_idx].l_close_val * 100;

    fundInfo[fund_idx].stockInfo[stock_idx].gain = gain;

    if (gain < -90)
        fundInfo[fund_idx].stockInfo[stock_idx].gain = 0;
    return 0;
}

void fundInitStockData(CURL *curl, char* code, int fund_idx) {
    char curl_addr[2048] = {0};
    int i = 0;

    char *src_js = malloc(2000);
    LOGD("%s[%s]:\n", fundInfo[fund_idx].f_name, fundInfo[fund_idx].f_code);
    for (i; i < 10; i++) {
        memset(res_buf, 0, shift);
        shift = 0;
        memset(curl_addr, 0, 2048);
        memset(src_js, 0, 2000);
        LOGD("%s\n", fundInfo[fund_idx].Stock_code[i]);
        if (fundInfo[fund_idx].Stock_code[i][6] == '1') {
            fundInfo[fund_idx].Stock_code[i][6] = '\0';
            sprintf(curl_addr, "http://hq.sinajs.cn/list=sh%s", fundInfo[fund_idx].Stock_code[i]);
        } else if (fundInfo[fund_idx].Stock_code[i][6] == '2') {
            fundInfo[fund_idx].Stock_code[i][6] = '\0';
            sprintf(curl_addr,  "http://hq.sinajs.cn/list=sz%s", fundInfo[fund_idx].Stock_code[i]);
        } else if (fundInfo[fund_idx].Stock_code[i][6] == '\0' && fundInfo[fund_idx].Stock_code[i][5] == '5'){
            fundInfo[fund_idx].Stock_code[i][5] = '\0';
            sprintf(curl_addr,  "http://hq.sinajs.cn/list=hk%s", fundInfo[fund_idx].Stock_code[i]);
        } else {
            return;
        }
        //LOGD("%s\n", curl_addr);
        fundGetCurlData(curl, curl_addr);
        int num = curlDataToJson(res_buf, src_js, 2, NULL);
        fundStockDataparse(src_js, fund_idx, i);
    }
    free(src_js);
    memset(res_buf, 0, shift);
    shift = 0;
}

void fundInitByCode(CURL *curl, char* code, int fund_idx) {
    cJSON *js, *node;
    char curl_addr[2048] = {0};
    sprintf(curl_addr, "http://fund.eastmoney.com/pingzhongdata/%s.js?v=20160518155842", code);
    fundGetCurlData(curl, curl_addr);
    char *src_js = malloc(shift);
    int num = curlDataToJson(res_buf, src_js, 1, "Data_netWorthTrend");
    if (!num) {
        LOGD("%s curl netWorthTrend data abnormal\n", code);
        free(src_js);
        return;
    }

    node = JsonParse_objectInArray(src_js, "equityReturn");
    js = cJSON_GetArrayLastItem(node, 1, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 1] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 2, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 2] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 3, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 3] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 4, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 4] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 5, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 5] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 6, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 6] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 7, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 7] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 8, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 8] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 9, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 9] = (float)js->valuedouble;
    js = cJSON_GetArrayLastItem(node, 10, "equityReturn");
    fundInfo[fund_idx].histroy[HISTROY_NUM - 10] = (float)js->valuedouble;
    free(src_js);

    //get stockCodes data
    char *stock_js = malloc(120);
    memset(stock_js, 0, 120);
    num = curlDataToJson(res_buf, stock_js, 1, "stockCodes");
    //LOGD("%d\n", num);

    //need to clear res_buf, or effect next curl data
    memset(res_buf, 0, shift);
    shift = 0;
    if (num < 3) {
        LOGD("%s curl stock data NULL\n", code);
        free(stock_js);
        return;
    }

    node = JsonParse_objectInArray(stock_js, NULL);
    int i = 0;
    for (i; i < 10 ; i++) {
        js = JsonParse_ItemInArray(node, i);
        memcpy(fundInfo[fund_idx].Stock_code[i], js->valuestring,strlen(js->valuestring));
        //LOGD("%s\t", fundInfo[fund_idx].Stock_code[i]);
    }
    free(stock_js);

}

void fundGetInfoByCode(CURL *curl, char* code) {
    cJSON *js;
    char curl_addr[2048] = {0};
    //double a, b, c;
    sprintf(curl_addr, "http://fundgz.1234567.com.cn/js/%s.js?rt=1463558676006", code);
    fundGetCurlData(curl, curl_addr);
    char * src_js = malloc(shift);
    int num = curlDataToJson(res_buf, src_js, 0, NULL);
    memset(res_buf, 0, shift);
    shift = 0;
    if (!num) {
        LOGD("%s curl data abnormal\n", code);
        free(src_js);
        return;
    }
    //printf("%3d\t", count);
    js = JsonParse_object(src_js, "fundcode");
    //printf("%s\t", js->valuestring);
    js = JsonParse_object(src_js, "name");
    if (fundInfo[count - 1].f_name[0] == '\0')
        strcpy(fundInfo[count - 1].f_name, js->valuestring);
    //printf("%-35s\t", js->valuestring);
    js = JsonParse_object(src_js, "dwjz");
    fundInfo[count - 1].l_val = (float) atof(js->valuestring);
    //printf("%s\t", js->valuestring);
    js = JsonParse_object(src_js, "gsz");
    fundInfo[count - 1].c_val = (float) atof(js->valuestring);
    //printf("%s\t\t\t\t\t", js->valuestring);
    fundInfo[count - 1].g_val_l = fundInfo[count - 1].g_val;
    fundInfo[count - 1].g_val = fundInfo[count - 1].c_val - fundInfo[count - 1].l_val;
    js = JsonParse_object(src_js, "gszzl");
    float gszzl = (float) atof(js->valuestring);
    /*
    if(gszzl > 0)
        if (gszzl > 1)
            printf(LIGHT_RED"+");
        else
            printf(RED"+");
    else
        if (gszzl < -1)
            printf(LIGHT_GREEN);
        else
            printf(GREEN);
    */
    fundInfo[count - 1].gain = (float)atof(js->valuestring);
    fundInfo[count - 1].total = (int)(fundInfo[count - 1].holders * (fundInfo[count - 1].l_val - fundInfo[count - 1].bid_price));
    //printf("%s%%  ", js->valuestring);
    count++;
    free(src_js);
}

char* code[30] = {
    "009791",
    "011613",
    "260108",
    "007349",
    "001475",
    "008888",
    "270042",
    "010755",
    "502010",
    "010125",
    "005495",
    "008086",
    "161725",
    "160221",
    "164403",
    "100038",
    "010198",
    "501058",
    "005491",
    "502023",
    "006328",
    "501005"
    "end",
};

#define CODE_NUM 30
void fundGetInfo(CURL *curl) {
    fundPriTittle();
    int i = 0;
    for(i = 0; i < CODE_NUM; i++) {
        if (strlen(code[i]) != 6)
            return;
        fundGetInfoByCode(curl, code[i]);
    }
}

void fundInitFromXml(fundInfo_s *a, CURL *curl, int num) {
    int i;
    for(i = 0; i < num; i++) {
        if(strlen((a+i)->f_code) != 6)
            continue;
        fundInitByCode(curl, (a+i)->f_code, i);
        //fundInitStockData(curl, (a+i)->f_code, i);
    }
}

void fundPriInfoPart2(fundInfo_s *a, int i) {
    int j;
    //print code num for start
    printf("%s\t", (a+i)->f_code);
    for (j = 0; j < 10; j++) {
        if ((a+i)->stockInfo[j].s_name[0] == '\0') {
            printf("\t");
            if ((j+1) == 5) {
                printf("\t\t\t\t\t\t\t   [%4d]", (a+i)->total);
                printf("\n");
                printf("\t");
            }
            continue;
        }
        printf("%s:", (a+i)->stockInfo[j].s_name);
        if((a+i)->stockInfo[j].gain > 0) {
            if ((a+i)->stockInfo[j].gain > 5)
                printf(LIGHT_RED"+");
            else
                printf(RED"+");
        }
        else if ((a+i)->stockInfo[j].gain < 0) {
            if ((a+i)->stockInfo[j].gain < -5)
                printf(LIGHT_GREEN);
            else
                printf(GREEN);
        }
        printf("%.1f%%\t", (a+i)->stockInfo[j].gain);
        printf(NONE);

        if ((j+1) == 5) {
            printf("\t\t   [%4d]", (a+i)->total);
            printf("\n");
            printf("\t");
        }
    }
    printf("\n");
}

void fundPriInfoPart1(fundInfo_s *a, int i) {
    char pribuf[1024] = {};
    int j;
    memset(pribuf, 0, 1024);

    //--------------------------------LINE 1 BEGIN
    printf(COLOR1);

    float tmp = (a+i)->g_val*(a+i)->holders;
    sprintf(pribuf, "%-38s\t%.3f\t%.3f\t%4.3f\t\t\t\t\t", (a+i)->f_name, 
                                      (a+i)->l_val, 
                                      (a+i)->c_val,
                                      (a+i)->g_val);
    
    printf("%s", pribuf);
    float gszzl = (a+i)->g_val;
    if(gszzl > 0)
        if (gszzl > 1)
            printf(LIGHT_RED"+");
        else
            printf(RED"+");
    else
        if (gszzl < -1)
            printf(LIGHT_GREEN);
        else
            printf(GREEN);

    printf("%4.2f%%  %3d", (a+i)->gain, (int)tmp);
    if ((a+i)->g_val > (a+i)->g_val_l) {
        printf(LIGHT_RED" ^");
    } else if ((a+i)->g_val < (a+i)->g_val_l) {
        printf(LIGHT_GREEN" v");
    } else {
        printf("  ");
    }
    //printf("  ");
    printf(NONE);

    printf("\t%8.2f\t", (a+i)->holders);
    for(j = 0; j < 10; j++) {
        if ((a+i)->histroy[j] > 0)
            if ((a+i)->histroy[j] > 1)
                printf(LIGHT_RED"+"NONE);
            else
                printf(RED"+"NONE);
        else
            if ((a+i)->histroy[j] < -1)
                printf(LIGHT_GREEN"-"NONE);
            else
                printf(GREEN"-"NONE);
    }

    printf("\n");
    //--------------------------------LINE 1 END
}

void fundGetInfoFromXml(fundInfo_s *a, CURL *curl, int num) {
#if 1
    printf(CLEAR);

    fundPriTittle();
    int i, j;
    for (i = 0; i < f_num; i++) {
        if(strlen((a+i)->f_code) != 6)
            return;
        fundGetInfoByCode(curl, (a+i)->f_code);
        
        fundPriInfoPart1(a, i);
        fundPriInfoPart2(a, i);
        if((i+1)%10 == 0) {
            fundPriSummart();
            sleep(25);
            printf(CLEAR);
            fundPriTittle();
        }

        if ((i + 1 ) == f_num) {
            for (j = 0; j < 10 - (f_num%10); j++) {
                printf("\n");
                printf("\n");
                printf("\n");
            }
            fundPriSummart();
            sleep(15);
            printf(CLEAR);
        }
            
    }

#else
    printf(CLEAR);
    fundPriTittle();
    int i = 0,j;
    char invert = 1;
    for(i = 0; i < 10; i++) {
        if (invert)
            printf(COLOR1);
        //else
        //    printf(COLOR2);
        //LOGD("%d %s\n", i, (a+i)->f_code);
        if(strlen((a+i)->f_code) != 6)
            continue;
        fundGetInfoByCode(curl, (a+i)->f_code);
        if((a+i)->g_val > 0 )
                printf("+");
        printf("%5.4f  ",  (a+i)->g_val);
        
        float tmp = (a+i)->g_val*(a+i)->holders;
        printf("%4d", (int)tmp);

        if ((a+i)->g_val > (a+i)->g_val_l) {
            printf(LIGHT_RED" ^");
        } else if ((a+i)->g_val < (a+i)->g_val_l) {
            printf(LIGHT_GREEN" v");
        } else {
            printf("  ");
        }


        printf(NONE);
        printf("\t%8.2f", (a+i)->holders);
        //printf("%8s\n", (a+i)->status);
        printf("  ");
        for(j = 0; j < 10; j++) {
            if ((a+i)->histroy[j] > 0)
                if ((a+i)->histroy[j] > 1)
                    printf(LIGHT_RED"+"NONE);
                else
                    printf(RED"+"NONE);
            else
                if ((a+i)->histroy[j] < -1)
                    printf(LIGHT_GREEN"-"NONE);
                else
                    printf(GREEN"-"NONE);
        }
        //printf("\t\t\t\t%5.4f", (a+i)->bid_price);
        //printf("\t\t\t%4d\t", (int)((a+i)->holders * ((a+i)->l_val - (a+i)->bid_price)));
        printf("\n");
        printf("\t\t");

        for (j = 0; j < 5; j++) {
            if ((a+i)->stockInfo[j].s_name[0] == '\0')
                continue;
            printf("%s:", (a+i)->stockInfo[j].s_name);
            if((a+i)->stockInfo[j].gain > 0) {
                if ((a+i)->stockInfo[j].gain > 5)
                    printf(LIGHT_RED"+");
                else
                    printf(RED"+");
            }
            else if ((a+i)->stockInfo[j].gain < 0) {
                if ((a+i)->stockInfo[j].gain < -5)
                    printf(LIGHT_GREEN);
                else
                    printf(GREEN);
            }
            printf("%.1f\t", (a+i)->stockInfo[j].gain);
            printf(NONE);
        }
        printf("\n");
        printf("\t\t");
        for (j = 5; j < 10; j++) {
            if ((a+i)->stockInfo[j].s_name[0] == '\0')
                continue;
            printf("%s:", (a+i)->stockInfo[j].s_name);
            if((a+i)->stockInfo[j].gain > 0) {
                if ((a+i)->stockInfo[j].gain > 5)
                    printf(LIGHT_RED"+");
                else
                    printf(RED"+");
            }
            else if ((a+i)->stockInfo[j].gain < 0) {
                if ((a+i)->stockInfo[j].gain < -5)
                    printf(LIGHT_GREEN);
                else
                    printf(GREEN);
            }
            printf("%.1f\t", (a+i)->stockInfo[j].gain);
            printf(NONE);
        }
        printf("\n");
        //invert = !invert;

    }

    fundPriSummart();

    sleep(15);
    printf(CLEAR);
    fundPriTittle();
    for(i = 10; i < num; i++) {
        if (invert)
            printf(COLOR1);
        //else
        //    printf(COLOR2);
        //LOGD("%d %s\n", i, (a+i)->f_code);
        if(strlen((a+i)->f_code) != 6)
            continue;
        fundGetInfoByCode(curl, (a+i)->f_code);
        if((a+i)->g_val > 0 )
                printf("+");
        printf("%5.4f  ",  (a+i)->g_val);
        
        float tmp = (a+i)->g_val*(a+i)->holders;
        printf("%4d", (int)tmp);

        if ((a+i)->g_val > (a+i)->g_val_l) {
            printf(LIGHT_RED" ^");
        } else if ((a+i)->g_val < (a+i)->g_val_l) {
            printf(LIGHT_GREEN" v");
        } else {
            printf("  ");
        }


        printf(NONE);
        printf("\t%8.2f", (a+i)->holders);
        //printf("%8s\n", (a+i)->status);
        printf("  ");
        for(j = 0; j < 10; j++) {
            if ((a+i)->histroy[j] > 0)
                if ((a+i)->histroy[j] > 1)
                    printf(LIGHT_RED"+"NONE);
                else
                    printf(RED"+"NONE);
            else
                if ((a+i)->histroy[j] < -1)
                    printf(LIGHT_GREEN"-"NONE);
                else
                    printf(GREEN"-"NONE);
        }
        //printf("\t\t\t\t%5.4f", (a+i)->bid_price);
        //printf("\t\t\t%4d\t", (int)((a+i)->holders * ((a+i)->l_val - (a+i)->bid_price)));
        printf("\n");
        printf("\t\t");
        for (j = 0; j < 5; j++) {
            if ((a+i)->stockInfo[j].s_name[0] == '\0')
                continue;
            printf("%s:", (a+i)->stockInfo[j].s_name);
            if((a+i)->stockInfo[j].gain > 0) {
                if ((a+i)->stockInfo[j].gain > 5)
                    printf(LIGHT_RED"+");
                else
                    printf(RED"+");
            }
            else if ((a+i)->stockInfo[j].gain < 0) {
                if ((a+i)->stockInfo[j].gain < -5)
                    printf(LIGHT_GREEN);
                else
                    printf(GREEN);
            }
            printf("%.1f\t", (a+i)->stockInfo[j].gain);
            printf(NONE);
        }
        printf("\n");
        printf("\t\t");
        for (j = 5; j < 10; j++) {
            if ((a+i)->stockInfo[j].s_name[0] == '\0')
                continue;
            printf("%s:", (a+i)->stockInfo[j].s_name);
            if((a+i)->stockInfo[j].gain > 0) {
                if ((a+i)->stockInfo[j].gain > 5)
                    printf(LIGHT_RED"+");
                else
                    printf(RED"+");
            }
            else if ((a+i)->stockInfo[j].gain < 0) {
                if ((a+i)->stockInfo[j].gain < -5)
                    printf(LIGHT_GREEN);
                else
                    printf(GREEN);
            }
            printf("%.1f\t", (a+i)->stockInfo[j].gain);
            printf(NONE);
        }
        printf("\n");
        //invert = !invert;
    }
 
    fundPriSummart();
#endif
}

void fundInfopri(fundInfo_s *a) {
    int i;
    for(i = 0; i < 30; i++) {
        LOGD("%d:%s\n", i, (a+i)->f_code);
    }
}

int fundmain(int id, int fd)
{
    Get_time();
    CURL *curl;
	CURLcode res2;
	static char str[20480];
	res2 = curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
    //fundInfo = calloc(30, sizeof(fundInfo));
    f_num = xmlLoadInfo(fundInfo);
    printf("Find %d funders, initing...\n", f_num);
    //fundInfopri(fundInfo);
    fundInitFromXml(fundInfo, curl, f_num);
    printf("Finish initing\n");
    while(1) {
        fundGetInfoFromXml(fundInfo, curl, f_num);
        //sleep(15);
        count = 1;
    }
    //fundGetInfo(curl);
	curl_global_cleanup();
#if 0
	if(curl2) 
	{
        //fp2=fopen("UsefullInfo.json", "w+");
        //curl_easy_setopt(curl2, CURLOPT_URL, "http://fund.eastmoney.com/pingzhongdata/001186.js?v=20160518155842");//这是请求的url
        curl_easy_setopt(curl2, CURLOPT_URL, "http://fundgz.1234567.com.cn/js/001186.js?rt=1463558676006");//这是请求的url
        //curl_easy_setopt(curl2, CURLOPT_POSTFIELDS, "username=root@pam&password=nicaiba_88");//这是post的内容
        //curl_easy_setopt(curl2, CURLOPT_HTTPHEADER, list);//若需要带头，则写进list
        curl_easy_setopt(curl2, CURLOPT_SSL_VERIFYPEER, 0);//-k
        curl_easy_setopt(curl2, CURLOPT_SSL_VERIFYHOST, 0);//-k
        //curl_easy_setopt(curl2, CURLOPT_VERBOSE, 1);//这是请求过程的调试log
        curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, copy_data);//数据请求到以后的回调函数
        curl_easy_setopt(curl2, CURLOPT_WRITEDATA, str);//选择输出到字符串
        //curl_easy_setopt(curl2, CURLOPT_WRITEDATA, fp2);//选择输出到文件
        res2 = curl_easy_perform(curl2);//这里是执行请求
        curl_easy_cleanup(curl2);
        //fclose(fp2);
	}
#endif
    //sql3_test();
	return 0;
}

