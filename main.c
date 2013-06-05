//
//  main.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 24/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//

//#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define NBAGENT 5

//creation d'une file de message par agent


//mémoire partagé pour le nombre de client
int *shNbClients;//adresse d'atachement
int shId;
int shIdCli1;
int shIdCli2;
int shIdCli3;
int shIdCli4;


//mémoire partagé pour le nombre de client
struct Client *shCli1;//adresse d'atachement
struct Client *shCli2;//adresse d'atachement
struct Client *shCli3;//adresse d'atachement


//file de message pour stocker les clients
//struct FileAttente fileAttCli;
int mqIDcli1, cli2, cli3;
//int msgid1,msgid2,msgid3,msgid4,msgid5,msgid6;
//int msgidNbCLient;

#define mainPid getpid()

#define KEY 123



//la file d'attente va être utilisé par file de message

//files
#include "agent.h"
#include "client.h"
#include "fileAttente.h"

void fileAttenteAdd(struct FileAttente file, struct Client cli)
{
    
    // file = malloc(sizeof(struct Client));
    file.listeClient[file.size+1]=cli;
    file.size++;
    printf("size lol %d \n",file.size);
}



void supAllProc()
{
    shmctl(shId,IPC_RMID , NULL);
    shmctl(shIdCli1,IPC_RMID , NULL);
    shmctl(shIdCli2,IPC_RMID , NULL);
    shmctl(shIdCli3,IPC_RMID , NULL);
    
    
    msgctl(mqIDcli1, IPC_RMID, NULL);
    msgctl(cli2, IPC_RMID, NULL);
    msgctl(cli3, IPC_RMID, NULL);
    
    kill(mainPid, SIGKILL);
}

void sigCreaCli(struct Client pClient);

int main(int argc, const char * argv[])
{
    
    
    shNbClients=0;
    
    if ((shId = shmget(KEY, sizeof(int), 0777 | IPC_CREAT)) < 0)
	{
		perror("shmget");
	}
    
    if ((shIdCli1 = shmget(KEY, sizeof(int), 0777 | IPC_CREAT)) < 0)
    {
        perror("shmget");
    }
    
    
    if ((shIdCli2 = shmget(KEY, sizeof(int), 0777 | IPC_CREAT)) < 0)
    {
        perror("shmget");
    }
    
    
       if ((shIdCli3 = shmget(KEY, sizeof(int), 0666 | IPC_CREAT)) < 0)
	{
		perror("shmget");
	}
    
    if ((shIdCli4 = shmget(KEY, sizeof(int), 0666 | IPC_CREAT)) < 0)
	{
		perror("shmget");
	}
    
    
    int nbCli =0;
    
    
    int sema;
    struct sembuf sem[2];
    
    sem[0].sem_num=0;
    sem[0].sem_op=1;
    
    //sem[1].sem_num=1;
    //sem[1].sem_op=1;
    //   semop(sema,sem,1);
    
    //predre ressource
    //  sem[0].sem_num=0;
    // sem[0].sem_op=-1;
    //semop(sema,sem,0);
    
    /*#######################################
     # Signal de creation de client (Ctrl+C #
     ########################################*/
    signal(SIGINT,sigCreaCli);
    /*########################################*/
    
    /*#######################################
     # Signal de suppression de processus (Ctrl+Z #
     ########################################*/
    signal(SIGTSTP,supAllProc);
    /*########################################*/
    struct Agent ag;
    
    int iAg=0;
    int grpAg;
    int langAg;
    
    /*-----création de NB agents (nb processus)----*/
    while (iAg<=NBAGENT)
    {
        //Structure definissant les opérateurs
        ++iAg;
        
        if(fork()==0)//on est dans le fils
        {
            //bloque ctrl+c dans tout les processus fils
            sigblock(SIGINT);
            
            /*--Random------------*/
            //détermination groupe
            srand(time(NULL) ^ (getpid()<<16));
            grpAg=(rand() % 2) + 0;
            
            //détermination langue
            srand(time(NULL) ^ (getpid()<<16));
            langAg=(rand() % 3) + 0;
            /*--Random------------*/
            
            ag.groupe=grpAg;
            ag.langue=langAg;
            ag.numero=getpid();
            
            iAg=NBAGENT+1;//permet de sortir du while une fois le fork créée
        }
    }
    /*-------------------------------------------------------------*/
    
    
    
    /* int i;
     for(i=0; i<NBAGENT;++i)
     tabAgent[i]=ag;*+
     
     
     /* int idx=0;
     for(idx=0; idx<NBAGENT;++idx)
     waitpid(tabAgent[idx].numero,NULL,NULL);*/
    
    
    printf("pid : %d \n", getpid());
    lireAgent(&ag);
    
    
    while (1){
        pause();
    }
    
    return 0;
}


//Pour redefinir le ctrl+c du fork du signal
void sigDeSig()
{
    
    
}

void sigCreaCli(struct Client pClient)
{
    //on est dans le fils et le pid est celui du prog principal
    if((fork()==0)&&(getpid()==mainPid))
    {
        
        if((shNbClients = shmat (shId,shNbClients ,0))==(int *) -1)
        {
            perror("pb shmataa");
        }
        
        shNbClients[0]++;
        
        printf("nb client:  %d \n",shNbClients[0]);
        switch (shNbClients[0]){
            case 1:
                if((shCli1 = shmat (shIdCli1,(void *)0,0))==(int *) -1)
                {
                    perror("pb shmataa");
                }
                shCli1[0].probleme=(rand() % 3) + 0;
                shCli1[0].langue=(rand() % 2) + 0;
                shCli1[0].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli1[0].numero);
                if(shmdt (shCli1)==(int *) -1)
                    perror("pb shmat1");
                break;
            case 2:
                
                if((shCli2 = shmat (shIdCli2,shCli2,0))==(int *) -1)
                {
                    perror("pb shmat2");
                }
                shCli2[0].probleme=(rand() % 3) + 0;
                shCli2[0].langue=(rand() % 2) + 0;
                shCli2[0].numero=getpid();

                printf("Le client  : %d vient d'arriver  \n",shCli2[0].numero);

                if(shmdt (shCli2)==(int *) -1)
                {
                    perror("pb shmatii");
                }
                
                if((shCli1 = shmat (shIdCli1,shCli1,0))==(int *) -1)
                    perror("pb shmatabbbb");
                
                printf("Le client  : %d vient d'arriver  \n",shCli2[0].numero);
                
                printf("le Client précedent est :  %d \n", shCli1[0].numero);

                
                if(shmdt (shCli1)==(int *) -1)
                    perror("pb shmatii");
    
                shNbClients[0]++;
                break;
          //  case 3:
             /*   if((shCli3 = shmat (shId,shCli3,0))==(int *) -1)
                {
                    perror("pb shmataa");
                }
                shCli3[0].probleme=(rand() % 3) + 0;
                shCli3[0].langue=(rand() % 2) + 0;
                shCli3[0].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli3->numero);
                break;*/
            default:
                /*   if((shCli1 = shmat (shId,(void *)0,0))==(int *) -1)
                 {
                 perror("pb shmataa");
                 }*/
                //printf("CLI :  %d \n", shCli1[0].numero);
                printf("dans default \n");
                //affichageClient(&cliAff);

                break;
        }

        printf("dans signal ctr c \n ");
        if(shmdt (shNbClients)==(int *) -1)
        {
            perror("pb shmataa");
        }
        signal(SIGINT,sigDeSig);
        
        
    }
    
}


