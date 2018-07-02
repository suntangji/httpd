/// \file handler.c
/// \brief 解析请求
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
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
  } else if (strcasecmp(buf, "HEAD") == 0) {
    strcpy(req->method, "HEAD");
  } else {
    /// log
    fprintf(stderr, "%s ", buf);
    /// end log
    /// \brief while 读取所有请求
    /// 要不然无法完成响应
    /// \param read_size
    while (read_size) {
      read_size = GetLine(sockfd, buf, sizeof(buf));
      /*printf("%s", buf);*/
      if (strcmp(buf, "\n") == 0) {
        break;
      }
    }
    return 501;
    // 方法暂不支持
  }
  // 判断请求路径
  char *path = buf;
  path += strlen(buf) + 1;
  // 判断 http 版本
  char *version = path;
  version += strlen(path) + 1;
  strcpy(req->version, version);
  // 判断 query_string
  char *query_string = path;
  while (*query_string != '\0') {
    if (*query_string == '?') {
      *query_string = '\0';
      query_string++;
      strcpy(req->query_string, query_string);
      break;
    } else {
      query_string++;
    }
  }
  strcpy(req->path, path);


  /// log
  fprintf(stderr, "%s %s %s ", req->method, req->path, req->version);
  /// end log
  while (read_size) {
    read_size = GetLine(sockfd, buf, sizeof(buf));
    /*printf("%s", buf);*/
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
  //case 400:
  //Handler_400(sockfd);
  //break;
  //case 405:
  //Handler_405(sockfd);
  //break;
  case 501:
    Handler_501(sockfd);
    break;
  default:
    Handler_500(sockfd);

  }
}
void *handler_request(void *arg) {
  res_param_t *res_param = (res_param_t *)arg;
  int sockfd = res_param->sockfd;
  /*printf("%d\n", sockfd);*/
  /// log
  char time[30];
  GetTime(time);
  PrintLog(time);
  char client_ip[20];
  strcpy(client_ip, res_param->client_ip);
  PrintLog(client_ip);
  /// end log
  Request req;
  int status = ParserRequest(sockfd, &req);
  /*status = 405;*/
  /*printf("%d\n", status);*/
  HandlerResponse(sockfd, &req, status);

  close(sockfd);
  free(res_param);
#ifdef DEBUG
  /*fprintf(stderr, "HandlerResponse OK!\n");*/
#endif

  return NULL;
}

void GetTime(char *buf) {
  time_t now;
  struct tm *timenow;
  time(&now);
  timenow = localtime(&now);
  /*printf("%s\n",asctime(timenow));*/
  sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d ", timenow->tm_year + 1900,
          timenow->tm_mon + 1, timenow->tm_mday,
          timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
}
// 非线程安全
void PrintLog(const char *log) {
  fprintf(stderr, "%s ", log);
}
