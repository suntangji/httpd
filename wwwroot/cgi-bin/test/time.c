#include<time.h>
#include<stdio.h>
int main() {
  time_t now;
  struct tm *timenow;
  time(&now);
  timenow = localtime(&now);
  /*printf("%s\n",asctime(timenow));*/
  char ch = 'a';

  if (ch == 'a')
    printf("%d/%d/%d %d:%d:%d\n", timenow->tm_year + 1900, timenow->tm_mon + 1,
           timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
  else
    printf("参数不合法！");
}
