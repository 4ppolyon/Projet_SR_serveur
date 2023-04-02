/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

int main(int argc, char **argv){

    size_t len, t_nom_serv;
    int code_sortie;
    int clientfd, port;
    char *host, *nom_serv, buf[MAXLINE];

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
    printf("client connected to master server\n"); 

    // Récuperation du nom de serveur esclave
    Rio_readn(clientfd, &t_nom_serv, sizeof(size_t));
    nom_serv = Calloc(t_nom_serv, sizeof(char)*t_nom_serv);
    Rio_readn(clientfd, nom_serv, t_nom_serv);
    // Récuperation du numero de port
    Rio_readn(clientfd, &port, sizeof(int));
    printf("client connected to slave server\n"); 

    Close(clientfd);
    // Connexion au serveur esclave
    clientfd = Open_clientfd(nom_serv, port);

    while (Fgets(buf, sizeof(buf), stdin) != NULL) {
        len = strcspn(buf, "\n"); // Trouve la longueur de la chaîne jusqu'au premier '\n'
        buf[len] = '\0'; // Remplace le '\n' par un caractère nul pour terminer la chaîne

        // Envoie la taille du nom de fichier puis le nom de fichier
        Rio_writen(clientfd, &len, sizeof(size_t));
        Rio_writen(clientfd, buf, len);

        if (Rio_readn(clientfd, &code_sortie, sizeof(int)) > 0) {

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
