/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define NB_FILS 1
#define NB_SERV 1

extern int client_down;
int slave_turn;
pid_t PID_pere;
pid_t pid;
pid_t L_fils[NB_FILS];


void SIGINT_handler(int sig){
    Signal(SIGCHLD, SIG_IGN);
    for(int i = 0; i < NB_FILS; i++){
        Kill(L_fils[i], SIGINT);
    }
    while(wait(NULL) > 0);
    exit(0);
}

void SIGPIPE_handler(int sig){
    client_down = 1;
}

void SIGUSR1_handler(int sig){
    slave_turn++;
    if (slave_turn >= NB_SERV){
        slave_turn = 0;
    }
    if (PID_pere == getpid()){
        for(int i = 0; i < NB_FILS; i++){
            Kill(L_fils[i], SIGUSR1);
        }
    }
}

void child_handler(int sig){
    int status;
    int res;
    while ((res = waitpid(-1, &status,WNOHANG)) > 0){
        pid = res;
    }
    if (errno != ECHILD && errno != EXIT_SUCCESS)
        unix_error("waitpid error");
}

/* 
 * Note that this code only works with IPv4 addresses
 *
 */
int main(int argc, char **argv){

    int k;
    int listenfd, connfd, port;
    int L_SERV_PORT[NB_SERV];
    size_t t_nom_ser;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN], client_hostname[MAX_NAME_LEN], L_SERV[NB_SERV][MAX_NAME_LEN];
    sigset_t mask_CHLD, mask_tmp;

    Sigemptyset(&mask_tmp);
    Sigemptyset(&mask_CHLD);
    Sigaddset(&mask_CHLD, SIGCHLD);
    
    Signal(SIGCHLD, child_handler);
    Signal(SIGINT, SIGINT_handler);
    // Utile pour les fils
    Signal(SIGPIPE,SIGPIPE_handler);

    Signal(SIGUSR1,SIGUSR1_handler);
    
    
    pid = 0;
    
    port = 2112;

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    PID_pere=getpid();
    slave_turn = 0;

    for(int i = 0; i < NB_SERV; i++){
       strcpy(L_SERV[i], "localhost");
       L_SERV_PORT[i] = 2121 + i;
    }    

    printf("Ready\n");
    for(int i = 0; i < NB_FILS; i++){
        if ((L_fils[i]=Fork())!=0){
            break;
        }
    }

    while(1){
        if (getpid()!=PID_pere){
            while (1) {
                // Vérifie que la connexion est réaliser
                while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) == -1);
                client_down = 0;
                
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                        INET_ADDRSTRLEN);
                
                printf("Master server connected to %s (%s)\n", client_hostname, client_ip_string);

                // Gestion demande
                t_nom_ser = strlen(L_SERV[slave_turn]);
                Rio_writen(connfd, &t_nom_ser, sizeof(size_t));
                Rio_writen(connfd, L_SERV[slave_turn], t_nom_ser);
                Rio_writen(connfd, &L_SERV_PORT[slave_turn], sizeof(int));
                Kill(PID_pere,SIGUSR1);
                Close(connfd);
            }
        }else{
            pause();
            Sigprocmask(SIG_BLOCK, &mask_CHLD, &mask_tmp);
            fprintf(stderr,"%d\n",pid);
            if (pid != 0){
                for(k = 0; k<NB_FILS && L_fils[k]!= pid; k++);
                if (k<NB_FILS){
                    L_fils[k]=Fork();
                }
                pid = 0;
            }
            Sigprocmask(SIG_SETMASK, &mask_tmp, NULL);
        }
    }
    exit(0);
}

