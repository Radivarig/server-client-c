#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock_fd, new_sock_fd;  //file descriptors
    int port_no;
    socklen_t cli_len;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    if (argc < 2) {
       fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
       error("ERROR opening socket");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    port_no = atoi(argv[1]);   //string to int
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_no);
    
    if (bind(
            sock_fd, 
            (struct sockaddr *) &serv_addr,
            sizeof(serv_addr))
        < 0)
        error("ERROR on binding");
    

    listen(sock_fd, 5);
    cli_len = sizeof(cli_addr);
    
    new_sock_fd = accept(
        sock_fd, 
        (struct sockaddr *) &cli_addr, 
        &cli_len
    );

    if (new_sock_fd < 0) 
        error("ERROR on accept");
    
    bzero(buffer,256);
    
    int n = read(new_sock_fd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    
    printf("Here is the message: %s\n", buffer);
    
    n = write(new_sock_fd,"I got your message", 18);
    if (n < 0) error("ERROR writing to socket");
    
    close(new_sock_fd);
    close(sock_fd);
    return 0; 
}
