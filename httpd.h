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
  char uri[1024];
  char version[4];
} Request;

void * handler_request(void *arg);


#endif /* end of include guard: __HTTPD_H__ */
