#include "socket.h"
#include "definitions.h"

report *reports;
int reportsLength = 0;
int nViolations = 0;
int nRedSign = 0;
int mediaSpeed = 0;

void updateReportsLength () {
    reports = (report *) malloc((reportsLength + 1) * sizeof(report));
    reportsLength++;
}

void addReport (int way, int speed, bool violation) {
    updateReportsLength();
    reports[reportsLength - 1].direction = way;
    reports[reportsLength - 1].speed = speed;
    reports[reportsLength - 1].violation = violation;
}

void func (int sockfd) {
    char buff[MAX];
    int n;

    while(1) {
        bzero(buff, MAX);
        read(sockfd, buff, sizeof(buff));

        int speed = buff[0];
        int direction = buff[1];
        bool violation = buff[2];
        
        addReport(direction, speed, violation);

        int carSpeedSum = 0;

        for(int i = 0; i < reportsLength; i++) {
            if(reports[i].violation && reports[i].speed < 60) {
                nRedSign++;
                nViolations++;
            }

            if(reports[i].speed > 60) {
                nViolations++;
            }

            carSpeedSum += reports[i].speed;
        }

        mediaSpeed = carSpeedSum / reportsLength;
        printf("Quantidade de Carros: %d\n", reportsLength);
        printf("Velocidade média: %d\n", mediaSpeed);
        printf("Ultrapassa sinal vermelho: %d\n", nRedSign);
        printf("Numero de infrações: %d\n", nViolations);
        bzero(buff, MAX);
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