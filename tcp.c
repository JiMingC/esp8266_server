#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "linklist.h"
#include "typedef.h"

#include "Msg_handler.h"
#define LINE     10
#define DEBUG    1

int main()
{
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
	seraddr.sin_port=htons(8888);
	//seraddr.sin_addr.s_addr=inet_addr("192.168.59.131");
	//seraddr.sin_addr.s_addr=inet_addr("150.109.115.216");
	//seraddr.sin_addr.s_addr=inet_addr("172.0.0.1");
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

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

    while(1) {
        //bzero 
        FD_ZERO(&rset);

        //add listen fd
        FD_SET(serfd,&rset);

        //queue list, add all fd into set
        struct list_head *pos;
        linklist p;
        list_for_each(pos,&head->list) {
            p = list_entry(pos, listnode, list);
            FD_SET(p->confd,&rset);

            maxfd = maxfd > p->confd ? maxfd : p->confd;
        }

        //select for wait fd respond
        select(maxfd+1, &rset, NULL, NULL, NULL);

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

            listnode *new_cli = new_client(newID, confd, clientaddr);
            if(new_cli == NULL) {
                LOGD("new user create fail\n");
            } else {
                LOGI("[%s:%hu]\n", inet_ntoa(new_cli->addr.sin_addr), ntohs(new_cli->addr.sin_port));
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
                    LOGD("msg comming!!!\n");
                    memset(srcNetMsg,0,NET_MESSAGE_MAX_LENGTH);
                    netparseMsg(srcNetMsg, msg);
                    //nethandlerMsg(srcNetMsg);
                    if (srcNetMsg->enOpcode == 0x79) {
                        strcpy(p->usrname, srcNetMsg->body);
                        LOGI("%s join!!\n", p->usrname);
                    }else{
                        LOGI("msg:%s\n", srcNetMsg->body);
                    }
                }
            }
        }
    }

    /*
	//调用recv接收客户端的消息
	while(1)
	{
		int rev=0;
		int sed=0;
		char buf[1024]={0};
		rev=recv(confd,buf,sizeof(buf),0);
		if(rev>0)
		{
			printf("本次收到了%d个字节\n",rev);
			printf("receive: %s\n",buf);
		}
	
		memset(buf,0,sizeof(buf));
		gets(buf);
		sed=send(confd,buf,strlen(buf),0);
		if(sed<0)
		{
			perror("send failed");
			close(serfd);
			return -1;
		}
		printf("send success\n");
	}
    */
	close(serfd);
	
	return 0;
}
