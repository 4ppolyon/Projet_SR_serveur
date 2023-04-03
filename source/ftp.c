#include <sys/stat.h>
#include "csapp.h"

int client_down;

void ftp(int connfd) {

    off_t contenu_rest, t_bloc = 1000000; // Bloc de 1 Mo
    int nb_bloc;

    int f;
    int code_sortie;
    uint16_t net_code_sortie;
    size_t t_nomf;
    uint32_t net_t_nomf;
    struct stat stat_f;
    off_t buf_off, decal;
    uint32_t net_buf_off, net_decal;
    char *buf, path[MAXLINE];
    void *contenu, *bloc;

    while (1){ /* le fichier n'existe pas */
        strcpy(path,"./serveur_file/");

        // Récupère la taille du nom de fichier
        Rio_readn(connfd, &net_t_nomf, sizeof(size_t));
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

            // Récupère status fichier
            fstat(f,&stat_f);
            buf_off = stat_f.st_size;

            // envoie la taille du fichier
            net_buf_off = htonl(buf_off);
            Rio_writen(connfd, &net_buf_off, sizeof(off_t));

            //Décalage s'il y a déjà des element télécharger (peut être égal à 0)
            Rio_readn(connfd, &net_decal, sizeof(off_t));  
            decal = ntohl(net_decal);
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
