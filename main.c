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
#define KEYCLI 123
#define KEYAGENT 1234

/*---mémoire partagé pour les client---*/
//Nombre de clients
int *shNbClients;
int shIdNbCli;

//Le client en lui même
int shIdCli;
struct Client *shCli;
/*------------------------------------*/

/*----mémoire partagé pour les agents----*/
//nombre d'agent
int *shNbAgent;
int shIdNbAgent;

//l'agent en lui même
struct Agent *shAgent;//adresse d'atachement
int shIdAgent;
/*------------------------------------*/

#define mainPid getpid()

//files
#include "agent.h"
#include "client.h"
#include "fileAttente.h"

void fileAttenteAdd(struct FileAttente file, struct Client cli)
{
    
    file.listeClient[file.size+1]=cli;
    file.size++;
    printf("size lol %d \n",file.size);
}

void sigUser1(int signum)
{
    if (signum==SIGUSR1)
    {
        printf("dans sigUser1 \n");
    }
}

void supAllProc()
{
    
    shmctl(shIdNbCliEnFile,IPC_RMID , NULL);
    shmctl(shIdCliEnFile,IPC_RMID , NULL);
    shmctl(shIdNbCli,IPC_RMID , NULL);
    shmctl(shIdCli,IPC_RMID , NULL);
    shmctl(shIdAgent, IPC_RMID, NULL);
    shmctl(shIdNbAgent, IPC_RMID, NULL);
    
    kill(mainPid, SIGKILL);
}

void sigCreaCli(struct Client pClient);

int main(int argc, const char * argv[])
{
    
    shNbCliEnFile=0;
    shNbClients=0;
    shNbAgent=0;
    
    if ((shIdNbCli = shmget(KEYCLI, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if ((shIdCli = shmget(KEYCLI, sizeof(int), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if ((shIdNbAgent = shmget(KEYAGENT, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if ((shIdAgent = shmget(KEYAGENT, sizeof(int), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    /*#######################################
     # Signal de creation de client (Ctrl+C #*/
    signal(SIGINT,sigCreaCli);
    /*########################################*/
    
    /*#######################################
     # Signal de suppression de processus (Ctrl+Z #*/
    signal(SIGTSTP,supAllProc);
    /*########################################*/
    
    signal(SIGUSR1,sigUser1);
    
    struct Agent ag;
    
    int iAg=0;
    int grpAg;
    int langAg;
    
    if((shNbAgent = shmat (shIdNbAgent,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    if((shAgent = shmat (shIdNbAgent,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    
    /*##################################################
     /*-----création de NB agents (nb processus)----*/
    while (iAg<NBAGENT)
    {
        ++iAg;
        
        if(fork()==0)//on est dans le fils
        {
            //bloque ctrl+c dans tout les processus fils
            sigblock(SIGINT);
            
            int numberPid = getpid();
            int lastDigitPid = numberPid%10;
            fflush(NULL);

            printf("Mon pid est : %d ma dernier digit est %d \n     ", numberPid, lastDigitPid);
            if(lastDigitPid==0)
            {
                shAgent[shNbAgent[0]].groupe=0;
                shAgent[shNbAgent[0]].langue=1;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else  if(lastDigitPid==1)
            {
                shAgent[shNbAgent[0]].groupe=0;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else  if(lastDigitPid==2)
            {
                shAgent[shNbAgent[0]].groupe=0;
                shAgent[shNbAgent[0]].langue=0;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else  if(lastDigitPid==3)
            {
                shAgent[shNbAgent[0]].groupe=1;
                shAgent[shNbAgent[0]].langue=1;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==4)
            {
                shAgent[shNbAgent[0]].groupe=1;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==5)
            {
                shAgent[shNbAgent[0]].groupe=1;
                shAgent[shNbAgent[0]].langue=0;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==6)
            {
                shAgent[shNbAgent[0]].groupe=0;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==7)
            {
                shAgent[shNbAgent[0]].groupe=1;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==8)
            {
                shAgent[shNbAgent[0]].groupe=0;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            else if(lastDigitPid==9)
            {
                shAgent[shNbAgent[0]].groupe=1;
                shAgent[shNbAgent[0]].langue=2;
                shAgent[shNbAgent[0]].numero=getpid();
            }
            
            iAg=NBAGENT+1;//permet de sortir du while une fois le fork créée
        }
    }
    /*###################################################*/

    lireAgent(shAgent[shNbAgent[0]],NBAGENT);
    
   // printf("on est dans le proc : %d \n", getpid());
    
    if(shmdt (shNbAgent)==(int *) -1)
        perror("pb shmataa");
    
    //détachement mémoire partagée des clients
    if(shmdt (shAgent)==(int *) -1)
        perror("pb shmataa");
    while (1){
        pause();
        
        if((shNbAgent = shmat (shIdNbAgent,(void*)0,0))==(int *) -1)
            perror("pb shmataa");
        
        if((shAgent = shmat (shIdNbAgent,(void *)0,0))==(int *) -1)
            perror("pb shmataa");
        
        //  printf("test \n");
        //   printf(" nb agents avant traitement %d \n",shNbAgent[0]);
        // printf(" nb clients avant traitement %d \n",shNbClients[0]);
        
        /* if(shAgent[shNbAgent[0]].langue==shCli[shNbClients[0]].langue&&shAgent[shNbAgent[0]].groupe==shCli[shNbClients[0]].probleme)
         {
         printf("Agent %d traite client %d", shAgent[0].numero, shCli[0].numero);
         }*/
        
        if(shmdt (shNbAgent)==(int *) -1)
            perror("pb shmataa");
        
        //détachement mémoire partagée des clients
        if(shmdt (shAgent)==(int *) -1)
            perror("pb shmataa");
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
        int j;
        if((shNbClients = shmat (shIdNbCli,(void*)0 ,0))==(int *) -1)
            perror("pb shmataa");
        
        if((shCli = shmat (shIdCli,(void *)0,0))==(int *) -1)
            perror("pb shmataa");
        
        //shNbClients[0]++;
        int forSwitch = shNbClients[0]+1;
        switch (forSwitch){
            case 1:
                
                shCli[1].probleme=(rand() % 3) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[1].langue=(rand() % 2) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[1].tempsAppel=(rand() % 10) + 5;
                shCli[1].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli[1].numero);
                traitementClient(shAgent, &shCli[1]);
                shNbClients[0]++;
                break;
            case 2:
                shCli[2].probleme=(rand() % 3) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[2].tempsAppel=(rand() % 10) + 5;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[2].langue=(rand() % 2) + 0;
                shCli[2].numero=getpid();
                printf("Le client  precedant est : %d \n",shCli[1].numero);
                printf("Le client  : %d vient d'arriver  \n",shCli[2].numero);
                shNbClients[0]++;
                break;
            case 3:
                shCli[3].probleme=(rand() % 3) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[3].tempsAppel=(rand() % 10) + 5;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[3].langue=(rand() % 2) + 0;
                shCli[3].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli[3].numero);
                shNbClients[0]++;
                break;
            default:
                printf("Trop de client en file d'attend, %d racroche \n",getpid());
                for(j=1; j<=shNbClients[0];++j)
                    printf("Actuelement en file d'attente : %d  \n",shCli[j].numero);
                //  shNbClients[0]++;
                break;
        }
        
        
        //détachement mémoire partagée nombre de clients
        if(shmdt (shNbClients)==(int *) -1)
            perror("pb shmataa");
        
        //détachement mémoire partagée des clients
        if(shmdt (shCli)==(int *) -1)
            perror("pb shmataa");
        
        
        signal(SIGINT,sigDeSig);
        
        
    }
    
}


