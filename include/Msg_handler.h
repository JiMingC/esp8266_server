#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include <stdlib.h>
#include "typedef.h"
#include <unistd.h>
#define NET_MESSAGE_BODY_MAX_LENGTH 128
#define NET_MESSAGE_MAX_LENGTH      135
#define NET_MESSAGE_HEAD_LEN        8

#define NET_MESSAGE_INFO_LEN        32

#define MSGBUF_MAX  5120
typedef enum Net_Logical_Address {
    NET_ADDR_SERVER = 0,
    NET_ADDR_PC = 1,
    NET_ADDR_DEV = 2,
} Net_Logical_Address_e;

typedef enum
{
    EN_MSG_ERROR                  = 0x0,
    EN_MSG_HEARTBEAT              = 0x1,
    EN_MSG_SINGLE_SEND            = 0x2,
    EN_MSG_SINGLE_SEND_REPLY      = 0x3,
    EN_MSG_GIVE_USRID             = 0x10,
    EN_MSG_ESP_TFTSHOW            = 0x12,
    EN_MSG_GET_USRNAME            = 0x78,
    EN_MSG_GIVE_USRNAME           = 0x79,
    EN_MSG_ESP_POSTFUNDJSON       = 0x90,
    EN_MSG_NONE                   = 0xff,
}Net_MSG_Opcode_LIST;

typedef struct net_message {
    /* logical address of sender */
    Net_Logical_Address_e initiator;
    /* logical address of receiver */
    Net_Logical_Address_e destination;

    /* net message opcode */
    unsigned char enOpcode;
    /* net sender ID*/
    unsigned short netId;
    /* Length in bytes of body, range [0, CEC_MESSAGE_BODY_MAX_LENGTH-3] */
    int body_len;
    unsigned char body[NET_MESSAGE_BODY_MAX_LENGTH-2];
} net_message_t;

typedef struct net_info {
    int usrID;
    char usrNAME[NET_MESSAGE_INFO_LEN];
} net_info_t;

int netparseMsg(net_message_t *pstNetMsg, unsigned char* buf);
int nethandlerReport_String(char* buf, char* string, int str_len);
int nethandlerReport_Int32(char* buf, int num);
int netgetMsgFromBuff(net_message_t *pstNetMsg, unsigned char *buf);
Net_MSG_Opcode_LIST netgetMsgOpcodeType(net_message_t *srcNetMsg);
int netsendMsgUsrID(int fd, int ID);
void pr_netMsg(net_message_t *srcNetMsg);
void pr();
int netsendMsg(int fd, char* buf, int len);
int nethandlerMsg(int confd, net_message_t *srcNetMsg);
int netsendTFTbuf(int fd, unsigned short *TFTbuf, int size);
int netsendFundJson(int fd, char* js_body, int size);
#endif // MSG_HANDLER_H
