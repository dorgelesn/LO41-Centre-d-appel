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
