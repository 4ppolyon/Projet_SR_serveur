/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define NB_FILS 10

pid_t L_fils[NB_FILS];

void ftp(int connfd);

void SIGINT_handler(int sig){
    for(int i = 0; i < NB_FILS; i++){
        Kill(L_fils[i], SIGINT);
    }
    while(wait(NULL) > 0);
    exit(0);
}

void child_handler(int sig){
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status,WNOHANG)) > 0);
    if (errno != ECHILD && errno != EXIT_SUCCESS)
        unix_error("waitpid error");
}

/* 
 * Note that this code only works with IPv4 addresses
 *
 */
int main(int argc, char **argv){

    int listenfd, connfd, port;
    pid_t PID_pere;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN], client_hostname[MAX_NAME_LEN];
    
    Signal(SIGCHLD, child_handler);
    Signal(SIGINT, SIGINT_handler);
    
    port = 2112;

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    PID_pere=getpid();

    for(int i = 0; i < NB_FILS; i++){
        if ((L_fils[i]=Fork())!=0){
            break;
        }
    }

    if (getpid()!=PID_pere){
        while (1) {
            // Vérifie que la connexion est réaliser
            while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) == -1);
            
            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);
            
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                    INET_ADDRSTRLEN);
            
            printf("server connected to %s (%s)\n", client_hostname,
                client_ip_string);
            
            ftp(connfd);
            Close(connfd);
        }
    }else{
        while(1){
            pause();
        }
    }
    exit(0);
}

