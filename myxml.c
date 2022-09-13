#include "include/common.h"
#include "include/myxml.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
//refer http://blog.chinaunix.net/uid-25695950-id-3921541.html

#define XML_FILE "test.xml"

void fundInfoSet(fundInfo_s *fundInfo, char *name, char* value) {
    if (!strcmp(name,"name"))
        strcpy(fundInfo->f_name, value);

    if (!strcmp(name, "holders"))
        //strcpy(fundInfo->holders, value, strlen(value));
        fundInfo->holders = (float)atof(value);
    else if (!strcmp(name, "status"))
        strcpy(fundInfo->status, value);
    else if (!strcmp(name, "code")){
        if (strlen(value) != 6)
            return;
        strcpy(fundInfo->f_code, value);
        fundInfo->f_code[6]= '\0';
    }
    else if (!strcmp(name, "bid-price")) {
        fundInfo->bid_price = (float)atof(value);
    }
    return;
}

int xmlLoadInfo(fundInfo_s *a) {
    int num = xmlGetNodeNum();
    //a = (fundInfo_s*)calloc(num, sizeof(fundInfo_s));

    xmlDocPtr doc=NULL;
    xmlNodePtr cur=NULL;
    xmlNodePtr f_node = NULL;
    char* name=NULL;
    char* value=NULL;

    xmlKeepBlanksDefault (0);

    //create Dom tree
    doc=xmlParseFile(XML_FILE);
    if(doc==NULL)
    {
        printf("ERROR: Loading xml file failed.\n");
        exit(1);
    }

    // get root node
    cur=xmlDocGetRootElement(doc);
    if(cur==NULL)
    {
        printf("ERROR: empty file\n");
        xmlFreeDoc(doc);
        exit(2);
    }

    //walk the tree
    cur=cur->xmlChildrenNode; //get sub node
    num = 0;
    while(cur !=NULL)
    {
        f_node =  cur->xmlChildrenNode;
        while(f_node != NULL) {
            name=(char*)(f_node->name);
            value=(char*)xmlNodeGetContent(f_node);
            //LOGD("DEBUG: name is: %s, value is: %s\n", name, value);
            fundInfoSet(a+num, name, value);
        //    printf("DEBUG: name is: %s, value is: %s\n", name, value);
            xmlFree(value);
            f_node = f_node->next;

        }
        num++;
        cur = cur->next;
    }

    // release resource of xml parser in libxml2
    xmlFreeDoc(doc);
    //xmlCleanupParser();

    return num;
}

int xmlGetNodeNum() {
    int num = 0;
    xmlDocPtr doc=NULL;
    xmlNodePtr cur=NULL;
    xmlNodePtr f_node = NULL;
    char* name=NULL;
    char* value=NULL;

    xmlKeepBlanksDefault (0);

    //create Dom tree
    doc=xmlParseFile(XML_FILE);
    if(doc==NULL)
    {
        printf("ERROR: Loading xml file failed.\n");
        exit(1);
    }

    // get root node
    cur=xmlDocGetRootElement(doc);
    if(cur==NULL)
    {
        printf("ERROR: empty file\n");
        xmlFreeDoc(doc);
        exit(2);
    }

    //walk the tree
    cur=cur->xmlChildrenNode; //get sub node
    while(cur !=NULL)
    {
        num++;
        cur = cur->next;
    }

    // release resource of xml parser in libxml2
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return num;
}

int xml_test() {
    int num = 0;
    xmlDocPtr doc=NULL;
    xmlNodePtr cur=NULL;
    xmlNodePtr f_node = NULL;
    char* name=NULL;
    char* value=NULL;

    xmlKeepBlanksDefault (0);

    //create Dom tree
    doc=xmlParseFile(XML_FILE);
    if(doc==NULL)
    {
        printf("ERROR: Loading xml file failed.\n");
        exit(1);
    }

    // get root node
    cur=xmlDocGetRootElement(doc);
    if(cur==NULL)
    {
        printf("ERROR: empty file\n");
        xmlFreeDoc(doc);
        exit(2);
    }

    //walk the tree
    cur=cur->xmlChildrenNode; //get sub node
    while(cur !=NULL)
    {
        f_node =  cur->xmlChildrenNode;
        while(f_node != NULL) {
            name=(char*)(f_node->name);
            value=(char*)xmlNodeGetContent(f_node);
            printf("DEBUG: name is: %s, value is: %s\n", name, value);
            xmlFree(value);
            f_node = f_node->next;
        }
        num++;
        cur = cur->next;
    }

    // release resource of xml parser in libxml2
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return num;
}
