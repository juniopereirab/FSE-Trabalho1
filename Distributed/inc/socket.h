#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX 100
#define PORT 8080
#define SA struct sockaddr

int configureSocket ();
void bindSocket (int sockfd, struct sockaddr_in servaddr);
void listenSocket (int sockfd);
int handleAcceptSocket (int sockfd, struct sockaddr_in cli, int len);
void connectSocket (int sockfd, struct sockaddr_in servaddr);

#endif