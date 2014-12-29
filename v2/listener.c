// datagram socket server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MYPORT "4950"
#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd;                             //socket file descritor, integer
    struct addrinfo hints,                  //data of type addrinfo 
           *servinfo;                       //pointer to data of type addrinfo that will be filled 
    struct sockaddr_storage their_addr;     
    char buf[MAXBUFLEN];                    //buffer
    socklen_t addr_len;                     //unsigned opaque integer length of at least 32 bits
    char s[INET6_ADDRSTRLEN];               //INET6_ADDRSTRLEN constant int for IPv6 addrstrlen, > IPv4 addrstrlen

    memset(&hints, 0, sizeof hints);        //set all bytes to zero
    hints.ai_family = AF_UNSPEC;            //integer, any AF_INET or AF_INET6, address family
    hints.ai_socktype = SOCK_DGRAM;         //integer, socket datagram
    hints.ai_flags = AI_PASSIVE;            //integer, address info, use my IP

    int rv;
    if (
        (rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo))     != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    struct addrinfo *p;
    for(p = servinfo;    p != NULL;    p = p->ai_next)
    {
        if (
            (sockfd = socket(p->ai_family, 
                             p->ai_socktype,
                             p->ai_protocol))   == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (
            bind(sockfd,
                 p->ai_addr,
                 p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;

    int numbytes;
    if (
        (numbytes = recvfrom(sockfd,
                             buf,
                             MAXBUFLEN -1,
                             0,
                             (struct sockaddr *)&their_addr,
                             &addr_len))                            == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family,
                                                       get_in_addr((struct sockaddr *)&their_addr),
                                                       s,
                                                       sizeof s) );
    
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);

    return 0;
}
