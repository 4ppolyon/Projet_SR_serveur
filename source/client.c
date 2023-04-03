/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

#define NOM_HOST "localhost"

int main(int argc, char **argv){

    size_t len, t_nom_serv;
    uint32_t net_len, net_t_nom_serv;
    int code_sortie, port;
    uint16_t net_code_sortie, net_port;
    int clientfd;
    char *host, *nom_serv, buf[300];

    /* Note that the 'host' can be a name or an IP address.*/
    host = NOM_HOST;
    port = 2112;

    clientfd = Open_clientfd(host, port);
    
    /* At this stage, the connection is established between the client */
    printf("client connected to master server\n"); 

    // Récuperation du nom de serveur esclave
    Rio_readn(clientfd, &net_t_nom_serv, sizeof(uint32_t));
    t_nom_serv = ntohl(net_t_nom_serv);
    nom_serv = Calloc(t_nom_serv, sizeof(char)*t_nom_serv);
    Rio_readn(clientfd, nom_serv, t_nom_serv);
    
    // Récuperation du numero de port
    Rio_readn(clientfd, &net_port, sizeof(uint16_t));
    port = ntohs(net_port);

    // Fermeture de connexion avec maitre
    Close(clientfd);


    // Connexion au serveur esclave
    clientfd = Open_clientfd(nom_serv, port);
    printf("client connected to slave server\n"); 

    while (Fgets(buf, sizeof(buf), stdin) != NULL) {
        // Trouve la longueur de la chaîne jusqu'au premier '\n' puis le remplace par un caractère nul 
        len = strcspn(buf, "\n");
        buf[len] = '\0';

        // Envoie la taille du nom de fichier puis le nom de fichier
        net_len = htonl(len);
        Rio_writen(clientfd, &net_len, sizeof(size_t));
        Rio_writen(clientfd, buf, len);

        if (Rio_readn(clientfd, &net_code_sortie, sizeof(uint16_t)) > 0) {
            code_sortie = ntohs(net_code_sortie);
            if (gest_erreur(code_sortie) == 0){
                recuperation_fichier(clientfd, buf);
            }

        } else { /* the server has prematurely closed the connection */
            break;
        }
    }
    Close(clientfd);
    exit(0);
}
