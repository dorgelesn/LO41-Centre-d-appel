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
    
    if ((shIdNbCliEnFile = shmget(KEYNBCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if((shNbCliEnFile = shmat (shIdNbCliEnFile,(void*)0,0))==(int *) -1)
        perror("pb shmat nb cli en file");
    
   
    if ((shIdCliEnFile = shmget(KEYCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if((shCliEnFile = shmat (shIdCliEnFile,(void*)0,0))==(int *) -1)
        perror("pb shmat nb cli en file");
    
    
    int i;
    //on test le client sur chaque agent
    for(i=0; i<6; ++i)
    {
        if((ag[i].langue==2) || (ag[i].langue==1 && cli->langue==1) || (ag->langue==2 && cli->langue==2))
        {
            printf("Le client %d est traité par l'agent %d \n ", cli->numero, ag->numero);
            
            /*----bloquer l'agent pour le temps d'attente...*/
            printf("Le client %d est entrain d'être traité par l'agent %d \n",cli->numero,ag[i].numero);

            P(ressourceProc);
            printf("Le client %d est entrain d'être traité par l'agent %d \n",cli->numero,ag[i].numero);
            sleep(cli->tempsAppel);
            V(ressourceProc);
            
            /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..*/
            // ...
            
            return 1;
        }
    }
    printf("Le client %d est dans la file d'attente \n", cli->numero);
    shNbCliEnFile[0]++;
    shCliEnFile[shNbCliEnFile[0]]=*cli;
    
    /*-----DETACHEMENT--------*/
    if(shmdt (shNbCliEnFile)==(int *) -1)
        perror("pb shmataa");
    
    if(shmdt (shCliEnFile)==(int *) -1)
        perror("pb shmataa");
    /*-------------------------*/
    
}