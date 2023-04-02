/*
 * echo - read and echo text lines until client closes connection
 */
#include <sys/stat.h>
#include "csapp.h"


            //// test si c'est un repertoire
            // if (stat_f.st_mode == S_IFDIR){
            //     continue;
            // }
int client_down;

void ftp(int connfd) {

    off_t contenu_rest, t_bloc = 1000000; // Bloc de 1 Mo
    int nb_bloc;

    int f;
    int code_sortie;
    size_t t_nomf, decal;
    struct stat stat_f;
    off_t buf_off;
    char *buf, path[MAXLINE];
    void *contenu, *bloc;

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
            // envoie la taille du fichier
            Rio_writen(connfd, &buf_off, sizeof(off_t));

            //Décalage si il y a déjà des element télécharger (peut etre egal à 0)
            Rio_readn(connfd, &decal, sizeof(off_t));  
            Lseek(f,decal,SEEK_CUR);
            buf_off = buf_off - decal;

            // Calcul du nombre de blocs et du contenu restant 
            nb_bloc = buf_off/t_bloc;
            contenu_rest = buf_off-(nb_bloc*t_bloc);

            // Envoi des blocs
            bloc = Malloc(t_bloc);
            for (int i = 0; i<nb_bloc; i++){
                // lit le contenu du fichier dans un bloc
                Rio_readn(f, bloc, t_bloc);    
                // envoie le contenu du bloc
                Rio_writen(connfd, bloc, t_bloc);
            }

            if (contenu_rest != 0){
                contenu = Malloc(contenu_rest);
                // lit le reste du contenu du fichier
                Rio_readn(f, contenu, contenu_rest);
                // envoie le reste du contenu du fichier
                Rio_writen(connfd, contenu, contenu_rest);
                Free(contenu);
            }
            
            Free(bloc);
        }
    }


}
