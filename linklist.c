#include "linklist.h"

//client
/*
typedef struct client {
    int ID;
    int connfd;
    struct sockaddr_in addr;
    
    struct list_head list;

}listnode, *linklist;
*/

linklist init_list(void) {
    linklist head = calloc(1, sizeof(listnode));
    if (head != NULL) {
        INIT_LIST_HEAD(&head->list);
    }
    return head;
}

linklist new_client(int ID, int fd, struct sockaddr_in addr) {
    linklist new_cli = calloc(1, sizeof(listnode));

    if (new_cli == NULL) {
        perror("calloc failed");
    }
    else {
        new_cli->ID = ID;
        new_cli->confd = fd;
        new_cli->addr = addr;

        INIT_LIST_HEAD(&new_cli->list);
    }
}

