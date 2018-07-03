/// \file read_config.h
/// \brief 配置文件相关
/// \author suntangji, suntangj2016@gmail.com
/// \version 1.0
/// \date 2018-07-01
#ifndef __READ_CONFIG_H__
#define __READ_CONFIG_H__

typedef struct {
  char ip[20];
  char root[100];
  char log[100];
  int port;
} config_t;

extern config_t conf;


#define CONFIG_FILE_PATH "/home/stj/code/project/httpd/conf/httpd.conf"
#define ROOT "./wwwroot"
#define LOG_PATH "./logs/httpd.log"
#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_PORT 8000

int read_config();
#endif /* end of include guard: __READ_CONFIG_H__ */
