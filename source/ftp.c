/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"

void ftp(int connfd) {
    // rio_t rio;
    // size_t n;
    int f;
    int code_sortie;
    size_t t_nomf;
    struct stat stat_f;
    off_t buf_off;
    char *buf, path[MAXLINE];
    strcpy(path,"./serveur_file/");

    // Recupère la taille du nom de fichier
    Rio_readn(connfd, &t_nomf, sizeof(size_t));
    // Alloue et recupère le contenue du socket
    buf = Calloc(t_nomf,sizeof(char)*t_nomf);
    Rio_readn(connfd, buf, sizeof(char)*t_nomf);

    strcat(path,buf);
    while ((f = open(path, O_RDONLY, 0)) < 0){ /* le fichier n'existe pas */
        code_sortie = 1;
        Rio_writen(connfd, &code_sortie, sizeof(int));
        Free(buf);

        // Recupère la taille du nom de fichier
        Rio_readn(connfd, &t_nomf, sizeof(size_t));
        // Recupère le contenue du socket
        buf
         = Calloc(t_nomf,sizeof(char)*t_nomf);
        Rio_readn(connfd, buf, sizeof(char)*t_nomf);
    }

    code_sortie = 0;
    Rio_writen(connfd, &code_sortie, sizeof(int));
    Free(buf);

    // Recupere status fichier
    fstat(f,&stat_f);
    buf_off = stat_f.st_size;
    void *contenu = Malloc(buf_off);
    // envoie la taille du fichier
    Rio_writen(connfd, &buf_off, sizeof(off_t));
    // lit le contenu du fichier
    Rio_readn(f, contenu, buf_off);
    // envoie le contenu du fichier
    Rio_writen(connfd, contenu, buf_off);
    Free(contenu);
    
}
