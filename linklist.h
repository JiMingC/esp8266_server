#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "kernel_list.h"
//client

#define NAME_LEN_MAX 32

typedef struct client {
    int ID;
    int confd;
    char usrname[NAME_LEN_MAX];
    struct sockaddr_in addr;

    struct list_head list;

}listnode, *linklist;

linklist init_list(void);
linklist new_client(int ID, int fd, struct sockaddr_in addr);
