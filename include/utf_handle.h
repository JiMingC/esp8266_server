#ifndef _UTF_HANDLE_H_
#define _UTF_HANDLE_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
uint8_t UTF8ToUnicode(uint8_t *utf8, uint32_t *unicode);
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t *utf16);
unsigned short bsearch_gb2312(uint16_t unicode);
int GetGB2312TFTidx(unsigned short gb2312_code);
void printfGB2312(int idx);
int utfToTFTbuf(unsigned char *msg, unsigned short *TFTbuf, bool iscompress);
#endif
