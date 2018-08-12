#include "httpd.h"
void SetNonBlock(int fd) {
  int f1 = fcntl(fd, F_GETFL);
  if (f1 < 0) {
    perror("fcntl");
    return;
  }
  fcntl(fd, F_SETFL, f1 | O_NONBLOCK);
}
void ProcessConnect(int listen_fd, int epoll_fd) {
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  int connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
  if (connect_fd < 0) {
    perror("accept");
    return;
  }
  SetNonBlock(connect_fd);
  struct epoll_event ev;
  ev.data.fd = connect_fd;
  ev.events = EPOLLIN | EPOLLET;
  int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &ev);
  if (ret < 0) {
    perror("epoll_ctl");
    return;
  }
  return;
}
ssize_t NonBlockRead(int fd, char *buf, int size) {
  (void) size;
  ssize_t total_size = 0;
  while (1) {
    ssize_t cur_size = read(fd, buf + total_size, 1024);
    total_size += cur_size;
    if (cur_size < 1024 || errno == EAGAIN) {
      break;
    }
  }
  buf[total_size] = '\0';
  return total_size;
}
void ProcessRequest(int connect_fd, int epoll_fd) {
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, connect_fd, NULL);
  res_param_t *res_param = (res_param_t *)malloc(sizeof(res_param_t));
  res_param->sockfd = connect_fd;
  res_param->epollfd = epoll_fd;
  pthread_t thread_id;
  pthread_create(&thread_id, 0, handler_request, res_param);
  pthread_detach(thread_id);
}
