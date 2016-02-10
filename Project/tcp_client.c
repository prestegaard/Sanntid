/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */

// ./tcp_client 192.241.187.145 20017
// gcc -std=gnu99 -Wall -g -o tcp_client tcp_client.c -lpthread

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");


    
    /* get message line from the user */
    void* tcp_send(){
        while(1){
            printf("Please enter msg: ");
            bzero(buf, BUFSIZE);
            fgets(buf, BUFSIZE, stdin);   
            /* send the message line to the server */
            n = write(sockfd, buf, strlen(buf));
            if (n < 0) 
              error("ERROR writing to socket");
        }
    }
    /* print the server's reply */
    

    void* tcp_recv(){
        while(1){
            bzero(buf, BUFSIZE);
            n = read(sockfd, buf, BUFSIZE);
            if (n < 0) 
              error("ERROR reading from socket");
            printf("Echo from server: %s", buf);
        }
    }
   
    pthread_t thread1;
    pthread_t thread2;
    
    pthread_create(&thread1, NULL, tcp_send, NULL);
    pthread_create(&thread2, NULL, tcp_recv, NULL);
    
    if(!pthread_join(thread1, NULL))
        printf("Thread1_join sucsess\n");
    if(!pthread_join(thread2, NULL))
        printf("Thread2_join sucsess\n");
    printf("Hello from main!\n");

    close(sockfd);
    return 0;
}
