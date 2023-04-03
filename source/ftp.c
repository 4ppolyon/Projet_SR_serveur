/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"

int client_down;

void ftp(int connfd) {

    int f;
    int code_sortie;
    uint16_t net_code_sortie;
    size_t t_nomf;
    uint32_t net_t_nomf;
    struct stat stat_f;
    off_t buf_off;
    uint32_t net_buf_off;
    char *buf, path[MAXLINE];
    void *contenu;

    while (1){ /* le fichier n'existe pas */
        strcpy(path,"./serveur_file/");

        // Récupère la taille du nom de fichier
        Rio_readn(connfd, &net_t_nomf, sizeof(uint32_t));
        t_nomf = ntohl(net_t_nomf);
        // Récupère le contenu du socket
        buf = Calloc(t_nomf,sizeof(char)*t_nomf);
        Rio_readn(connfd, buf, sizeof(char)*t_nomf);

        strcat(path,buf);
        if(client_down == 1){
            return;
        }else if ((strlen(buf) == 0)) {
            code_sortie = 1;
            net_code_sortie = htons(code_sortie);
            Rio_writen(connfd, &net_code_sortie, sizeof(uint16_t));
            Free(buf);
        }else if ((f = open(path, O_RDONLY, 0)) < 0) {
            code_sortie = 2;
            net_code_sortie = htons(code_sortie);
            Rio_writen(connfd, &net_code_sortie, sizeof(uint16_t));
            Free(buf);
        }else {
            code_sortie = 0;
            net_code_sortie = htons(code_sortie);
            Rio_writen(connfd, &net_code_sortie, sizeof(uint16_t));
            Free(buf);

            // Recupere status fichier
            fstat(f,&stat_f);
            buf_off = stat_f.st_size;
            contenu = Malloc(buf_off);
            
            // envoie la taille du fichier
            net_buf_off = htonl(buf_off);
            Rio_writen(connfd, &net_buf_off, sizeof(uint32_t));
            // lit le contenu du fichier
            Rio_readn(f, contenu, buf_off);
            // envoie le contenu du fichier
            Rio_writen(connfd, contenu, buf_off);
            Free(contenu);
        }
    }
}
