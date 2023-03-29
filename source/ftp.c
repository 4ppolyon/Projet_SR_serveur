/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"

int client_down;

void ftp(int connfd) {

    int f;
    int code_sortie;
    size_t t_nomf;
    struct stat stat_f;
    off_t buf_off;
    char *buf, path[MAXLINE];
    void *contenu;

    while (1){ /* le fichier n'existe pas */
        strcpy(path,"./serveur_file/");

        // Recupère la taille du nom de fichier
        Rio_readn(connfd, &t_nomf, sizeof(size_t));
        // Recupère le contenue du socket
        buf = Calloc(t_nomf,sizeof(char)*t_nomf);
        Rio_readn(connfd, buf, sizeof(char)*t_nomf);

        strcat(path,buf);
        if(client_down == 1){
            return;
        }else if ((strlen(buf) == 0)) {
            code_sortie = 1;
            Rio_writen(connfd, &code_sortie, sizeof(int));
            Free(buf);
        }else if ((f = open(path, O_RDONLY, 0)) < 0) {
            code_sortie = 2;
            Rio_writen(connfd, &code_sortie, sizeof(int));
            Free(buf);
        }else {
            code_sortie = 0;
            Rio_writen(connfd, &code_sortie, sizeof(int));
            Free(buf);

            // Recupere status fichier
            fstat(f,&stat_f);
            buf_off = stat_f.st_size;
            contenu = Malloc(buf_off);

            //test si c'est un repertoire
            // if (stat_f.st_mode == S_IFDIR){
            //     continue;
            // }
            
            // envoie la taille du fichier
            Rio_writen(connfd, &buf_off, sizeof(off_t));
            // lit le contenu du fichier
            Rio_readn(f, contenu, buf_off);
            // envoie le contenu du fichier
            Rio_writen(connfd, contenu, buf_off);
            Free(contenu);
        }
    }


}
