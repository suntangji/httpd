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
  /*const char *connect = "Connection: close\r\n";*/
  /*send(sockfd,connect,strlen(connect),0);*/
  // keep-alive

  const char *blank_line="\r\n";
  send(sockfd, blank_line, strlen(blank_line), 0);
  if (body != NULL) {
    send(sockfd,html, strlen(html), 0);
  }

}

static int GetPath(char *path_dst, const char *path_src) {
  struct stat st;
  strcpy(path_dst, conf.root);
  size_t path_len = strlen(path_dst);
  if (path_dst[path_len - 1] == '/') {
    path_dst[path_len - 1] = '\0';
  }
  strcat(path_dst, path_src);
  if (stat(path_dst, &st) < 0) {
    // 判断是否存在
    return -1;
  } else {
    if (S_ISDIR(st.st_mode)) {
      // 判断目录
      if (path_dst[strlen(path_dst) - 1] == '/') {
        strcat(path_dst, "index.html");
      } else {
        strcat(path_dst, "/index.html");
      }
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
      Handler_403(sockfd);
    } else {
      stat(root, &st);
      HandlerCommon(sockfd, 200, NULL, st.st_size);
      sendfile(sockfd, fd, NULL, st.st_size);
      close(fd);
    }
  }
}

void GetCGIPath(char *path_dst, const char *path_src) {
  strcpy(path_dst, conf.root);
  if (path_dst[strlen(path_dst) - 1] == '/') {
    strcat(path_dst, "cgi-bin");
  } else {
    strcat(path_dst, "/cgi-bin");
  }
  strcat(path_dst, path_src);

  /*printf("%s\n", path_dst);*/
}

void HandlerCGI(int sockfd, Request *req, const char *path) {
  printf("HandlerCGI\n");
  int father_pipe[2];
  int child_pipe[2];
  if (pipe(father_pipe) < 0) {
    Handler_500(sockfd);
    return;
  }
  if (pipe(child_pipe) < 0) {
    close(father_pipe[0]);
    close(father_pipe[1]);
    Handler_500(sockfd);
    return;
  }
  int father_output = father_pipe[0];
  int father_input = father_pipe[1];
  int child_output = child_pipe[0];
  int child_input = child_pipe[1];
  int ret = fork();
  if (ret > 0) {
    // father
    close(father_output);
    close(child_input);
    int ch;
    /// \brief for 把 body 部分写入管道
    for (int i = 0; i < req->content_lens; ++i) {
      recv(sockfd, &ch, 1, 0);
      write(father_input, &ch, 1);
    }

    char *buf = (char *)malloc(1024 * 1000);
    int i = 0;
    while (read(child_output, &ch, 1) > 0) {
      /*send(sockfd, &ch, 1, 0);*/
      buf[i++] = ch;
    }
    buf[i] = '\0';
    HandlerCommon(sockfd, 200, "OK", strlen(buf));
    send(sockfd, buf, strlen(buf), 0);

    waitpid(ret, NULL, 0);
    close(father_input);
    close(child_output);
  } else if (ret == 0) {
    // child
    close(father_input);
    close(child_output);
    dup2(child_input, 1);
    dup2(father_output, 0);
    char method_env[20];
    char query_string_env[1024 * 2];
    char content_length_env[40];
    /*char content_length_env[30];*/
    sprintf(method_env, "METHOD=%s", req->method);
    putenv(method_env);
    if (strcasecmp(req->method, "GET") == 0) {
      sprintf(query_string_env, "QUERY_STRING=%s", req->query_string);
      putenv(query_string_env);
    } else {
      sprintf(content_length_env, "CONTENT_LENGTH=%d", req->content_lens);
      putenv(content_length_env);
    }
    execl(path, path, NULL);
    exit(1);
  } else {
    Handler_500(sockfd);
    return;
  }
}
void HandlerDynamic(int sockfd, Request *req) {
  // cgi
  char path[1024];
  GetCGIPath(path, req->path);
  struct stat st;
  if (stat(path, &st) < 0) {
    Handler_404(sockfd);
  } else if (S_ISDIR(st.st_mode)) {
    /*printf("isdir:%s\n", path);*/
    Handler_403(sockfd);
  } else if ((st.st_mode & S_IXUSR)
             || (st.st_mode & S_IXGRP)
             || (st.st_mode & S_IXOTH)) {
    // cgi
    HandlerCGI(sockfd, req, path);
  } else {
    Handler_403(sockfd);
  }
  return;
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
    /*HandlerCommon(sockfd, 200, "OK", 0);*/
    HandlerDynamic(sockfd, req);
    printf("HandlerDynamic Done!\n");

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
