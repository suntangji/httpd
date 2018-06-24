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
  int len = strlen(buf);
  for (int i = 0; i < len; i++) {
    if (isspace(buf[i])) {
      buf[i] = '\0';
    }
  }
  if (strcasecmp(buf, "GET") == 0) {
    strcpy(req->method, "GET");
  } else if(strcasecmp(buf, "POST") == 0) {
    strcpy(req->method, "POST");
  } else {
    return 405;
    // 方法不允许
  }


}
void * handler_request(void *arg) {
  int sockfd = *(int*)arg;
  /*printf("%d\n", sockfd);*/
  Request req;
  ParserRequest(sockfd, &req);

  return NULL;
}
