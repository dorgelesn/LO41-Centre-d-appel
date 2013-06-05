//
//  client.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 25/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//

#include "client.h"


void affichageClient(struct Client *cli)
{

    char* pb;
    char* langu;
    int pid;
    
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
    printf("Client %d parlant %s, avec le probléme : %s \n", getpid(), langu, pb);
    
}
