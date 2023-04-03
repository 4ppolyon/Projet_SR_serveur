/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

int main(int argc, char **argv){

    size_t len;
    uint32_t net_len;
    int code_sortie;
    uint16_t net_code_sortie;
    int clientfd, port;
    char *host, buf[300];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    host = "localhost";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n");

    while (Fgets(buf, sizeof(buf), stdin) != NULL) {
        len = strcspn(buf, "\n"); // Trouve la longueur de la chaîne jusqu'au premier '\n'
        buf[len] = '\0'; // Remplace le '\n' par un caractère nul pour terminer la chaîne

        // Envoie la taille du nom de fichier puis le nom de fichier
        net_len = htonl(len);
        Rio_writen(clientfd, &net_len, sizeof(size_t));
        Rio_writen(clientfd, buf, len);

        if (Rio_readn(clientfd, &net_code_sortie, sizeof(uint16_t)) > 0) {
            code_sortie = ntohs(net_code_sortie);
            if (gest_erreur(code_sortie) == 0){
                recuperation_fichier(clientfd, buf);
            }

        } else { /* the server has prematurely closed the connection */
            break;
        }
    }
    Close(clientfd);
    exit(0);
}
