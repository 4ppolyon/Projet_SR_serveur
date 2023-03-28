/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"

void ftp(int connfd) {
    // rio_t rio;
    // size_t n;
    int f;
    int t_nomf, code_sortie;
    struct stat stat_f;
    off_t buf_off;

    // Intialise la lecture du socket
    // Rio_readinitb(&rio, connfd);

    // Recupère la taille du nom de fichier
    Rio_readn(connfd, &t_nomf, sizeof(int));
    fprintf(stderr, "%d\n", t_nomf);
    // Recupère le contenue du socket
    char *buf = Malloc(sizeof(char)*t_nomf);
    Rio_readn(connfd, buf, t_nomf);

    fprintf(stderr, "aaaaaaaaaaaaaaaaaaa\n");
    fprintf(stderr, "%s\n", buf);
    fprintf(stderr, "bbbbbbbbbbbbbbbbbbb\n");

    while ((f = open(buf, O_RDONLY, 0)) < 0){ /* le fichier n'existe pas */
        code_sortie = 1;
        Rio_writen(connfd, &code_sortie, sizeof(int));


        // Recupère la taille du nom de fichier
        Rio_readn(connfd, &t_nomf, sizeof(int));
        fprintf(stderr, "%d\n", t_nomf);
        // Recupère le contenue du socket
        char *buf = Malloc(sizeof(char)*t_nomf);
        Rio_readn(connfd, buf, t_nomf);

        fprintf(stderr, "aaaaaaaaaaaaaaaaaaa\n");
        fprintf(stderr, "%s\n", buf);
        fprintf(stderr, "bbbbbbbbbbbbbbbbbbb\n");


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

//     while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
//         printf("server received %u bytes\n", (unsigned int)n);
//         Rio_writen(connfd, buf, n);
//     }
}

