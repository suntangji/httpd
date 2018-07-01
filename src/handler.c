#include "httpd.h"
#include <ctype.h>

static int GetLine(int fd, char *buf, int len) {
  char c = '\0';
  int i = 0;
  while ((c != '\n') && (i < len - 1)) {
    ssize_t read_size = recv(fd, &c, 1, 0);
    if (read_size > 0) {
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
  fprintf(stderr,"first line:%s\n", buf);
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
  } else if(strcasecmp(buf, "POST") == 0) {
    strcpy(req->method, "POST");
  } else {
    return 405;
    // 方法不允许
  }
  // 判断请求路径
  char * path = buf;
  path += strlen(buf) + 1;
  strcpy(req->path, path);

  // 判断 http 版本
  char * version = path;
  version += strlen(path) + 1;
  strcpy(req->version, version);

#ifdef DEBUG
  fprintf(stderr, "首行解析完毕：");
  fprintf(stderr, "%s %s %s\n", req->method, req->path, req->version);
  /*fprintf(stderr, "%s\n", path);*/
#endif

  return 200;
}
static int HandlerResponse(int sockfd, Request *req, int status) {
  const char * body = "<h1>200 OK</h1>";

  const char * first_line = "HTTP/1.0 200 OK\r\n";
  send(sockfd, first_line, strlen(first_line),0);
  const char * server = "Server: httpd\r\n";
  send(sockfd, server,strlen(server),0);

  const char * content_type = "Content-Type: text/html; charset=utf-8\r\n";
  send(sockfd,content_type,strlen(content_type),0);
  char content_length[30] = {0};
  sprintf(content_length, "Content-Length: %lu\r\n", strlen(body));
  send(sockfd,content_length,strlen(content_length),0);
  /*const char * connect = "Connection: keep-alive\r\n";*/
  /*send(sockfd,connect,strlen(connect),0);*/

  const char * blank_line="\r\n";
  send(sockfd, blank_line, strlen(blank_line), 0);
  send(sockfd, body, strlen(body), 0);
  return 1;
}
void * handler_request(void *arg) {
  int sockfd = *(int*)arg;
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
