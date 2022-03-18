#include "include/utf_handle.h"
#include "include/typedef.h"
#include "string.h"

#define PARSE_ERROR  1
#define BOOL   _Bool

#define GB2312_START    0xB0A0
#define FONTS_NUM_BLOCK     94
#define FONTS_BYTE_NUM      32

#define UTF_GB2312_TABLE    "utfTogb2312_table.bin"
#define GB2313_TFT_SIZE      216631

extern const unsigned char bin_data[];
/*
Unicode符号范围     |        UTF-8编码方式
(十六进制)        |              （二进制）
----------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
/**
 * @brief utf8编码转unicode字符集(usc4)，最大支持4字节utf8编码，(4字节以上在中日韩文中为生僻字)
 * @param *utf8 utf8变长编码字节集1~4个字节
 * @param *unicode utf8编码转unicode字符集结果，最大4个字节，返回的字节序与utf8编码序一致
 * @return length 0：utf8解码异常，others：本次utf8编码长度
 */
uint8_t UTF8ToUnicode(uint8_t *utf8, uint32_t *unicode) {
    const uint8_t lut_size = 3;
    const uint8_t length_lut[] = {2, 3, 4};
    const uint8_t range_lut[] = {0xE0, 0xF0, 0xF8};
    const uint8_t mask_lut[] = {0x1F, 0x0F, 0x07};

    uint8_t length = 0;
    unsigned int b = *(utf8 + 0);
    uint32_t i = 0;

    if(utf8 == NULL) {
        *unicode = 0;
        return 1;
    }
    // utf8编码兼容ASCII编码,使用0xxxxxx 表示00~7F
    if(b < 0x80) {
        *unicode =  b;
        return 1;
    }
    // utf8不兼容ISO8859-1 ASCII拓展字符集
    // 同时最大支持编码6个字节即1111110X
    if(b < 0xC0 || b > 0xFD) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    for(i = 0; i < lut_size; i++) {
        if(b < range_lut[i]) {
            *unicode = b & mask_lut[i];
            length = length_lut[i];
            break;
        }
    }
    // 超过四字节的utf8编码不进行解析
    if(length == 0) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    // 取后续字节数据
    for(i = 1; i < length; i++ ) {
        b = *(utf8 + i);
        // 多字节utf8编码后续字节范围10xxxxxx‬~‭10111111‬
        if( b < 0x80 || b > 0xBF ) {
            break;
        }
        *unicode <<= 6;
        // ‭00111111‬
        *unicode |= (b & 0x3F);
    }
    // 长度校验
    return (i < length) ? PARSE_ERROR : length;
}



/**
 * @brief 4字节unicode(usc4)字符集转utf16编码
 * @param unicode unicode字符值
 * @param *utf16 utf16编码结果
 * @return utf16长度，(2字节)单位
 */
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t *utf16) {
    // Unicode范围 U+000~U+FFFF
    // utf16编码方式：2 Byte存储，编码后等于Unicode值
    if(unicode <= 0xFFFF) {
        if(utf16 != NULL) {
            *utf16 = (unicode & 0xFFFF);
        }
        return 1;
    }else if( unicode <= 0xEFFFF ) {
        if( utf16 != NULL ) {
            // 高10位
            *(utf16 + 0) = 0xD800 + (unicode >> 10) - 0x40;
            // 低10位
            *(utf16 + 1) = 0xDC00 + (unicode &0x03FF);
        }
        return 2;
    }

    return 0;
}



/**
 * @brief 使用utf16.lut查找表查询unicode对应的gb2312编码
 * @brief unicode小端方式输入，gb2312大端方式输出
 * @param unicode 双字节unicode码, 小端模式
 * @return gb2312 gb2312字符集，大端模式
 * */
unsigned short bsearch_gb2312(uint16_t unicode) {
    FILE *fp;
    BOOL success;
    uint16_t readin = 0;
    unsigned char pack[4];
    uint32_t  group;
    int32_t start, middle, end;
    short gb2312  = 0;

    fp = fopen(UTF_GB2312_TABLE, "r");
    if(fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        // 四字节一组，低两字节为unicode，高两字节为gb2312
        group = (length / sizeof(uint32_t));
        middle = group / 2;
        end = group;
        start = -1;
        do {
            // 二分法，从中间查起
            success = fread(pack, 1,sizeof(uint32_t), fp);
            if(!success || middle <= 0 || middle >= (group - 1)) {
                break;
            }
            readin = (uint16_t)(pack[0] | pack[1] << 8);
            if(unicode < readin) {
                // 向左查询
                end = middle;
                middle -= ((end - start) / 2);
            }else if(unicode > readin) {
                // 向右查询
                start = middle;
                middle += ((end - start) / 2);
            }
        }while(unicode != readin);

        //printf("%x %x %x %x\n", pack[0], pack[1], pack[2], pack[3]);
        gb2312 = pack[2] << 8 | pack[3];
        fclose(fp);
    } else {
        printf("%s fopen fail\n", UTF_GB2312_TABLE);
    }

    return gb2312;
}

int GetGB2312TFTidx(unsigned short gb2312_code) {
    int idx = 0;
    idx = ((gb2312_code & 0xFF) - 0xa1 + ((gb2312_code >> 8 &0xff) - (GB2312_START >> 8))*FONTS_NUM_BLOCK )*FONTS_BYTE_NUM;
    //if (idx < 0xb0a1)
    //    idx = 0;
    return idx;
}

