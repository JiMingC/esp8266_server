#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
 
char data_buf[1024];
//输出到字符串再打印到屏幕上
ssize_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	strcat(stream, (char *)ptr);
	puts(stream);
    memcpy(data_buf, (char*)stream, size*nmemb);
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
 
int main(int argc, char *argv[])
{
    CURL *curl2;
	CURLcode res2;
	//FILE *fp2;
	//struct curl_slist *list=NULL;
	//list = curl_slist_append(list, argv[1]);//这个其实是-H但是这边没用到所以注释
	//list = curl_slist_append(list, argv[2]);//有几个-H头就append几次
	static char str[20480];
	res2 = curl_global_init(CURL_GLOBAL_ALL);
	curl2 = curl_easy_init();
	if(curl2) 
	{
		//fp2=fopen("UsefullInfo.json", "w+");
		curl_easy_setopt(curl2, CURLOPT_URL, "http://fundgz.1234567.com.cn/js/001186.js?rt=1463558676006");//这是请求的url
		//curl_easy_setopt(curl2, CURLOPT_POSTFIELDS, "username=root@pam&password=nicaiba_88");//这是post的内容
		//curl_easy_setopt(curl2, CURLOPT_HTTPHEADER, list);//若需要带头，则写进list
		curl_easy_setopt(curl2, CURLOPT_SSL_VERIFYPEER, 0);//-k
		curl_easy_setopt(curl2, CURLOPT_SSL_VERIFYHOST, 0);//-k
		//curl_easy_setopt(curl2, CURLOPT_VERBOSE, 1);//这是请求过程的调试log
		curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, write_data);//数据请求到以后的回调函数
		curl_easy_setopt(curl2, CURLOPT_WRITEDATA, str);//选择输出到字符串
		//curl_easy_setopt(curl2, CURLOPT_WRITEDATA, fp2);//选择输出到文件
		res2 = curl_easy_perform(curl2);//这里是执行请求
		curl_easy_cleanup(curl2);
		//fclose(fp2);
	}
	curl_global_cleanup();
    printf("%s\n", data_buf);
	return 0;
}
