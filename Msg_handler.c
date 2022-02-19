#include "Msg_handler.h"
#include <stdio.h>
#include <string.h>

void pr() {
    printf("send coo printf %d\n", __LINE__);
    fflush(stdout);
}
//int cecsendMsg(net_message_t NetMsg)
//{
//    //LOGD("%s:\n", __FUNCTION__);
//    int ret = -1;
//    ret = sendMessage(&NetMsg);
//    if(ret < 0){
//        printf("%s: cecsendTxMsg fail! ret = %d \n", __FUNCTION__,ret);
//        return -1;
//    }
//    return ret;
//}

static int USRID = 0;
static net_info_t net_info;

int netconfig_usrname(char* name, int len) {
    int i = 0;
    for(i = 0; i < len; i++) {
        net_info.usrNAME[i] = name[i];
    }
    return 0;
}

void netconfig_init(){
    net_info.usrID = USRID;
    netconfig_usrname("LOCAL_PC",9);
}

int netparseMsg(net_message_t *pstNetMsg, unsigned char* buf) {
    int i = 0;
    memset(pstNetMsg, 0, NET_MESSAGE_MAX_LENGTH);
    if (buf == NULL)
        return -1;
    pstNetMsg->initiator = buf[0];
    pstNetMsg->destination = buf[1];
    pstNetMsg->enOpcode = (Net_MSG_Opcode_LIST)buf[2];
    pstNetMsg->netId = (buf[3] << 24) + (buf[4] << 16) + (buf[5] << 8) + buf[6];
    pstNetMsg->body_len = buf[7];
    for(i = 0 ;i< pstNetMsg->body_len;i++)
    {
       pstNetMsg->body[i] = buf[i + 8];
    }
    LOGD("0:%x 1:%x 2:%x 3:%x 4:%x 5:%x 6:%x 7:%x\n",
            buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
    pr_netMsg(pstNetMsg);
    return 0;
}

int netgetMsgFromBuff(net_message_t *pstNetMsg, unsigned char *buf) {
    int i = 0;
    memset(pstNetMsg, 0, NET_MESSAGE_MAX_LENGTH);
    if (buf == NULL)
        return -1;
    pstNetMsg->initiator = buf[0];
    pstNetMsg->destination = buf[1];
    pstNetMsg->enOpcode = (Net_MSG_Opcode_LIST)buf[2];
    pstNetMsg->netId = (buf[3] << 24) + (buf[4] << 16) + (buf[5] << 8) + buf[6];
    pstNetMsg->body_len = buf[7];
    for(i = 0 ;i< pstNetMsg->body_len;i++)
    {
       pstNetMsg->body[i] = (char)buf[i + 8];
    }
    pstNetMsg->body[i + 1] = '\0';
    return pstNetMsg->body_len + 9;
}

void pr_netMsg(net_message_t *srcNetMsg) {
    if (srcNetMsg == NULL)
        return;
    LOGD("initiator[0x%x] destination[0x%x] enOpcode[0x%x] netID[%d] len[%d]\n",
                        srcNetMsg->initiator,
                        srcNetMsg->destination,
                        srcNetMsg->enOpcode,
                        srcNetMsg->netId,
                        srcNetMsg->body_len);
    switch(srcNetMsg->enOpcode) {
        case EN_MSG_GIVE_USRNAME :
            LOGD("GIVE_USRNAME: %s\n", (char*)srcNetMsg->body);
            break;
    }
    fflush(stdout);
}

int nethandlerReport_String(char* buf, char* string, int str_len) {
    int i = 0;
    buf[7] = str_len;
    for(i = 0 ;i< str_len;i++) {
        buf[i + NET_MESSAGE_HEAD_LEN] = string[i];
    }
}

int nethandlerReport_Int32(char* buf, int num) {
    buf[7] = sizeof(int);
    buf[8] = num >> 24; 
    buf[9] = num >> 16; 
    buf[10] = num >> 8; 
    buf[11] = num; 
}

int nethandlerReport(int confd, net_message_t *srcNetMsg) {
    char buf[128] = {0};
    buf[0] = srcNetMsg->destination;
    buf[1] = srcNetMsg->initiator;
    buf[2] = srcNetMsg->enOpcode + 1;
    buf[3] = (net_info.usrID >> 24);
    buf[4] = (net_info.usrID >> 16);
    buf[5] = (net_info.usrID >> 8) ;
    buf[6] = USRID;
    buf[7] = 0;
    switch((Net_MSG_Opcode_LIST)srcNetMsg->enOpcode) {
        case EN_MSG_GET_USRNAME:
            nethandlerReport_String(buf, net_info.usrNAME, strlen(net_info.usrNAME));
        default:
            break;
    }
    int ret = write(confd, buf, NET_MESSAGE_HEAD_LEN + srcNetMsg->body_len);
    return ret;
}

int nethandlerReceive() {
    return 0;
}

int nethandlerMsg(int confd, net_message_t *srcNetMsg) {
    if (srcNetMsg->enOpcode == EN_MSG_NONE) {
        nethandlerReceive();
    } else {
        nethandlerReport(confd, srcNetMsg);
    }
}

Net_MSG_Opcode_LIST netgetMsgOpcodeType(net_message_t *srcNetMsg) {
    if (srcNetMsg == NULL)
        return EN_MSG_ERROR;
    return (Net_MSG_Opcode_LIST)srcNetMsg->enOpcode;

}

int netsendMsgUsrID(int fd, int ID) {
/*
    net_message_t *dstNetMsg;
    dstNetMsg = (net_message_t *)calloc(1,NET_MESSAGE_MAX_LENGTH);
    
    dstNetMsg->initiator = 0xF;
    dstNetMsg->destination = 0;
    dstNetMsg->enOpcode = EN_MSG_GIVE_USRID;
    dstNetMsg->netId = (1>> 24) + (2 >> 16) + (3 >> 8) + 4;
    dstNetMsg->body_len = 4;
    dstNetMsg->body[0] = ID >> 24;
    dstNetMsg->body[1] = ID >> 16;
    dstNetMsg->body[2] = ID >> 8;
    dstNetMsg->body[3] = ID;
*/
    unsigned char buf[12] = {0};
    buf[0] = 0x1;
    buf[1] = 0x2;
    buf[2] = EN_MSG_GIVE_USRID;
    buf[3] = 1;
    buf[4] = 2;
    buf[5] = 3;
    buf[6] = 4;
    buf[7] = 4;
    buf[8] = ID >> 24 & 0xff;
    buf[9] = ID >> 16 & 0xff;
    buf[10] = ID >> 8 & 0xff;
    buf[11] = ID >> 0 & 0xff;

    printf("NewID:0x%x\n", ID);
    printf("send ID: 0x%x 0x%x 0x%x 0x%x\n", buf[8], buf[9],buf[10], buf[11]);

    return write(fd, (char*)buf, 12);
}

int netsetID(int ID) {
    USRID = ID;
    return 0;
}
