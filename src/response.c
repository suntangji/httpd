/// \file response.c
/// \brief 处理响应
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#include "response.h"
#include "read_config.h"


/// \brief HandlerCommon 此函数用于处理响应的公共部分
///
/// \param sockfd
/// \param status 此参数表示响应状态码
/// \param body 为空不发送 body, 不为空使用它作为 body
/// \param length 如果为 0 表示不知 length 为多少，在函数内部确定 length
/// 如果不为 0 就使用参数的 length
static void HandlerCommon(int sockfd, int status, const char *body, size_t length) {
  char desc[20] = "OK";
  char html[1024 * 4];
  if (length == 0) {
    sprintf(html, "<html style=\"margin:0;padding:0 \"><head><title>%d %s</title></head>\
      <body><center><h1>%d %s</h1></center><hr>\
      <center>httpd</center></body></html>",
            status, body, status, body);
    length = strlen(html);
  }

  if (status != 200) {
    strcpy(desc, body);
  }
  char first_line[60];
  sprintf(first_line, "HTTP/1.1 %d %s\r\n", status, desc);
  /*const char *first_line = "HTTP/1.0 200 OK\r\n";*/
  send(sockfd, first_line, strlen(first_line),0);
  /// log
  fprintf(stderr, "%d %s\n", status, desc);
  /// end log
  const char *server = "Server: httpd\r\n";
  send(sockfd, server,strlen(server),0);

  const char *content_type = "Content-Type: text/html; charset=utf-8\r\n";
  send(sockfd,content_type,strlen(content_type),0);
  char content_length[30] = {0};
  sprintf(content_length, "Content-Length: %lu\r\n", length);
  send(sockfd,content_length,strlen(content_length),0);
  const char *connect = "Connection: close\r\n";
  send(sockfd,connect,strlen(connect),0);
  // keep-alive

  const char *blank_line="\r\n";
  send(sockfd, blank_line, strlen(blank_line), 0);
  if (body != NULL) {
    send(sockfd,html, strlen(html), 0);
  }

}

static int GetPath(char *path_dst, const char *path_src) {
  struct stat st;
  strcat(path_dst, conf.root);
  strcat(path_dst, path_src);
  if (stat(path_dst, &st) < 0) {
    // 判断是否存在
    return -1;
  } else {
    if (S_ISDIR(st.st_mode)) {
      // 判断目录
      strcat(path_dst, "index.html");
    }
    return 0;
  }
}
void HandlerStatic(int sockfd, char *path) {
  char root[1024] = {0};
  struct stat st;
  int ret = GetPath(root, path);
  if (ret < 0) {
    /*fprintf(stderr, "path not exist!\n");*/
    Handler_404(sockfd);
  } else {
    // 路径存在, 获取文件大小
    int fd = open(root, O_RDONLY);
    if (fd < 0) {
      /*perror("open");*/
      Handler_403(sockfd);
    } else {
      stat(root, &st);
      HandlerCommon(sockfd, 200, NULL, st.st_size);
      sendfile(sockfd, fd, NULL, st.st_size);
      close(fd);
    }
  }
}
void HandlerDynamic(int sockfd, char *path) {

}
void Handler_200(int sockfd, Request *req) {
  if (strcasecmp(req->method, "HEAD") == 0) {
    HandlerCommon(sockfd, 200, NULL, 0);
  } else if ((strcasecmp(req->method, "GET") == 0)
             && strcasecmp(req->query_string, "") == 0) {
    HandlerStatic(sockfd, req->path);
    printf("HandlerStatic Done!\n");
    /*HandlerCommon(sockfd, 200, "OK");*/
  } else {
    HandlerCommon(sockfd, 200, "OK", 0);

  }
}
void Handler_400(int sockfd) {
  HandlerCommon(sockfd, 400, "Bad Request", 0);
}
void Handler_403(int sockfd) {
  HandlerCommon(sockfd, 403, "Forbidden", 0);
}
void Handler_404(int sockfd) {
  HandlerCommon(sockfd, 404, "Not Found", 0);
}
void Handler_405(int sockfd) {
  HandlerCommon(sockfd, 405, "Method Not Allowed", 0);
}
void Handler_500(int sockfd) {
  HandlerCommon(sockfd, 500, "Internal Server Error", 0);
}
void Handler_501(int sockfd) {
  HandlerCommon(sockfd, 501, "Not Implemented", 0);
}
