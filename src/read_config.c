/// \file read_config.c
/// \brief 读取配置文件
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#include <stdio.h>
#include <string.h>
#include "read_config.h"

int read_config() {
  /*printf("%s\n", CONFIG_FILE_PATH);*/
  FILE *fp= fopen(CONFIG_FILE_PATH, "r");
  if (!fp) {
    return -1;
  }
  int port = DEFAULT_PORT;
  char ip[20] = DEFAULT_IP;
  char root[100] = ROOT;
  char temp[1024] = { 0 };
  while (!feof(fp)) {
    fgets(temp, sizeof(temp), fp);
    /*fscanf(fp,"server_ip %s",&port);*/
    if (temp[0] == '#') {
      continue;
    }
    int read_listen= sscanf(temp, "listen %d", &port);
    if (read_listen > 0) {
      continue;
    }
    int read_host = sscanf(temp, "host %s", ip);
    if (read_host > 0) {
      continue;
    }
    int read_root = sscanf(temp, "root %s", root);
    if (read_root > 0) {
      continue;
    }
  }
  fclose(fp);
  strcpy(conf.ip, ip);
  conf.port = port;
  strcpy(conf.root, root);
  /*printf("%s %d %s\n", conf->ip, conf->port, conf->root);*/
  return 0;
}
