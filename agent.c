//
//  Agent.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 25/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//
#include "agent.h"


void lireAgent(struct Agent *ag)
{
    
    char* grp;
    char* langu;
    /*----Pour affichage----*/
    if(ag->langue==0)
        langu="Français";
    else if (ag->langue==2)
        langu="Anglais";
    else
        langu="Français/Anglais";
    
    if(ag->groupe==0)
        grp="Technicien";
    else
        grp="Commercial";
    /*----Pour affichage----*/
    
    
    fflush(NULL);
    
    //printf("Operateur %d parlant %s, dans le goupe %s \n", getpid(), langu, grp);
    
    fflush(NULL);//libération buffer pour affichage
    printf("|--------------------------------------------| \n");
    printf("| Operateur %d                              \n", getpid());
    printf("| %s parant %s                \n", grp,langu);
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|============================================| \n");
    printf("\n");
    
    fflush(NULL);//libération buffer pour affichage
}

int traitementClient(struct Agent ag[6], struct Client *cli)
{
    int i;
    for(i=0; i<6; ++i)
    {
        if((ag[i].langue==2) || (ag[i].langue==1 && cli->langue==1) || (ag->langue==2 && cli->langue==2))
        {
            printf("Le client %d est traité par l'agent %d \n ", cli->numero, ag->numero);
            return 1;
        }
    }
    printf("Le client %d est dans la file d'attente \n", cli->numero);

}