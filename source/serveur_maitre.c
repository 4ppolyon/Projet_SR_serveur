#include "csapp.h"

#define MAX_NAME_LEN 256
#define NB_MAX_SLAVE 3

extern int client_down;
int slave_turn, nb_slave;

/* 
 * Note that this code only works with IPv4 addresses
 *
 */
int main(int argc, char **argv){

    int code_type, k, num_port, port;
    uint16_t net_code_type, net_port;
    int listenfd, connfd;
    int L_SERV_PORT[NB_MAX_SLAVE];
    size_t t_nom_serv;
    int32_t net_t_nom_serv;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN], client_hostname[MAX_NAME_LEN], L_SERV[NB_MAX_SLAVE][MAX_NAME_LEN];
    
    nb_slave = 0;
    slave_turn = 0;

    num_port = 2130; 
    port = 2112;

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    printf("Ready\n");
    while (1) {
        // Vérifie que la connexion est réalisée
        while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) == -1);
        client_down = 0;

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string, INET_ADDRSTRLEN);

        // Recupere le type de connexion
        Rio_readn(connfd, &net_code_type, sizeof(uint16_t));
        code_type = ntohs(net_code_type);

        // Traitement selon le type de connexion
        switch(code_type){
            // If the connexion is defined as a new slave
            case 0 :
                printf("Master server connected to new slave %s (%s)\n", client_hostname, client_ip_string);
                nb_slave++;
                // If there is too much slave, it refuses the connexion
                // else, it adds to the table the slave
                if(nb_slave > NB_MAX_SLAVE){
                    printf("New slave denied\n");
                    nb_slave--;
                    port = 0;
                    net_port = htons(port);
                    Rio_writen(connfd,&net_port,sizeof(uint16_t));
                }else{
                    printf("New slave accepted\n");
                    num_port++;
                    port = num_port;
                    strcpy(L_SERV[nb_slave-1],client_hostname);
                    L_SERV_PORT[nb_slave-1]=port;
                    net_port = htons(port);
                    Rio_writen(connfd,&net_port,sizeof(uint16_t));
                }
                Close(connfd);
                break;

            // If the connexion is defined as a client
            case 1:
                printf("Master server connected to client %s (%s)\n", client_hostname, client_ip_string);

                // Give the client a slave server
                t_nom_serv = strlen(L_SERV[slave_turn]);
                net_t_nom_serv = htonl(t_nom_serv);
                Rio_writen(connfd, &net_t_nom_serv, sizeof(uint32_t));
                Rio_writen(connfd, L_SERV[slave_turn], t_nom_serv);
                net_port = htons(L_SERV_PORT[slave_turn]);
                Rio_writen(connfd, &net_port, sizeof(uint16_t));

                // Change wich slave will work next
                slave_turn++;
                if (slave_turn >= nb_slave){
                    slave_turn = 0;
                }
                Close(connfd);
                break;

            // If the connexion is defined as a closing slave
            case 2:
                printf("Connecting to dying slave %s (%s)\n", client_hostname, client_ip_string);
                // Recupere le port de l'esclave mourrant
                Rio_readn(connfd, &net_port, sizeof(uint16_t));
                port = ntohs(net_port);
                for(k = 0; k<nb_slave; k++){
                    // Find the slave in the table
                    if(!strcmp(L_SERV[k],client_hostname) && L_SERV_PORT[k] == port){
                        nb_slave--;
                        // Remove it and replace it by the next element
                        for(k = k; k<nb_slave; k++){
                            strcpy(L_SERV[k],L_SERV[k+1]);
                            L_SERV_PORT[k] = L_SERV_PORT[k+1];
                        }
                        printf("New slave suppressed\n");
                        break;
                    }
                }
                Close(connfd);
                break;
        }
    }
    exit(0);
}

