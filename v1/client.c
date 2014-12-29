#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock_fd, port_no;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    port_no = atoi(argv[2]);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy(
        (char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length
    );
    serv_addr.sin_port = htons(port_no);
    
    int n, i;
    char *msg = "msg";
    for(i = 0; i < 10; ++i){
        if (connect(
                sock_fd,
                (struct sockaddr *) &serv_addr,sizeof(serv_addr)) 
            < 0) 
            error("ERROR connecting");
        
        n = write(sock_fd, msg, strlen(msg));
        if (n < 0) error("ERROR writing to socket");
    }
    // printf("Please enter the message: ");
    //bzero(buffer,256);
    //fgets(buffer,255,stdin);
    //int n = write(sock_fd,buffer,strlen(buffer));
    
    msg = "done.";
    n = write(sock_fd, msg, strlen(msg));
        if (n < 0) error("ERROR writing to socket");

    //bzero(buffer,256);
    //n = read(sock_fd,buffer,255);
    //if (n < 0) error("ERROR reading from socket");
    //printf("%s\n",buffer);

    close(sock_fd);
    return 0;
}
