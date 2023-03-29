/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    size_t t_nomf;
    int code_sortie;
    int clientfd, port;
    char *host, buf[MAXLINE], path[MAXLINE];
    off_t buf_off;
    strcpy(path,"./client_file/");

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    host = "f214-06";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    // Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        t_nomf=strlen(buf)-1;
        Rio_writen(clientfd, &t_nomf, sizeof(size_t));
        Rio_writen(clientfd, buf, t_nomf);
        if (Rio_readn(clientfd, &code_sortie, sizeof(int)) > 0) {
            // modulariser le switch (fonction qui retourne 0 quand ok)
            switch(code_sortie){
                case 1 :
                    fprintf(stderr,"Erreur fichier\n");
                    break;
                default :
                    Rio_readn(clientfd, &buf_off, sizeof(off_t));
                    void *contenu = Malloc(buf_off);
                    Rio_readn(clientfd, contenu ,buf_off);
                    strcat(path,buf);
                    int f = Open(path, O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH); ////////// recupe le nom du fichier
                    strcpy(path,"./client_file/");
                    //// faire un write dans le fichier
                    Rio_writen(f, contenu, buf_off);
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
