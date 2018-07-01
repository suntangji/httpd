/// \file response.c
/// \brief 处理响应
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#include "response.h"


/// \brief HandlerCommon 此函数用于处理响应的公共部分
///
/// \param sockfd
/// \param status 此参数表示响应状态
/// \param body 如果是200，body为响应，其他为响应状态描述
static void HandlerCommon(int sockfd, int status, const char *body) {
  char desc[20] = "OK";
  char html[1024 * 4];
  sprintf(html, "<html style=\"margin:0;padding:0 \"><head><title>%d %s</title></head>\
      <body><center><h1>%d %s</h1></center><hr>\
      <center>httpd</center></body></html>",
          status, body, status, body);

  if (status != 200) {
    strcpy(desc, body);
  }
  char first_line[60];
  sprintf(first_line, "HTTP/1.1 %d %s\r\n", status, desc);
  /*const char *first_line = "HTTP/1.0 200 OK\r\n";*/
  send(sockfd, first_line, strlen(first_line),0);
  const char *server = "Server: httpd\r\n";
  send(sockfd, server,strlen(server),0);

  const char *content_type = "Content-Type: text/html; charset=utf-8\r\n";
  send(sockfd,content_type,strlen(content_type),0);
  char content_length[30] = {0};
  sprintf(content_length, "Content-Length: %lu\r\n", strlen(html));
  send(sockfd,content_length,strlen(content_length),0);
  /*const char * connect = "Connection: keep-alive\r\n";*/
  /*send(sockfd,connect,strlen(connect),0);*/

  const char *blank_line="\r\n";
  send(sockfd, blank_line, strlen(blank_line), 0);
  if (strcasecmp(body, "HEAD") != 0) {
    send(sockfd,html, strlen(html), 0);
  }

}

void Handler_200(int sockfd, Request *req) {
  if (strcasecmp(req->method, "HEAD") == 0) {
    HandlerCommon(sockfd, 200, "HEAD");
  } else if ((strcasecmp(req->method, "GET") == 0) && strcasecmp(req->query_string, "") == 0) {
    /*HandlerStatic(sockfd, req->path);*/
    /*printf("query_string:%s\n", req->query_string);*/
    HandlerCommon(sockfd, 200, "OK");
  } else {
    HandlerCommon(sockfd, 200, "OK");
  }
}
void Handler_400(int sockfd) {
  HandlerCommon(sockfd, 400, "Bad Request");
}
void Handler_405(int sockfd) {
  HandlerCommon(sockfd, 405, "Method Not Allowed");
}
void Handler_500(int sockfd) {
  HandlerCommon(sockfd, 500, "Internal Server Error");
}
void Handler_501(int sockfd) {
  HandlerCommon(sockfd, 501, "Not Implemented");
}
