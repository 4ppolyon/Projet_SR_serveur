/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    off_t buf_off;
    rio_t rio;

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    host = "f212-04";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        if (Rio_readlineb(&rio, buf, sizeof(int)) > 0) {
            // modulariser le switch (fonction qui retourne 0 quand ok)
            switch(*buf){
                case 1 :
                    fprintf(stderr,"Erreur fichier\n");
                    break;
                default :
                    Rio_readlineb(&rio, &buf_off, sizeof(off_t));
                    void *contenu = Malloc(buf_off);
                    Rio_readlineb(&rio, contenu ,buf_off);
                    int f = Open("nom_fichier", O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH); ////////// recupe le nom du fichier
                    //// faire un write dans le fichier
                    Rio_writen(f, buf, strlen(buf));
                    Close(f);
                    Free(contenu);
            }
        } else { /* the server has prematurely closed the connection */
            break;
        }
    }
    Close(clientfd);
    exit(0);
}
