/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"

void ftp(int connfd) {
    int f;
    // size_t n;
    char buf[MAXLINE];
    int code_sortie;
    rio_t rio;
    struct stat stat_f;
    off_t buf_off;

    // Intialise la lecture du socket
    Rio_readinitb(&rio, connfd);
    // Recupère le contenue du socket
    Rio_readlineb(&rio, buf, MAXLINE);

    while ((f = Open(buf, O_RDONLY, 0)) < 0){ /* le fichier n'existe pas */
        code_sortie = 1;
        Rio_writen(connfd, &code_sortie, sizeof(int));
        Rio_readlineb(&rio, buf, MAXLINE);
    }
    code_sortie = 0;
    Rio_writen(connfd, &code_sortie, sizeof(int));

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

//     while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
//         printf("server received %u bytes\n", (unsigned int)n);
//         Rio_writen(connfd, buf, n);
//     }
}

