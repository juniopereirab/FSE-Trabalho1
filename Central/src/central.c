#include "socket.h"

void func (int sockfd) {
    char buff[MAX];
    int n;

    while(1) {
        bzero(buff, MAX);
        read(sockfd, buff, sizeof(buff));

        printf("From client: %s\t To Client: ", buff);
        bzero(buff, MAX);
        n = 0;

        while((buff[n++] = getchar()) != '\n');

        write(sockfd, buff, sizeof(buff));

        if(strncmp("exit", buff, 4) == 0){
            printf("Server exit...\n");
            break;
        }
    }
}

int main () {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = configureSocket();

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    bindSocket(sockfd, servaddr);
    listenSocket(sockfd);

    len = sizeof(cli);
    connfd = handleAcceptSocket(sockfd, cli, len);

    func(connfd);
    close(sockfd);

    return 0;
}