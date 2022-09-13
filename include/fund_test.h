#ifndef __FUNCTEST_H_
#define __FUNCTEST_H_
#include "common.h"
void fundInitFromXml(fundInfo_s *a, CURL *curl, int num);
void fundGetInfoFromXml(fundInfo_s *a, CURL *curl, int num);
int fundmain(int id, int fd);
#endif
