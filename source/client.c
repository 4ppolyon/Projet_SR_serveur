/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

int main(int argc, char **argv){

    size_t t_nomf;
    int code_sortie;
    int clientfd, port;
    char *host, buf[MAXLINE], path[MAXLINE];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    host = "f217-02";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 

    while (fscanf(stdin, "%s", buf) != EOF) {

        //Initialise le chemin de dépot des fichiers
        strcpy(path,"./client_file/");

        // Envoie la taille du nom de fichier puis le nom de fichier
        t_nomf=strlen(buf);
        Rio_writen(clientfd, &t_nomf, sizeof(size_t));
        Rio_writen(clientfd, buf, t_nomf);

        if (Rio_readn(clientfd, &code_sortie, sizeof(int)) > 0) {

            if (gest_erreur(code_sortie) == 0){
                strcat(path,buf);
                recuperation_fichier(clientfd, path);
            }

        } else { /* the server has prematurely closed the connection */
            break;
        }
    }
    Close(clientfd);
    exit(0);
}
