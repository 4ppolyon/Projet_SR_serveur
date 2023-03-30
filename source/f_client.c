#include <stdio.h>
#include <string.h>
#include "csapp.h"

int gest_erreur(int code_sortie){
    switch(code_sortie){
        case 1 :
            fprintf(stderr,"Ecriver un nom de fichier\n");
            break;
        case 2 :
            fprintf(stderr,"Erreur fichier\n");
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
    off_t buf_off;
    void *contenu;
    struct timeval start;
    struct timeval end;

    // Téléchargement du fichier
    gettimeofday(&start, NULL);
    Rio_readn(clientfd, &buf_off, sizeof(off_t));
    contenu = Malloc(buf_off);
    Rio_readn(clientfd, contenu, buf_off);
    gettimeofday(&end, NULL);
    printf("Download success :\ntime spent: %0.8f sec\nweight = %ld octet(s)\n",time_diff(&start, &end),buf_off);

    // écriture du fichier du fichier
    f = Open(path, O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH); 
    Rio_writen(f, contenu, buf_off);
    Close(f);
    Free(contenu);
}