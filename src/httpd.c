/// \file httpd.c
/// \brief 创建 socket, http 响应入口
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#include "httpd.h"
#include "read_config.h"

config_t conf;

int main() {

  daemon(0, 0);
  // 1. 读取配置文件
  int read_config_stat = read_config();
  if (read_config_stat < 0) {
    perror("read_config");
    return -1;
  }

  // 2. 创建 socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return -1;
  }
  // 3. 设置允许端口复用
  int opt=1;
  setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  // 4. bind
  struct sockaddr_in local;
  bzero(&local, sizeof(local));
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = inet_addr(conf.ip);
  local.sin_port = htons((conf.port));
  if (bind(sockfd, (struct sockaddr *)&local, sizeof(local)) < 0) {
    perror("bind");
    return -1;
  }
  // 5. listen
  if (listen(sockfd, LISTEN_SIZE) < 0) {
    perror("listen");
    return -1;
  }
  // 6. 等待连接
  int epoll_fd = epoll_create(10);
  if (epoll_fd < 0) {
    perror("epoll_create");
    return -1;
  }
  SetNonBlock(sockfd);
  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = sockfd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) < 0) {
    perror("epoll_create");
    return -1;
  }
  /*struct sockaddr_in client;*/
  /*socklen_t len = sizeof(client);*/
  // 把标准错误重定向，用于打印日志
  int log_fd = open(conf.log, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (log_fd < 0) {
    perror("打开 log 文件");
    return -1;
  } else {
    dup2(log_fd, 2);
  }
  printf("* Running on http://%s:%d/ (Press CTRL+C to quit)\n", conf.ip, conf.port);
  while (1) {
    /*// 7. accept*/
    /*int new_sock = accept(sockfd, (struct sockaddr *)&client, &len);*/
    /*if (new_sock < 0) {*/
    /*fprintf(stderr, "accept error!\n");*/
    /*perror("accept");*/
    /*continue;*/
    /*}*/
    /*printf("new connect\n");*/
    /*res_param_t *res_param = (res_param_t *)malloc(sizeof(res_param_t));*/
    /*res_param->sockfd = new_sock;*/
    /*strcpy(res_param->client_ip, inet_ntoa(client.sin_addr));*/
    /*pthread_t thread_id;*/
    /*pthread_create(&thread_id, 0, handler_request, res_param);*/
    /*pthread_detach(thread_id);*/
    struct epoll_event events[1024];
    int size = epoll_wait(epoll_fd, events, sizeof(events)/sizeof(events[0]), -1);
    if (size < 0) {
      perror("epoll_wait");
      continue;
    } else if (size == 0) {
      continue;
    } else {
      for (int i = 0; i < size; i++) {
        if (!(events[i].events & EPOLLIN)) {
          continue;
        }
        if (events[i].data.fd == sockfd) {
          ProcessConnect(sockfd, epoll_fd);
        } else {
          ProcessRequest(events[i].data.fd, epoll_fd);
        }
      }
    }
  }
  // 8. close
  close(log_fd);
  return 0;
}