void printfGB2312(int idx) {
    if (idx == 0) {
        printf("idx not find\n");
        return;
    }
    for (int i = 0; i < FONTS_BYTE_NUM;i++) {
        char c = bin_data[idx+i];
        for (int j = 0; j < 8; j++) {
            if (c & 0x80) {
                printf("*");
            } else {
                printf(" ");
            }
            c <<= 1;
        }
        if (i%2)
            printf("|\n");
    }
}

#define POINT_COLOR 0xFFFF
#define BACK_COLOR  0x0000
#define FONTS_H     16
#define FONTS_W     16
int fullGB2132buf(unsigned short* TFTbuf, int *table, short array_size, int flag) {
    if (TFTbuf == NULL || table == NULL) {
        LOGE("\n");
        return 0;
    }
    unsigned char c1, c2;
    int offset = 0;
    if (!flag) {
        TFTbuf[0] = 0xaa; //header+opt
        TFTbuf[1] = array_size*16; //width
        TFTbuf[2] = 16; //high
        TFTbuf[3] = 0;  //buf total size
        TFTbuf[4] = 0xFFFF; //point_color
        TFTbuf[5] = 0x0000; //back_color
        TFTbuf[6] = 0x0000; //start_x
        TFTbuf[7] = 0x0000; //start_y
        for (int i = 0; i < array_size; i++) {
            for (int j = 0; j < FONTS_H; j++) {
                c1 = bin_data[table[i] + j * 2];
                c2 = bin_data[table[i] + j * 2 + 1];
                for (int k = 0; k < FONTS_W; k++) {
                    offset = k + j * FONTS_W * array_size + FONTS_W * i + 8;
                    if (k < 8) {
                        if (c1 & 0x80) {
                            TFTbuf[offset] = POINT_COLOR;
                        } else {
                            TFTbuf[offset] = BACK_COLOR;
                        }
                        c1 <<= 1;
                    } else {
                        if (c2 & 0x80) {
                            TFTbuf[offset] = POINT_COLOR;
                        } else {
                            TFTbuf[offset] = BACK_COLOR;
                        }
                        c2 <<= 1;
                    
                    }

                } 
            }
        }
    } else {
        TFTbuf[0] = 0xab; //header+opt
        TFTbuf[1] = array_size*16; //width
        TFTbuf[2] = 16; //high
        TFTbuf[3] = 0;  //buf total size
        TFTbuf[4] = 0xFFFF; //point_color
        TFTbuf[5] = 0x0000; //back_color
        TFTbuf[6] = 0x0000; //start_x
        TFTbuf[7] = 0x0000; //start_y
        for (int i = 0; i < array_size; i++) {
            for (int j = 0; j < FONTS_H; j++) {
                c1 = bin_data[table[i] + j * 2];
                c2 = bin_data[table[i] + j * 2 + 1];
                offset = j * array_size + i + 8;

                TFTbuf[offset] = c1 << 8 | c2;
                printf("%x ", TFTbuf[offset]);
            }
            printf("\n");
        }
    
    }
    TFTbuf[3] = (offset - 8 + 1) * sizeof(short);
    LOGD("%d\n", offset);
    return offset+1;

}

void printfGB2132buf(unsigned short *TFTbuf,int len, int flag) {
    for (int i = 0; i < FONTS_H; i++) {
        unsigned short c = TFTbuf[i * len + 8];
        for (int j = 0; j < FONTS_W *len; j++) {
            if (!flag) {
                if (TFTbuf[j + i * FONTS_W * len + 8] == POINT_COLOR) {
                    printf("*");
                } else {
                    printf(" ");
                }
            } else {
                if (c & 0x8000) {
                    printf("*");
                } else {
                    printf(" ");
                }
                c <<= 1;
            }
            if ((j+1) % 16 == 0)
                c = TFTbuf[i * len + (j+1) / 16 + 8];
        }
        printf("\n");
    }
}

int utfToTFTbuf(unsigned char *msg, unsigned short *TFTbuf, bool iscompress) {
    if (msg == NULL || TFTbuf == NULL) {
        return 0;
    }
    char utf8[4] = {0};
    uint16_t utf16[2] = {0};
    uint32_t buffer;
    unsigned short gb_code;
    int table_idx[10] = {0};
    short cc_num= 0;

    for (short i = 0; i < strlen(msg);) {
        short len = 0;
        len = UTF8ToUnicode(msg + i, &buffer);
        LOGD("len:%d unicode:0x%x\n", len, buffer);
        if (len == 1) {
                break;
                
        }
        UnicodeToUTF16(buffer, utf16);
        LOGD("UnicodeToUTF16-utf16:0x%x\n", utf16[0]);
        gb_code = bsearch_gb2312(utf16[0]);
        LOGD("bsearch_gb2312-gb_code:0x%x\n", gb_code);
        table_idx[cc_num] = GetGB2312TFTidx(gb_code);
        LOGD("GetGB2312TFTidx[%d]:%d\n", cc_num, table_idx[cc_num]);
        cc_num++;
        i += len;
    }
#if 0
    for (int i = 0; i < tmp; i++) {
        //printf("%d ",table_idx[i]);
        printfGB2312(table_idx[i]);
    }
    printf("\n");
#endif 
    int ret = fullGB2132buf(TFTbuf, table_idx, cc_num, iscompress);

    printfGB2132buf(TFTbuf,cc_num, iscompress);
    return ret;
}
