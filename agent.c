//
//  Agent.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 25/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//
#include "agent.h"

void lireAgent()
{
    
    char* grp;
    char* langu;
    int i;
    
    int id;
    void* addrShAgent[6];
    
    //struct Agent ag;
    
    if ((id = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget dans agent");
    if((*addrShAgent = shmat (id,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
   
    struct Agent *ag = NULL;
    ag=*(((struct Agent **)addrShAgent));
    
    //printf("NBAAG : %d \n ", nbAg);
    for(i=0; i<6;++i)
    {
    
    printf("dans lire agent %d  \n", i);

    /*----Pour affichage----*/
    if(ag[i].langue==0)
        langu="Français";
    else if (ag[i].langue==1)
        langu="Anglais";
    else 
        langu="Anglais/Fançais";
  //  printf("GROUPE %d \n", ((struct Agent **)addrShAgent)[i]->langue);
    
    if(ag[i].groupe==0)
       grp="Technicien";
    else
       grp="Commercial";
    /*----Pour affichage----*/
    
    fflush(NULL);
    
    //printf("Operateur %d parlant %s, dans le goupe %s \n", getpid(), langu, grp);
    
    fflush(NULL);//libération buffer pour affichage
    printf("|--------------------------------------------| \n");
    fflush(NULL);//libération buffer pour affichage
    printf("| Agent %d                              \n", ag[i].numero);
    fflush(NULL);//libération buffer pour affichage
    printf("| %s parlant %s                \n", grp,langu);
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|============================================| \n");
    printf("\n");
    
    fflush(NULL);//libération buffer pour affichage
    }
}

int traitementClient(struct Agent *ag, struct Client *cli)
{

//    printf(" test %d \n ",agtest->numero);
    
    if ((shIdNbCliEnFile = shmget(344, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if((shNbCliEnFile = shmat (shIdNbCliEnFile,(void*)0,0))==(int *) -1)
        perror("pb shmat nb cli en file");
    
   
    if ((shIdCliEnFile = shmget(KEYCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if((shCliEnFile = shmat (shIdCliEnFile,(void*)0,0))==(int *) -1)
        perror("pb shmat nb cli en file");
    
    initSem(1,1,0);
    
    int i;
    //on test le client sur chaque agent
    for(i=0; i<6; ++i)
    {
        printf("test pour agent %d, pid : %d \n", i, ag[i].numero);
        if((ag[i].langue==2&&ag[i].groupe==0)|| (ag[i].langue==1 && cli->langue==1&&ag[i].groupe==0) || (ag[i].langue==2 && cli->langue==0&&ag[i].groupe==0)
            || (ag[i].langue==2&&ag[i].groupe==1&&cli->probleme==1) || (ag[i].langue==1 && cli->langue==1&&ag[i].groupe==1&&cli->probleme==1) || (ag[i].langue==2 && (cli->langue==0)&&ag[i].groupe==1&&cli->probleme==1))
        {
            /*----bloquer l'agent pour le temps d'attente...*/
            printf("Le client %d est entrain d'être traité par l'agent %d \n",cli->numero,ag[i].numero);

            P(ressourceProc);//on prend la ressource du processus
            //met le processus en attente
            sleep(cli->tempsAppel);
            V(ressourceProc);
            
            /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..*/
            kill(cli->numero,SIGKILL);
            
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