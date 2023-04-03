#include "csapp.h"
#include "ftp.h"

#define MAX_NAME_LEN 256
#define NB_FILS 1

extern int client_down;
int port;
pid_t pid;

pid_t L_fils[NB_FILS];

void SIGINT_handler(int sig){
    char *host ="f217-03";
    int clientfd, code_type, origin_port = 2112;
    uint16_t net_code_type, net_port;
    Signal(SIGCHLD, SIG_IGN);
    for(int i = 0; i < NB_FILS; i++){
        Kill(L_fils[i], SIGINT);
    }
    while(wait(NULL) > 0);
    // Annonce au client que l'esclave meurt
    clientfd = Open_clientfd(host, origin_port);
    code_type = 2;
    net_code_type = htons(code_type);
    Rio_writen(clientfd, &net_code_type, sizeof(uint16_t));
    net_port = htons(port);
    Rio_writen(clientfd, &net_port, sizeof(uint16_t));
    Close(clientfd);
    exit(0);
}

void SIGPIPE_handler(int sig){
    client_down = 1;
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

    int k, code_type;
    uint16_t net_code_type, net_port; 
    int listenfd, connfd, clientfd;
    pid_t PID_pere;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char *host;
    char client_ip_string[INET_ADDRSTRLEN], client_hostname[MAX_NAME_LEN];
    sigset_t mask_CHLD, mask_tmp;

    Signal(SIGPIPE,SIGPIPE_handler);
    Signal(SIGCHLD, child_handler);
    Signal(SIGINT, SIGINT_handler);
    
    Sigemptyset(&mask_tmp);
    Sigemptyset(&mask_CHLD);
    Sigaddset(&mask_CHLD, SIGCHLD);
    
    pid = 0;
    
    host = "localhost";
    port = 2112;

    // Essaye de se connecter au maitre pour récupérer son port pour le serveur et donner son identification
    clientfd = Open_clientfd(host, port);
    code_type = 0;
    net_code_type = htons(code_type);
    Rio_writen(clientfd, &net_code_type, sizeof(uint16_t));
    
    // Récupere le port
    Rio_readn(clientfd, &net_port, sizeof(uint16_t));
    port = ntohs(net_port);
    
    // Ferme la connexion
    Close(clientfd);


    // Le serveur se ferme s'il échoue
    if(port == 0){
        printf("No more place with master\n");
        exit(0);
    }

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    PID_pere=getpid();

    printf("Ready\n");
    for(int i = 0; i < NB_FILS; i++){
        if ((L_fils[i]=Fork())!=0){
            break;
        }
    }

    while(1){
        if (getpid()!=PID_pere){
            while (1) {
                // Vérifie que la connexion est réalisée
                while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) == -1);
                client_down = 0;
                
                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                
                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                        INET_ADDRSTRLEN);
                
                printf("Slave server connected to %s (%s)\n", client_hostname, client_ip_string);

                // Gestion demande
                ftp(connfd);

                Close(connfd);
            }
        }else{
            pause();
            Sigprocmask(SIG_BLOCK, &mask_CHLD, &mask_tmp);
            fprintf(stderr,"%d\n",pid);
            // Si il y a un enfant de mort, on en recrer un
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

