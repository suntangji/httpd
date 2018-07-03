/// \file response.h
/// \brief 处理响应相关函数声明
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "httpd.h"
void Handler_200(int sockfd, Request *req);
void Handler_400(int sockfd);
void Handler_403(int sockfd);
void Handler_404(int sockfd);
void Handler_405(int sockfd);
void Handler_500(int sockfd);
void Handler_501(int sockfd);
void HandlerStatic(int sockfd, char *path);
void HandlerDynamic(int sockfd, Request *req);

#endif /* end of include guard: __RESPONSE_H__ */
