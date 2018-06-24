#include <stdio.h>
#include <string.h>
#include "read_config.h"

int read_config(config_t * conf) {
  FILE * fp= fopen(CONFIG_FILE_PATH, "r");
  if (!fp) {
    return -1;
  }
  int port = DEFAULT_PORT;
  char ip[15] = DEFAULT_IP;
  char temp[1024] = { 0 };
  while(!feof(fp)) {
    fgets(temp, sizeof(temp), fp);
    /*fscanf(fp,"server_ip %s",&port);*/
    sscanf(temp, "listen %d", &port);
  }
  fclose(fp);
  strcpy(conf->ip, ip);
  conf->port = port;
  return 0;
}
