/*
 * =====================================================================================
 *
 *       Filename:  iconv.c
 *
 *    Description:  j
 *
 *        Version:  1.0
 *        Created:  08/05/2015 05:51:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iconv.h>


bool gbk_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{
    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
     *           IGNORE ：遇到无法转换字符跳过*/
    char *encTo = "UTF-8//IGNORE";
    /* 源编码 */
    char *encFrom = "GBK";

    /* 获得转换句柄
     *@param encTo 目标编码方式
     *@param encFrom 源编码方式
     *
     * */
    iconv_t cd = iconv_open (encTo, encFrom);
    if (cd == (iconv_t)-1)
    {
        perror ("iconv_open");
    }

    /* 需要转换的字符串 */
    //printf("inbuf=%s\t", inbuf);

    /* 打印需要转换的字符串的长度 */
    //printf("inlen=%d\n", (int)*inlen);


    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    char *tmpin = inbuf;
    char *tmpout = outbuf;
    size_t insize = *inlen;
    size_t outsize = *outlen;

    /* 进行转换
    *@param cd iconv_open()产生的句柄
    *@param srcstart 需要转换的字符串
    *@param inlen 存放还有多少字符没有转换
    *@param tempoutbuf 存放转换后的字符串
    *@param outlen 存放转换后,tempoutbuf剩余的空间
    *
    * */
    size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
    if (ret == -1)
    {
        perror ("iconv");
    }

    /* 存放转换后的字符串 */
    //printf("outbuf=%s\t", outbuf);

    //存放转换后outbuf剩余的空间
    //printf("outlen=%d\n", (int)*outlen);

    int i = 0;

    for (i=0; i<(outsize- (*outlen)); i++)
    {
     //printf("%2c", outbuf[i]);
     //printf("%x\n", outbuf[i]);
    }

    /* 关闭句柄 */
    iconv_close (cd);

    return 0;
}

bool unicode_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{
    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
     *           IGNORE ：遇到无法转换字符跳过*/
    char *encTo = "UTF-8//IGNORE";
    /* 源编码 */
    char *encFrom = "UNICODE";

    /* 获得转换句柄
     *@param encTo 目标编码方式
     *@param encFrom 源编码方式
     *
     * */
    iconv_t cd = iconv_open (encTo, encFrom);
    if (cd == (iconv_t)-1)
    {
        perror ("iconv_open");
    }

    /* 需要转换的字符串 */
    printf("inbuf=%s\n", inbuf);

    /* 打印需要转换的字符串的长度 */
    printf("inlen=%d\n", (int)*inlen);


    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    char *tmpin = inbuf;
    char *tmpout = outbuf;
    size_t insize = *inlen;
    size_t outsize = *outlen;

    /* 进行转换
    *@param cd iconv_open()产生的句柄
    *@param srcstart 需要转换的字符串
    *@param inlen 存放还有多少字符没有转换
    *@param tempoutbuf 存放转换后的字符串
    *@param outlen 存放转换后,tempoutbuf剩余的空间
    *
    * */
    size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
    if (ret == -1)
    {
        perror ("iconv");
    }

    /* 存放转换后的字符串 */
    printf("outbuf=%s\n", outbuf);

    //存放转换后outbuf剩余的空间
    printf("outlen=%d\n", (int)*outlen);

    int i = 0;

    for (i=0; i<(outsize- (*outlen)); i++)
    {
     //printf("%2c", outbuf[i]);
     //printf("%x\n", outbuf[i]);
    }

    /* 关闭句柄 */
    iconv_close (cd);

    return 0;
}

bool utf8_to_unicode (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{

    /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
    *           IGNORE ：遇到无法转换字符跳过*/
    char *encTo = "UNICODE//IGNORE";
    /* 源编码 */
    char *encFrom = "UTF-8";

    /* 获得转换句柄
    *@param encTo 目标编码方式
    *@param encFrom 源编码方式
    *
    * */
    iconv_t cd = iconv_open (encTo, encFrom);
    if (cd == (iconv_t)-1)
    {
        perror ("iconv_open");
    }

    /* 需要转换的字符串 */
    printf("inbuf=%s\n", inbuf);

    /* 打印需要转换的字符串的长度 */
    printf("inlen=%d\n", (int)*inlen);

    /* 由于iconv()函数会修改指针，所以要保存源指针 */
    char *tmpin = inbuf;
    char *tmpout = outbuf;
    size_t insize = *inlen;
    size_t outsize = *outlen;

    /* 进行转换
    *@param cd iconv_open()产生的句柄
    *@param srcstart 需要转换的字符串
    *@param inlen 存放还有多少字符没有转换
    *@param tempoutbuf 存放转换后的字符串
    *@param outlen 存放转换后,tempoutbuf剩余的空间
    *
    * */
    size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
    if (ret == -1)
    {
        perror ("iconv");
    }

    /* 存放转换后的字符串 */
    printf("outbuf=%s\n", outbuf);

    //存放转换后outbuf剩余的空间
    printf("outlen=%d\n", (int)*outlen);

    int i = 0;

    for (i=0; i<(outsize- (*outlen)); i++)
    {
        //printf("%2c", outbuf[i]);
        //printf("%x\n", outbuf[i]);
    }

    /* 关闭句柄 */
    iconv_close (cd);

    return 0;
}

int gbk2utf8(char* in, char* out) {
    if (in == NULL)
        return 1;
    char outbuf[1024] = {'\0'};
    size_t outlen = 1024;
    size_t inlen = strlen(in);
    gbk_to_utf8(in, &inlen, outbuf, &outlen);
    //unicode_to_utf8(in, &inlen, outbuf, &outlen);
    memcpy(out, outbuf, 1024 - outlen);

    return 1024 - outlen;
}
/*
int main ()
{
    // 需要转换的字符串
    //char inbuf[1024] = "abcdef哈哈哈哈行"; 
    char *text = "汉";    

    char inbuf[1024] = {};
    strcpy (inbuf, text);
    size_t inlen = strlen (inbuf);

    // 存放转换后的字符串
    char outbuf[1024] = {};
    size_t outlen = 1024;

    utf8_to_unicode (inbuf, &inlen, outbuf, &outlen);
    printf ("print outbuf: %s\n", outbuf);

    size_t outsize = strlen(outbuf);
    size_t insize = 1024;
    char instr[1024] = {};
    unicode_to_utf8 (outbuf, &outsize, instr, &insize);
    printf ("print buf: %s\n", instr);
    return 0;
}
*/
