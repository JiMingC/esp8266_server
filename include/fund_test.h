#ifndef __FUNCTEST_H_
#define __FUNCTEST_H_
#include "common.h"
void fundInitFromXml(fundInfo_s *a, CURL *curl, int num);
void fundGetInfoFromXml(fundInfo_s *a, CURL *curl, int num);
int fundfix(fundInfo_s *fundInfo, int id, int fd, int f_num);
int fundroll(fundInfo_s *fundInfo, int id, int fd, int f_num);
int fundInfoInit(fundInfo_s *fundInfo);
#endif
