//
//  client.c
//  LO41-Projet
//


#include "client.h"


void affichageClient(struct Client *cli)
{

    char* pb;
    char* langu;
    
    /*----Pour affichage----*/
    if(cli->langue==0)
        langu="Français";
    else
        langu="Anglais";
    
    if(cli->probleme==0)
        pb="technique";
    
    else if (cli->probleme==1)
        pb="commercial abonnement";
    
    else if (cli->probleme==2)
        pb="commercial autre";
    /*----Pour affichage----*/
    
    fflush(NULL);//libération buffer pour affichage
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Client %d parlant %s, avec le probléme : %s \n", getpid(), langu, pb);
    printf("temps necessaire : %d \n", cli->tempsAppel);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    
}
