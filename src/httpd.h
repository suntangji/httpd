/// \file httpd.h
/// \brief socket 相关头文件，一些参数定义
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <sys/stat.h>

#define DEBUG
#define LISTEN_SIZE 10

typedef struct {
  char method[10];
  char path[1024];
  char version[8];
} Request;

void *handler_request(void *arg);


#endif /* end of include guard: __HTTPD_H__ */
