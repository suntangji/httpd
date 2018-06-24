#include "httpd.h"
#include "read_config.h"

int main() {

  // 1. 读取配置文件
  config_t conf;
  int read_config_stat = read_config(&conf);
  if (read_config_stat < 0) {
    fprintf(stderr, "read config error!\n");
    return -1;
  }
  // 2. 创建 socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "create socker error");
    return -1;
  }
  // 3. 设置允许端口复用
  int opt=1;
  setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  // 4. bind
  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = inet_addr(conf.ip);
  local.sin_port = htons((conf.port));
  if (bind(sockfd, (struct sockaddr*)&local, sizeof(local)) < 0) {
    fprintf(stderr, "bind error!\n");
    return -1;
  }
  // 5. listen
  if (listen(sockfd, LISTEN_SIZE) < 0) {
    fprintf(stderr, "listen error!\n");
    return -1;
  }
  // 6. 等待连接
  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  printf("* Running on http://%s:%d/ (Press CTRL+C to quit)\n", conf.ip, conf.port);
  while (1) {
    // 7. accept
    int new_sock = accept(sockfd, (struct sockaddr*)&client, &len);
    if (new_sock < 0) {
      fprintf(stderr, "accept error!\n");
      continue;
    }
#ifdef DEBUG
    printf("A new client has been connected!\n");
#endif
    pthread_t thread_id;
    int *sock = &new_sock;
    pthread_create(&thread_id, 0, handler_request, (void*)sock);
    pthread_detach(thread_id);
  }
  // 8. close
  return 0;
}
