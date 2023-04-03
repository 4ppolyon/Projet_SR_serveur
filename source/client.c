/*
 * echoclient.c - An echo client
 */
#include <stdio.h>
#include <string.h>
#include "csapp.h"
#include "f_client.h"

int main(int argc, char **argv){

    size_t len, t_nom_serv;
    uint32_t net_len, net_t_nom_serv;
    int code_sortie, port, code_type;
    uint16_t net_code_sortie, net_port, net_code_type;
    int clientfd;
    char *host, *nom_serv, buf[MAXLINE];

    /*
     * Note that the 'host' can be a name or an IP address.
     */
    host = "localhost";
    port = 2112;

    clientfd = Open_clientfd(host, port);
    printf("client connected to master server\n"); 

    // Envoie au serveur qu'il est un client
    code_type = 1;
    net_code_type = htons(code_type);
    Rio_writen(clientfd, &net_code_type, sizeof(uint16_t));

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
