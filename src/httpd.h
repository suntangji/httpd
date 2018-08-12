/// \file httpd.h
/// \brief socket 相关头文件，一些参数定义
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define DEBUG
#define LISTEN_SIZE 10

typedef struct {
  char method[10];
  char path[1024];
  char query_string[1024];
  char version[8];
  int content_lens;
} Request;

typedef struct {
  int sockfd;
  int epollfd;
} res_param_t;

void *handler_request(void *arg);
void PrintLog(const char *log);
void GetTime(char *buf);
void SetNonBlock(int fd);
void ProcessConnect(int listen_fd, int epoll_fd);
ssize_t NonBlockRead(int fd, char *buf, int size);
void ProcessRequest(int connect_fd, int epoll_fd);


#endif /* end of include guard: __HTTPD_H__ */
