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


void recuperation_fichier(int clientfd, char *nom_fichier){

    off_t contenu_rest, t_bloc = 1000000; // Bloc de 1 Mo
    int nb_bloc;
    char path[300],final_path[300];

    int f;
    float t;
    off_t buf_off;
    void *contenu, *bloc;
    struct timeval start;
    struct timeval end;
    
    //Initialise le chemin de dépot de fichier masquer
    strcpy(path, "./client_file/.");
    strcat(path, nom_fichier);

    f = Open(path, O_TRUNC | O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH); 

    // Téléchargement du fichier
    gettimeofday(&start, NULL);
    Rio_readn(clientfd, &buf_off, sizeof(off_t));

    // Calcul du nombre de bloc
    nb_bloc = buf_off/t_bloc;
    contenu_rest = buf_off-(nb_bloc*t_bloc);


    // Récuperation des blocs
    bloc = Malloc(t_bloc);
    for (int i = 0; i<nb_bloc; i++){
        // lit le contenu du bloc 
        Rio_readn(clientfd, bloc, t_bloc);    
        // ecrit le contenu du bloc dans le fichier
        Rio_writen(f, bloc, t_bloc);
    }

    // Récupération du reste du contenue du fichier
    contenu = Malloc(contenu_rest);
    // lit le reste du contenue
    Rio_readn(clientfd, contenu, contenu_rest);
    // écri le reste du contenue dans le fichier
    Rio_writen(f, contenu, contenu_rest);

    gettimeofday(&end, NULL);
    // Affichage des stats de telechargement
    t = time_diff(&start, &end);
    printf("Download success :\ntime spent: %0.8f sec\nweight = %ld bytes\napprox speed = %f bytes/sec\n", t, buf_off, (buf_off/t));

    // Libération mémoire
    Close(f);
    Free(contenu);
    Free(bloc);

    // Renomage du fichier
    strcpy(final_path, "./client_file/");
    strcat(final_path, nom_fichier);
    rename(path,final_path);
}

