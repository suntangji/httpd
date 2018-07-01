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
  /*const char *body = "<h1>200 OK</h1>";*/
  char desc[20] = "OK";
  if (status != 200) {
    strcpy(desc, body);
  }
  char first_line[30];
  sprintf(first_line, "HTTP/1.1 %d %s\r\n", status, desc);
  /*const char *first_line = "HTTP/1.0 200 OK\r\n";*/
  send(sockfd, first_line, strlen(first_line),0);
  const char *server = "Server: httpd\r\n";
  send(sockfd, server,strlen(server),0);

  const char *content_type = "Content-Type: text/html; charset=utf-8\r\n";
  send(sockfd,content_type,strlen(content_type),0);
  char content_length[30] = {0};
  sprintf(content_length, "Content-Length: %lu\r\n", strlen(body));
  send(sockfd,content_length,strlen(content_length),0);
  /*const char * connect = "Connection: keep-alive\r\n";*/
  /*send(sockfd,connect,strlen(connect),0);*/

  const char *blank_line="\r\n";
  send(sockfd, blank_line, strlen(blank_line), 0);
  send(sockfd, body, strlen(body), 0);

}

void Handler_200(int sockfd, Request *req) {
  HandlerCommon(sockfd, 200, "test httpd");
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
