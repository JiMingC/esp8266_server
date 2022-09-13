#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include "include/linklist.h"
#include "include/typedef.h"
#include "include/utf_handle.h"
#include "include/threadpool.h"

#include "include/Msg_handler.h"
#include <curl/curl.h>
#include "include/myxml.h"
#include "include/common.h"
#include "include/fund_test.h"

#define LINE     10
#define DEBUG    1

#define TCP_PORT    7070

#define MEUN_HOME               0x0
#define MEUN_SENDMSG            0x1
#define MEUN_EspClientCTL       0x3
#define MEUN_Esp_TFTutfShow     0x31
#define MEUN_Esp_TFTutfShow_cp  0x32
#define MEUN_SENDMSG_CLIENT     0x11

void taskFund(void *arg) {
    listnode p = *(listnode*)arg;
    printf("thread %d is working, confd:%d\n", p.ID, p.confd);
    fundmain(p.ID, p.confd);

}

unsigned char func  = 0;
unsigned short level = 0;
unsigned short terminalinfopr(unsigned short level) {
    short ret = 0;
    switch(level) {
        case MEUN_HOME:
        printf("*[HOME] 1:SendMsg 2:CheckInfo 3:EspClient-Ctrl*\n");
        break;
        case MEUN_SENDMSG:
        printf("*[SendMsg] 1:client 2:broadcast 0:return*\n");
        break;
        case MEUN_EspClientCTL:
        printf("*[EspClient-Ctrl] 1:TFTutfShow 2:TFTutfShow(compress) 0:return*\n");
        break;
        case MEUN_Esp_TFTutfShow:
        case MEUN_Esp_TFTutfShow_cp:
        case MEUN_SENDMSG_CLIENT:
        func = level;
        printf("*[client] select one client:*\n");
        break;
        default:
        ret = 1;
        break;
    }
    return ret;
}
unsigned short terminalhandle(unsigned short level, char *msg){
    if (level == MEUN_SENDMSG_CLIENT || level == MEUN_Esp_TFTutfShow || level == MEUN_Esp_TFTutfShow_cp) {
        if (msg[0] == '0' || msg[1] > ' ') {
            level >>=  4;
            terminalinfopr(level);
        } else if (msg[0] > '9') {
            return level;
        } else {
            level = 0;
            for (int i = 0; msg[i] != '\n'; i++) {
                level = level * 10 + msg[i] - '0';
            }
            level |= 0x8000;
        }
    } else if (level == 0x7FFF) {
        //if (msg[0] == 'q' && msg[1] == '!') {
        //    level = 0x5;
        //    terminalinfopr(level);
        //}
        
    } else {
        if (msg[0] > '3')
            return level;
        else if (msg[0] == '0') {
            level >>=  4;
            terminalinfopr(level);
        } else {
            if (!terminalinfopr((level << 4) | (msg[0] - 48)))
                level = (level << 4) | (msg[0] - 48);
        }
    }
    LOGD("return :0x%x\n", level);
    return level;
}

