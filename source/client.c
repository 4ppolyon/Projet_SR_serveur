/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

int main(int argc, char **argv){

    size_t t_nomf, t_nom_serv;
    int code_sortie, code_type;
    int clientfd, port;
    char *host, *nom_serv, buf[MAXLINE];

    /*
     * Note that the 'host' can be a name or an IP address.
     */
    host = "localhost";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    printf("client connected to master server\n"); 

    // Envoie au serveur qu'il est un client
    code_type = 1;
    Rio_writen(clientfd, &code_type, sizeof(int));

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

    while (fscanf(stdin, "%s", buf) != EOF) {

        // Envoie la taille du nom de fichier puis le nom de fichier
        t_nomf=strlen(buf);
        Rio_writen(clientfd, &t_nomf, sizeof(size_t));
        Rio_writen(clientfd, buf, t_nomf);

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
