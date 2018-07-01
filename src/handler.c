/// \file handler.c
/// \brief 解析请求
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#include <ctype.h>
#include "httpd.h"
#include "response.h"

/// \brief GetLine 获取 http 请求的一行
/// 不同浏览器请求换行标志不同
/// 如果是 \r\n 就替换为 \n
/// 如果是 \r 也替换为 \n
/// \param fd
/// \param buf
/// \param len
///
/// \return 返回读取的字符数量
static int GetLine(int fd, char *buf, int len) {
  char c = '\0';
  int i = 0;
  while ((c != '\n') && (i < len - 1)) {
    ssize_t read_size = recv(fd, &c, 1, 0);
    if (read_size > 0) {
      if (c == '\r') {
        recv(fd, &c, 1, MSG_PEEK);
        if (c == '\n') {
          recv(fd, &c, 1, 0);
        } else {
          c = '\n';
        }
      }
      buf[i++] = c;
    }
  }
  buf[i] = '\0';
  return i;
}
static int ParserRequest(int sockfd, Request *req) {
  char buf[1024] = { 0 };
  int read_size = GetLine(sockfd, buf, sizeof(buf));
  if (read_size <= 0) {
    return 400;
    // bad request
  }
#ifdef DEBUG
#endif
  int len = strlen(buf);
  // 把空格置为 \0
  for (int i = 0; i < len; i++) {
    if (isspace(buf[i])) {
      buf[i] = '\0';
    }
  }
  // 判断方法
  if (strcasecmp(buf, "GET") == 0) {
    strcpy(req->method, "GET");
  } else if (strcasecmp(buf, "POST") == 0) {
    strcpy(req->method, "POST");
  } else {
    return 405;
    // 方法不允许
  }
  // 判断请求路径
  char *path = buf;
  path += strlen(buf) + 1;
  strcpy(req->path, path);

  // 判断 http 版本
  char *version = path;
  version += strlen(path) + 1;
  strcpy(req->version, version);

#ifdef DEBUG
  fprintf(stderr, "%s %s %s\n", req->method, req->path, req->version);
  /*fprintf(stderr, "%s\n", path);*/
#endif
  while (read_size) {
    read_size = GetLine(sockfd, buf, sizeof(buf));
    printf("%s", buf);
    if (strcmp(buf, "\n") == 0) {
      break;
    }
  }

  return 200;
}
static void HandlerResponse(int sockfd, Request *req, int status) {
  switch (status) {
  case 200:
    Handler_200(sockfd, req);
    break;
  case 400:
    Handler_400(sockfd);
  case 405:
    Handler_405(sockfd);
  default:
    Handler_500(sockfd);

  }
}
void *handler_request(void *arg) {
  int sockfd = *(int *)arg;
  /*printf("%d\n", sockfd);*/
  Request req;
  int status = ParserRequest(sockfd, &req);
  HandlerResponse(sockfd, &req, status);
  close(sockfd);
#ifdef DEBUG
  fprintf(stderr, "HandlerResponse OK!\n");
#endif

  return NULL;
}
