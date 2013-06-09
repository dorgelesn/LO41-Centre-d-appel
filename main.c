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

#define NBAGENT 6

#define KEYCLI 123
#define KEYAGENT 1234
#define KEYMAINPID 3443
#define KEYNBAGENT 6655

/*---mémoire partagé pour stocker pid du processus main--*/
void *addrPidMainProc;
int shIdPiddMainProc;
typedef struct
{
	int pid;
}strucPidPartage;

/*------------------------------------------*/

/*---mémoire partagé pour les client---*/
//Nombre de clients
void *addrShNbClients; //pointeur sur l'adresse d'attachement du segment de mémoire partagée
int shIdNbCli; // identificateur


//Le client en lui même
int shIdCli;
struct Client *shCli;
/*------------------------------------*/

/*----mémoire partagé pour les agents----*/
//nombre d'agent
void *addrShNbAgent;
int shIdNbAgent;

typedef struct
{
    int nbAg;
}strucNbAg;

//l'agent en lui même
void *addrShAgent[6];//pointeur sur l'adresse d'attachement du segment de mémoire partagée
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
    shmctl(shIdPiddMainProc, IPC_RMID, NULL);

    kill(mainPid, SIGKILL);
}

void sigCreaCli(struct Client pClient);

int main(int argc, const char * argv[])
{

   /* shNbCliEnFile=0;
    addrShNbClients=0;
    addrShNbAgent=0;*/
    
    if ((shIdNbCli = shmget(KEYCLI, sizeof(int), 0777 | IPC_CREAT)) < 0)
		perror("shmget");
    
    if ((shIdCli = shmget(KEYCLI, sizeof(int), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    /*-------pour pid du processus princiaple------*/
    strucPidPartage sPP;
    if ((shIdPiddMainProc = shmget(KEYMAINPID, sizeof(sPP), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if((addrPidMainProc = shmat (shIdPiddMainProc,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    sPP.pid=getpid();
    
    //attache le pid du main à la mémoire partagé
    *((strucPidPartage*)addrPidMainProc) = sPP;
    /*----------------------------------------------*/
    
    /*#######################################
     # Signal de creation de client (Ctrl+C #*/
    signal(SIGINT,sigCreaCli);
    /*########################################*/
    
    /*#######################################
     # Signal de suppression de processus (Ctrl+Z #*/
    signal(SIGTSTP,supAllProc);
    /*########################################*/
    
    signal(SIGUSR1,sigUser1);
    
    int iAg=0;
    int grpAg;
    int langAg;
    
    strucNbAg sNbA;
    if ((shIdNbAgent = shmget(KEYNBAGENT, sizeof(sNbA), 0777 | IPC_CREAT)) < 0)
		perror("shmget shidnbagent");
    if((addrShNbAgent = shmat (shIdNbAgent,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    if ((shIdAgent = shmget(KEYAGENT, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    if((*addrShAgent = shmat (shIdAgent,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
   
    sNbA.nbAg=0;
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

            if(lastDigitPid==0)
            {
                //struct ag;
              /*  ag[((strucNbAg *)addrShNbAgent)->nbAg].groupe=0;
                ag[((strucNbAg *)addrShNbAgent)->nbAg].langue=1;
                ag[0].numero=getpid();
                
                //atache à la mémoire partagé
               	*((struct Agent *)addrShAgent) = *ag;
                sNbA.nbAg++;
                //attache le nombre de d'agent
                *((strucNbAg *)addrShNbAgent) = sNbA;*/
                
            }
            else  if(lastDigitPid==1)
            {    
                struct Agent ag;

                printf("PID dans crea cli : %d \n ",getpid());
          //      ag[((strucNbAg *)addrShNbAgent)->nbAg].groupe=0;
            //    ag[((strucNbAg *)addrShNbAgent)->nbAg].langue=1;
                ag.numero=getpid();
                
                //atache à la mémoire partagé
                
               	*((struct Agent **)addrShAgent)[0] = ag;
            
                int shIdAgent2;
                void *addrShAgent2[6];//pointeur sur l'adresse d'attachement du segment de mémoire partagée

                
                if ((shIdAgent2 = shmget(KEYAGENT, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
                    perror("shmget shidnbagent");
                
                if((*addrShAgent2 = shmat (shIdAgent2,(void *)0,0))==(int *) -1)
                    perror("pb shmataa");
                
                printf("test dans crea age 1 %d \n ",((struct Agent **)addrShAgent)[0]->numero);
                printf("test dans crea age 2 %d \n ",((struct Agent **)addrShAgent2)[0]->numero);
                
                sNbA.nbAg++;
                //attache le nombre de d'agent
                *((strucNbAg *)addrShNbAgent) = sNbA;
            }
          /*  else  if(lastDigitPid==2)
            {    struct Agent *ag;

                ag[((strucNbAg *)addrShNbAgent)->nbAg].groupe=0;
                ag[((strucNbAg *)addrShNbAgent)->nbAg].langue=1;
                ag[((strucNbAg *)addrShNbAgent)->nbAg].numero=getpid();
                
                //atache à la mémoire partagé
               	*((struct Agent *)addrShAgent) = *ag;
                sNbA.nbAg++;
                //attache le nombre de d'agent
                *((strucNbAg *)addrShNbAgent) = sNbA;
                
            }*/
         /*   else  if(lastDigitPid==3)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;;
                
            }
            else if(lastDigitPid==4)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
               
            }
            else if(lastDigitPid==5)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
                
            }
            else if(lastDigitPid==6)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
            }
            else if(lastDigitPid==7)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
            }
            else if(lastDigitPid==8)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
              
            }
            else if(lastDigitPid==9)
            {
                struct ag;
                ag.groupe=0;
                ag.langue=1;
                ag.numero=getpid();
               	*((struct Agent *)addrShAgent) = ag;
            }*/
            
            iAg=NBAGENT+1;//permet de sortir du while une fois le fork créé            
        }
    }
    /*###################################################*/


    if ((shIdPiddMainProc = shmget(KEYMAINPID, sizeof(sPP), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if((addrPidMainProc = shmat (shIdPiddMainProc,(void*)0,0))==(int *) -1)
        perror("pb shmataa");

    if(((strucPidPartage*)addrPidMainProc)->pid==getpid())
    {
        printf("On est dans le main proc %d \n",((strucPidPartage*)addrPidMainProc)->pid);
        lireAgent2(shIdAgent);
    }

     while (1){
        pause();

      /*  if((shNbAgent = shmat (shIdNbAgent,(void*)0,0))==(int *) -1)
            perror("pb shmataa");
        
        if((shAgent = shmat (shIdNbAgent,(void *)0,0))==(int *) -1)
            perror("pb shmataa");
        
        if ((shIdNbCliEnFile = shmget(KEYNBCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
            perror("shmget");
        
        if((shNbCliEnFile = shmat (shIdNbCliEnFile,(void*)0,0))==(int *) -1)
            perror("pb shmat nb cli en file");
       
        if ((shIdCliEnFile = shmget(KEYCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
            perror("shmget");
        
        if((shCliEnFile = shmat (shIdCliEnFile,(void*)0,0))==(int *) -1)
            perror("pb shmat nb cli en file");*/
        
        
        printf("En file d'attentes : \n ");
        if(shNbCliEnFile>0)
            printf("%d \n : ",shCliEnFile[0].numero);

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
        if((addrShNbClients = shmat (shIdNbCli,(void*)0 ,0))==(int *) -1)
            perror("pb shmataa");
        
        if((shCli = shmat (shIdCli,(void *)0,0))==(int *) -1)
            perror("pb shmataa");
        
        //shNbClients[0]++;
    /*    int forSwitch = addrShNbClients[0]+1;
        switch (forSwitch){
            case 1:
                shCli[1].probleme=(rand() % 3) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[1].langue=(rand() % 2) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[1].tempsAppel=(rand() % 10) + 5;
                shCli[1].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli[1].numero);
           //     printf("shAgent : %d \n", shAgent[shNbAgent[0]].numero);
                traitementClient(&shAgent[1], &shCli[1]);
                shNbClients[0]++;
                break;
            case 2:
                shCli[2].probleme=(rand() % 3) + 0;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[2].tempsAppel=(rand() % 10) + 5;
                srand(time(NULL) ^ (getpid()<<16));
                shCli[2].langue=(rand() % 2) + 0;
                shCli[2].numero=getpid();
                traitementClient(shAgent, &shCli[1]);
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
                traitementClient(shAgent, &shCli[1]);
                printf("Le client  : %d vient d'arriver  \n",shCli[3].numero);
                shNbClients[0]++;
                break;
            default:
                printf("Trop de client en file d'attend, %d racroche \n",getpid());
                //for(j=1; j<=shNbClients[0];++j)
               //     printf("Actuelement en file d'attente : %d  \n",shCli[j].numero);
                //  shNbClients[0]++;
                break;*/
      //  }
        
        
        //détachement mémoire partagée nombre de clients
       // if(shmdt (shNbClients)==(int *) -1)
         //   perror("pb shmataa");
        
        //détachement mémoire partagée des clients
        //if(shmdt (shCli)==(int *) -1)
          //  perror("pb shmataa");
        
        
        signal(SIGINT,sigDeSig);
        
        
    }
    
}


