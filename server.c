#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void error(char *_msg)
{
    int len = strlen(_msg) + 9; 
    char msg[len];
    strcpy(msg, "Error: ");
    strcat(msg, _msg);
    strcat(msg, "\n");
    perror(msg);
    exit(1);
}

//argc is number or arguments, argv is array of arrays (of arguments)
int main(int argc, char *argv[])
{
    //time_t start_time = time(NULL);
    //double allowed_time = 200;

    int sock_fd, new_sock_fd;  //file descriptor, index of entry in kernel-resident array data structure that contains details of open files
    int port_no;
    socklen_t cli_len;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    
    if (argc < 2)
        error("no port provided");

    //SOCK_STREAM is UTP, SOCK_DGRAM is DCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
       error("opening socket");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    port_no = atoi(argv[1]);   //arrayofchars to int
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_no);
    
    if (bind(
            sock_fd, 
            (struct sockaddr *) &serv_addr,
            sizeof(serv_addr))
        < 0)
        error("on binding");
    

    /*double passed_sec = 0;
    while(passed_sec < 5){
        passed_sec = difftime(time(NULL), start_time);
    }*/
    
    listen(sock_fd, 5);
    cli_len = sizeof(cli_addr);
    
    new_sock_fd = accept(
        sock_fd, 
        (struct sockaddr *) &cli_addr, 
        &cli_len
    );

    if (new_sock_fd < 0) error("on accept");
    
    bzero(buffer,256);
    
    int n = read(new_sock_fd, buffer, 255);
    if (n < 0) error("reading from socket");
    
    printf("Here is the message: %s\n", buffer);
   
    n = write(new_sock_fd,"bla", 18);
    if (n < 0) error("writing to socket");
    
    close(new_sock_fd);
    close(sock_fd);
    return 0;
}
