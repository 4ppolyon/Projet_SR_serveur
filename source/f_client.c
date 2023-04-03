#include <stdio.h>
#include "csapp.h"

int gest_erreur(int code_sortie){
    switch(code_sortie){
        case 1 :
            fprintf(stderr,"Write a file name:\n");
            break;
        case 2 :
            fprintf(stderr,"File error\nCheck the file name or maybe you don't have the right to access this file.\n\n");
            break;
        default :
            return 0;
    }
    return code_sortie;
}

float time_diff(struct timeval *start, struct timeval *end){
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}


void recuperation_fichier(int clientfd, char *path){

    int f;
    float t;
    off_t buf_off;
    uint32_t net_buf_off;
    void *contenu;
    struct timeval start;
    struct timeval end;

    // Téléchargement du fichier
    gettimeofday(&start, NULL);
    Rio_readn(clientfd, &net_buf_off, sizeof(uint32_t));
    buf_off = ntohl(net_buf_off);
    contenu = Malloc(buf_off);
    Rio_readn(clientfd, contenu, buf_off);
    gettimeofday(&end, NULL);
    
    // Affichage des stats de téléchargement
    t = time_diff(&start, &end);
    printf("Download success :\ntime spent: %0.8f sec\nweight = %ld bytes\napprox speed = %f bytes/sec\n\n", t, buf_off, (buf_off/t));

    // écriture du contenu du fichier
    f = Open(path, O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH); 
    Rio_writen(f, contenu, buf_off);
    Close(f);
    Free(contenu);
}