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
#include <sys/stat.h>
#include <sys/msg.h>
#include "semaphore.h"
#include <semaphore.h>
#define NBAGENT 6

#define KEYCLI 123
#define KEYAGENT 1234
#define KEYMAINPID 3443
#define KEYNBAGENT 6655
const char *SEM_NAME= "MutexCREAAGNET";

int accesSHM;

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

//    sem_close(mutex);
    sem_unlink(SEM_NAME);
    
    kill(mainPid, SIGKILL);
}

void sigCreaCli(struct Client pClient);

int main(int argc, const char * argv[])
{
    initSem(1,(char*)121,NULL);
    
    //shNbCliEnFile=0;
   // addrShNbClients=0;
 //   addrShNbAgent=0;
    
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
    
    int iAg;
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
    
   // sNbA.nbAg=0;
    //*((strucNbAg *)addrShNbAgent)=sNbA ;
    
    /*##################################################
     /*-----création de NB agents (nb processus)----*/
    
    if ((shIdPiddMainProc = shmget(KEYMAINPID, sizeof(sPP), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if((addrPidMainProc = shmat (shIdPiddMainProc,(void*)0,0))==(int *) -1)
        perror("pb shmataa");

    if(((strucPidPartage*)addrPidMainProc)->pid==getpid())
    {
        //permet de sécuriser l'accès à la mémoire partagé
        accesSHM=0;
    }

    
    for (iAg=0; iAg<NBAGENT; ++iAg)
    {
       // ++iAg;

        //if(((strucPidPartage*)addrPidMainProc)->pid==getpid())
       // {
            if(fork()==0)//on est dans le fils
            {   
                sem_wait(mutex);
                //bloque ctrl+c dans tout les processus fils
                sigblock(SIGINT);
                
                int numberPid = getpid();
                int lastDigitPid = numberPid%10;
                //fflush(NULL);
                              //if(lastDigitPid==0)
               // {
                
                P(accesSHM);
                void *addrShNbAgent2;
                int shIdNbAgent2;
                if ((shIdNbAgent2 = shmget(KEYNBAGENT, sizeof(sNbA), 0777 | IPC_CREAT)) < 0)
                    perror("shmget shidnbagent");
                if((addrShNbAgent2 = shmat (shIdNbAgent2,(void*)0,0))==(int *) -1)
                    perror("pb shmataa");

                    strucNbAg sNbA2;

                    struct Agent ag;
                    ag.groupe=0;
                    ag.langue=1;
                    ag.numero=getpid();
                
                    //printf("PID dans crea agent : %d \n ",getpid());
                  //  sNbA2 = *((strucNbAg *)addrShNbAgent);
             
                   // printf("nb dans 0 %d : %d \n",getpid(),  sNbA2.nbAg);
                    ((struct Agent *)addrShAgent)[((strucNbAg *)addrShNbAgent)->nbAg] = ag;
                

                    //sNbA2.nbAg=sNbA2.nbAg+1;
                    //BUGGGG avec ça !
               // printf(" --addrSHNbAgent :  %d-- \n", &addrShNbAgent);

  //                *((strucNbAg *)addrShNbAgent) = sNbA2;
                    fflush(NULL);
                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",((strucNbAg *)addrShNbAgent2)->nbAg,((struct Agent *)addrShAgent)[((strucNbAg *)addrShNbAgent2)->nbAg].numero);
                sNbA2.nbAg++;
                ((strucNbAg *)addrShNbAgent2)->nbAg++;
                
                //sem_close(mutex);
                //sem_unlink(SEM_NAME);
                sem_post(mutex);

                
             //   }
//                else if(lastDigitPid==1)
//                {
//                    struct Agent ag;
//                    ag.groupe=0;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 1 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                else  if(lastDigitPid==2)
//                {
//                    struct Agent ag;
//                    ag.groupe=0;
//                    ag.langue=0;
//                    ag.numero=getpid();
//                    
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 2 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//
//                }
//                else  if(lastDigitPid==3)
//                {
//                    struct Agent ag;
//                    ag.groupe=1;
//                    ag.langue=1;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 3 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                else if(lastDigitPid==4)
//                {
//                    struct Agent ag;
//                    ag.groupe=1;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 4 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                else if(lastDigitPid==5)
//                {
//                    struct Agent ag;
//                    ag.groupe=1;
//                    ag.langue=0;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 5 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//                    
//                }
//                else if(lastDigitPid==6)
//                {
//                    struct Agent ag;
//                    ag.groupe=0;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    //printf("nb dans 6 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    ((struct Agent *)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//
//                }
//                else if(lastDigitPid==7)
//                {
//                    struct Agent ag;
//                    ag.groupe=0;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 7 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                else if(lastDigitPid==8)
//                {
//                    struct Agent ag;
//                    ag.groupe=1;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 8 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    *((struct Agent **)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                else if(lastDigitPid==9)
//                {
//                    struct Agent ag;
//                    ag.groupe=1;
//                    ag.langue=2;
//                    ag.numero=getpid();
//                    strucNbAg sNbA2;
//                    printf("PID dans crea agent : %d \n ",getpid());
//                    sNbA2.nbAg = ((strucNbAg *)addrShNbAgent)->nbAg;
//                    fflush(NULL);
//                    printf("nb dans 9 %d : %d \n",getpid(),  sNbA2.nbAg);
//                    sNbA2.nbAg++;
//                    *((strucNbAg *)addrShNbAgent) = sNbA2;
//                    ((struct Agent *)addrShAgent)[sNbA2.nbAg] = ag;
//                    printf("NUMERO DANS MAIN %d !!!!!! %d \n",sNbA2.nbAg,((struct Agent **)addrShAgent)[sNbA2.nbAg]->numero);
//
//                }
//                printf("NBNBNBNB it %d \n", iAg);
                //iAg=NBAGENT+1;//permet de sortir du while une fois le fork créé
                
          //  }
            //    V(accesSHM);
                
               
                iAg+=NBAGENT+1;

        }

    }
    /*###################################################*/
    
    
    if ((shIdPiddMainProc = shmget(KEYMAINPID, sizeof(sPP), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if((addrPidMainProc = shmat (shIdPiddMainProc,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    if(((strucPidPartage*)addrPidMainProc)->pid==getpid())
    {
        //if((*addrShAgent = shmat (shIdAgent,(void *)0,0))==(int *) -1)
          //  perror("pb shmataa");
        
        printf("On est dans le main proc %d \n",((strucPidPartage*)addrPidMainProc)->pid);
        
    //    printf("LANGUE DANS MAIN lolol !!!!!! %d \n",((struct Agent **)addrShAgent)[6]->numero);
    

        
        //lireAgent();
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
        
        
        //     printf("En file d'attentes : \n ");
        //   if(shNbCliEnFile>0)
        //printf("%d \n : ",shCliEnFile[0].numero);
        
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


