//
//  client.h
//  LO41-Projet
//

#ifndef LO41_Projet_client_h
#define LO41_Projet_client_h
#include <stdio.h>
#include <unistd.h>

struct Client
{
    /*------GROUPE-----*/
    //0 => technique
    //1 => commercial abonnement (prioritaire)
    //2 => commercial autre
    int probleme;
    /*----------------*/
    
    //pid du processus
    pid_t numero;
    
    //temps necessaire
    int tempsAppel;
    
    /*-----Langue----*/
    //0 => francais
    //1 => anglais
    int langue;
    /*---------------*/
    
    int rang;
};

//fonction pour thread
void affichageClient(struct Client *cli);


#endif
