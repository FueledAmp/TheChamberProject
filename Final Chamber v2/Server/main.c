#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

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

void error(const char *msg)
{
    exit(1);
}

void app_exit()
{
  _exit(0);
}

int main(int argc, char *argv[])
{
     DEAMON();
     signal(SIGINT , app_exit);

     int sockfd, newsockfd, portno; //sockfd and newsockfd are file descriptors.
                                    //These two variables store the values returned by the socket system call and the accept system call
                                    //portno stores the port number on which the server accepts connections.
     socklen_t clilen;              //stores the size of the address of the client. This is needed for the accept system call
     char buffer[256];              //The server reads characters from the socket connection into this buffer.
     struct sockaddr_in serv_addr, cli_addr; //A structure containing an internet address. This structure is defined in netinet/in.h.
     int n;                         //return value for the read() and write() calls

     sockfd = socket(AF_INET, SOCK_STREAM, 0); //creates a new socket
     if (sockfd < 0)                           //If error opening socket
        error("ERROR opening socket");
     int option = 1;
     setsockopt(sockfd , SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
     bzero((char *) &serv_addr, sizeof(serv_addr)); //function bzero() sets all values in a buffer serv_adr to zero
     portno = port_here;        //Get integer value of second argument which is the port to be opened
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, //Bind socket
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);          //The listen system call allows the process to listen on the socket for connections
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,  //accept() system call causes the process to block until a client connects to the server
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");

     while(1){ //Start to read and write
        bzero(buffer,256); //Set all values in buffer to zero
        n = read(newsockfd,buffer,255); //Read from socket into buffer
        //recv(sockfd , buffer, sizeof(buffer) , 0);
        //char *p = strchr(buffer, '\n');
        //if (p) *p = '\0';

        if (n <= 0){
          newsockfd = accept(sockfd,  //accept() system call causes the process to block until a client connects to the server
                      (struct sockaddr *) &cli_addr,
                      &clilen);
        }
        else{
          FILE *fl = fopen("rec.txt" , "a");
          //printf("***\n %s \n*** \n", buffer);
          fprintf(fl , "%s", buffer);
          fclose(fl);
        }
      }

      //recv(sockfd , buffer, sizeof(buffer) , 0);
      close(newsockfd);
      close(sockfd);
      _exit(0);
}
