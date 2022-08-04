#include "socket.h"
#include "definitions.h"
#include <errno.h>

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

int main () {
    bool opt = true;
    int sockfd, connfd[4], len, i;
    int max_sd, sd, new_socket, valread, addrlen, activity, max_conn = 4;
    struct sockaddr_in address, cli;
    fd_set readfds;
    char buff[MAX];

    for (i = 0; i < max_conn; i++) {  
        connfd[i] = 0;  
    } 

    sockfd = configureSocket();
    if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  

    bzero(&address, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    bindSocket(sockfd, address);
    listenSocket(sockfd);

    addrlen = sizeof(address);

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        max_sd = sockfd; 

        for ( i = 0 ; i < max_conn ; i++) {  
            sd = connfd[i];  

            if(sd > 0)  
                FD_SET(sd , &readfds);  
                 
            if(sd > max_sd)  
                max_sd = sd;  
        } 

        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR)) {  
            printf("select error");  
        }

        if (FD_ISSET(sockfd, &readfds)) {
            if ((new_socket = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }
            for (i = 0; i < max_conn; i++) {  
                if( connfd[i] == 0 ) {  
                    connfd[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);          
                    break;  
                }
            }  
        }

        for (i = 0; i < max_conn; i++) {  
            sd = connfd[i];  
            if (FD_ISSET(sd , &readfds)) {  
                if ((valread = read( sd , buff, MAX)) == 0) {
                    close( sd );  
                    connfd[i] = 0;
                } else {  
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
        } 
    }

    return 0;
}