int main()
{
    int tick = 0;

    ThreadPool *pool = threadPoolCreate(3, 10, 100);
	//创建socket套接字
	int serfd=0;
	serfd=socket(AF_INET,SOCK_STREAM,0);
	if(serfd<0)
	{
		perror("socket failed");
		return -1;
	}

	LOGD("socket ok!\n");

	//通过调用bind绑定IP地址和端口号
	int ret=0;
	struct sockaddr_in seraddr={0};
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(TCP_PORT);
//	seraddr.sin_addr.s_addr=inet_addr("175.178.82.209");
	//seraddr.sin_addr.s_addr=inet_addr("150.109.115.216");
	//seraddr.sin_addr.s_addr=inet_addr("172.0.0.1");
	seraddr.sin_addr.s_addr = htons(INADDR_ANY);

    int on = 1;
    if(setsockopt(serfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
        LOGE("setsockopt");

	ret=bind(serfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
	if(ret<0)
	{
		perror("bind failed");
		close(serfd);
		return -1;
	}

	LOGD("bind success\n");

    LOGD("bind ip:%s\n", inet_ntoa(seraddr.sin_addr));
	//通过调用listen将套接字设置为监听模式
	int lis=0;
	lis=listen(serfd,LINE);
	if(lis<0)
	{
		perror("listen failed");
		close(serfd);
		return -1;
	}

	LOGD("listen success\n");

    //creat list
    linklist head = init_list();
    
	//服务器等待客户端连接中，游客户端连接时调用accept产生一个新的套接字
	socklen_t addrlen;
	struct sockaddr_in clientaddr={0};
	addrlen=sizeof(clientaddr);

    //create fd_set
    fd_set rset;
    int maxfd = serfd;

    net_message_t *srcNetMsg;
    srcNetMsg = (net_message_t *)calloc(1,NET_MESSAGE_MAX_LENGTH);

    struct list_head *pos;
    linklist p, t;

    struct timeval timeout;
    terminalinfopr(level);
    while(1) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000*10;
        tick++;
        if (tick > 10000)
            tick = 0;
        //bzero 
        FD_ZERO(&rset);

        //add listen fd
        FD_SET(serfd,&rset);
        FD_SET(0, &rset);

        //queue list, add all fd into set
        list_for_each(pos,&head->list) {
            p = list_entry(pos, listnode, list);
            FD_SET(p->confd,&rset);

            maxfd = maxfd > p->confd ? maxfd : p->confd;
        }

        //select for wait fd respond
        select(maxfd+1, &rset, NULL, NULL, &timeout);

        //if new connect msg
        if (FD_ISSET(serfd, &rset)) {
            bzero(&clientaddr, addrlen);
        	int confd=accept(serfd,(struct sockaddr *)&clientaddr,&addrlen);

	        if(confd<0)
	        {
		        perror("accept failed");
		        close(serfd);
		        return -1;
	        }

            LOGD("new connect [%s:%hu]!\n", inet_ntoa(clientaddr.sin_addr),
                                          ntohs(clientaddr.sin_port));
            //creat id for new user
            int newID = rand() % 10000;
            LOGD("newID:%d\n", newID);
            netsendMsgUsrID(confd, newID);

            listnode *new_cli = new_client(newID, confd, clientaddr, tick);
            if(new_cli == NULL) {
                LOGD("new user create fail\n");
            } else {
                LOGI("[%s:%hu tick:%d]\n", inet_ntoa(new_cli->addr.sin_addr), ntohs(new_cli->addr.sin_port), new_cli->time);
                threadPoolAdd(pool, taskFund, (void*)new_cli);
                list_add_tail(&new_cli->list, &head->list);
            }
        }

        struct list_head *q;
        list_for_each_safe(pos,q,&head->list) {
            p=list_entry(pos, listnode, list);

            if (FD_ISSET(p->confd, &rset)) {
                char msg[100];
                int n;
                bzero(msg, 100);
                n = read(p->confd, msg, 100);

                if(n == 0) {
                    LOGD("[%s:%hu] is disconnect\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port));
                    list_del(pos);
                    free(p);
                    break;
                } else {
                    //LOGD("msg comming!!!\n");
                    p->time = tick;
                    memset(srcNetMsg,0,NET_MESSAGE_MAX_LENGTH);
                    netparseMsg(srcNetMsg, msg);
                    nethandlerMsg(p->confd, srcNetMsg);
                    if (srcNetMsg->enOpcode == 0x79) {
                        strcpy(p->usrname, srcNetMsg->body);
                        LOGI("%s join!!\n", p->usrname);
                    } else if (srcNetMsg->enOpcode == EN_MSG_HEARTBEAT){
                        LOGI("Heartbeat\n");
                    } else{
                        LOGI("msg:%s\n", srcNetMsg->body);
                    }
                }
            } else {
                if ((p->time < (tick - 1000)) || ((tick - p->time) > 9000)) {
                    LOGD("p->time:%d, tick:%d\n", p->time, tick);
                    LOGD("[%s:%hu] is timeout disconnect\n", inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port));
                    close(p->confd);
                    list_del(pos);
                    free(p);
                    
                }
            }
        }

        if (FD_ISSET(0, &rset)) {
            char msg[100];
            int n;
            bzero(msg,100);
            n = read(0, msg, 100);
            if (n == 0|| n == 1) {
                memset(msg, 0 , 100);
                //printf("NULL msg\n");
            } else {
                level = terminalhandle(level, msg);
                LOGD("terminalhandle return level:%d\n", level);
                int idx = 0;
                if (level == MEUN_SENDMSG_CLIENT || level == MEUN_Esp_TFTutfShow ||  level == MEUN_Esp_TFTutfShow_cp) {
                    printf("*              Online dev list:*\n");
                    list_for_each(pos, &head->list) {
                        idx++;
                        t = list_entry(pos, listnode,list);
                        printf("*              #[%d] ID:0x%4x (%d)*\n",idx, t->ID, t->ID);
                    }
                    printf("*              Dev total:%d, select the recvier(0 for return):*\n", idx);
                } else if (level >> 15) {
                    list_for_each(pos, &head->list) {
                        idx++;
                        if (idx == (level & 0x7FFF)) {
                            t = list_entry(pos, listnode,list);
                            if (func == MEUN_Esp_TFTutfShow)
                                printf("*Input your (utf)msg to ID:0x%4x(q! for exit):*\n",t->ID);
                            else if (func == MEUN_Esp_TFTutfShow_cp)
                                printf("*Input your (utf)msg to ID:0x%4x (compress)(q! for exit):*\n",t->ID);
                            else if (func == MEUN_SENDMSG_CLIENT)
                                printf("*Input your (ascii)msg to ID:0x%4x(q! for exit):*\n",t->ID);
                            level = 0x7FFF; 
                        } else {
                            printf("*invaild input, input again:*\n");
                            level = 0x5;
                        }

                    }
                    
                } else if (level == 0x7FFF) {
                    if (msg[0] == 'q' && msg[1] == '!') {
                        level = 0x1;
                        terminalinfopr(level);
                    } else if (msg[0] != '\n' || msg[0] != '\r'){
                        LOGD("func:0x%x\n", func);
                        if (func == MEUN_Esp_TFTutfShow) {
                            printf("*Input your (utf)msg to ID:0x%4x(q! for exit):*\n",t->ID);
                            unsigned short TFTbuf[MSGBUF_MAX];
                            int len = utfToTFTbuf(msg, TFTbuf, 0);
                            netsendTFTbuf(t->confd, TFTbuf, sizeof(short)*len);
                        } else if (func == MEUN_Esp_TFTutfShow_cp) {
                            printf("*Input your (utf)msg to ID:0x%4x(compress)(q! for exit):*\n",t->ID);
                            unsigned short TFTbuf[MSGBUF_MAX];
                            int len = utfToTFTbuf(msg, TFTbuf, 1);
                            for (int i = 0; i < len; i++) {
                                printf("%d 0x%x\n", i , TFTbuf[i]);
                            }
                            netsendTFTbuf(t->confd, TFTbuf, sizeof(short)*len);
                        } else if (func == MEUN_SENDMSG_CLIENT) {
                            printf("*Input your (ascii)msg to ID:0x%4x(q! for exit):*\n",t->ID);
                            netsendMsg(t->confd, msg, strlen(msg));
                        }
                        //printf("*Input your msg to ID:0x%4x(q! for exit):*\n", t->ID);
                    } else {
                        bzero(msg,100);
                    }
                }
            }
        }
    }

	close(serfd);
	
	return 0;
}
