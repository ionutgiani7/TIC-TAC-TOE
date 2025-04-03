#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>




int main(int argc,char **argv){
    if(argc != 3){
        printf("./client IP port\n");
        exit(0);
    }
    int sockfd;
    struct sockaddr_in server;
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("SocketError\n");
        exit(1);
    }

    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    if((connect(sockfd,(struct sockaddr *)&server,sizeof(server))) < 0){
        printf("ConnectError\n");
        exit(2);
    }
    printf("Connected\n");
    char buffer[1024];
    int n;
    while((n = read(sockfd,buffer,1023)) > 0){
        printf("%s",buffer);
        if(strstr(buffer,"make a move") == NULL || strstr(buffer,"Invalid input") != NULL){
            printf("Select your move: ");
            memset(buffer,0,1024);
            fgets(buffer,1024,stdin);
            write(sockfd,buffer,strlen(buffer));
        }
    }
    close(sockfd);



    return 0;
}