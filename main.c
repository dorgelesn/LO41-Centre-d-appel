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

//files
#include "agent.h"
#include "client.h"
#define NBAGENT 6

#define SEMNOM "SemaphoreCreacli"

#define KEYCLI 123
#define KEYAGENT 1234
#define KEYMAINPID 3443
#define KEYNBAGENT 6655

//pour sémaphore d'accès à la mémoire partagée
int accesSHM=0;

/*####Mémoire partagé pour stocker pid du processus main##*/
void *addrPidMainProc;
int shIdPiddMainProc;
typedef struct
{
    int pid;
}strucPidPartage;
/*####FIN Mémoire partagé pour stocker pid du processus main##*/


/*#####mémoire partagé pour les agents######*/
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
/*#####FIN mémoire partagé pour les agents######*/

#define mainPid getpid()





void supAllProc()
{
    //shmctl(shIdNbCliEnFile,IPC_RMID , NULL);
    //shmctl(shIdCliEnFile,IPC_RMID , NULL);
    
    //shmctl(shIdNbCli,IPC_RMID , NULL);
    //shmctl(shIdCli,IPC_RMID , NULL);
    
    int shIdCliFile;
    if (( shIdCliFile= shmget(KEYCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    int shIdNbCliFile;
    if ((shIdNbCliFile= shmget(KEYNBCLIENFILE, sizeof(int), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    shmctl(shIdNbCliFile, IPC_RMID, NULL);
    shmctl(shIdCliFile, IPC_RMID, NULL);
    
    shmctl(shIdAgent, IPC_RMID, NULL);
    shmctl(shIdNbAgent, IPC_RMID, NULL);
    shmctl(shIdPiddMainProc, IPC_RMID, NULL);
    
    int tabSemId;
    key_t cle = ftok(SEMNOM,'0');
    (tabSemId  = semget(cle, 1, IPC_CREAT  | 0600));
    semctl(tabSemId, IPC_RMID, NULL);
    
    int tabSemId2;
    key_t cle2 = ftok(SEMNOMFile,'0');
    (tabSemId2  = semget(cle2, 1, IPC_CREAT  | 0600));
    
    semctl(tabSemId2, IPC_RMID, NULL);

    
    kill(getpid(), SIGKILL);
}

void sigCreaCli();

int main(int argc, const char * argv[])
{
    
    
    int pvInit[1];
    pvInit[0]=1;
    initSem(1,SEMNOM,pvInit);
    //int pvInit2[1];
    //pvInit2[0]=1;
    //initSem(1,SEMNOMATTCL,pvInit2);
    //P(accesApresCrea);
    
    
    //(accesApresCrea);
    
    //shNbCliEnFile=0;
    //addrShNbClients=0;
    //addrShNbAgent=0;
    
    
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
    
    
    
    int iAg;
    int grpAg;
    int langAg;
    
    if ((shIdAgent = shmget(KEYAGENT, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    if((*addrShAgent = shmat (shIdAgent,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    
    strucNbAg sNbA;
    if ((shIdNbAgent = shmget(KEYNBAGENT, sizeof(sNbA), 0777 | IPC_CREAT)) < 0)
		perror("shmget shidnbagent");
    if((addrShNbAgent = shmat (shIdNbAgent,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    
    if ((shIdPiddMainProc = shmget(KEYMAINPID, sizeof(sPP), 0777 | IPC_CREAT)) < 0)
        perror("shmget");
    
    if((addrPidMainProc = shmat (shIdPiddMainProc,(void*)0,0))==(int *) -1)
        perror("pb shmataa");
    
    //   if(((strucPidPartage*)addrPidMainProc)->pid==getpid())
    // {
    //permet de sécuriser l'accès à la mémoire partagé
    //}
    
    /*##################################################
     /*-----création de NB agents (nb processus)----*/
    for (iAg=0; iAg<NBAGENT; ++iAg)
    {
        
        if(fork()==0)//on est dans le fils
        {
            //permet de sécuriser l'accès à la mémoire partagé
            P(accesSHM);
            //bloque ctrl+c dans tout les processus fils
            sigblock(SIGINT);
            
            int numberPid = getpid();
            int lastDigitPid = numberPid%10;
            
            if(lastDigitPid==0)
            {
                
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=0;
                ag[nbagents.nbAg].langue=1;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                fflush(NULL);
                
                //pour test
                //  struct Agent *ag2 = NULL;
                //ag2=*(((struct Agent **)addrShAgent));
                //printf("NUMERO DANS MAIN %d !!!!!! %d \n",nbagents.nbAg,ag2[nbagents.nbAg].numero);
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
                
            }
            else if(lastDigitPid==1)
            {
                
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=0;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
            }
            else  if(lastDigitPid==2)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=0;
                ag[nbagents.nbAg].langue=0;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
                
            }
            else  if(lastDigitPid==3)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=1;
                ag[nbagents.nbAg].langue=1;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
            }
            else if(lastDigitPid==4)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=1;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
            }
            else if(lastDigitPid==5)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=1;
                ag[nbagents.nbAg].langue=0;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
                
            }
            else if(lastDigitPid==6)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=0;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
                
            }
            else if(lastDigitPid==7)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=0;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
            }
            else if(lastDigitPid==8)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=1;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
                
            }
            else if(lastDigitPid==9)
            {
                //recuperer la structure du nombre d'agents
                strucNbAg nbagents;
                nbagents = *(strucNbAg *)addrShNbAgent;
                
                //recuperer la structure des agents
                struct Agent *ag = NULL;
                ag=*(((struct Agent **)addrShAgent));
                ag[nbagents.nbAg].dispo=1;
                ag[nbagents.nbAg].groupe=1;
                ag[nbagents.nbAg].langue=2;
                ag[nbagents.nbAg].numero=getpid();
                
                //rétache l'agent
                (*(struct Agent **)addrShAgent) = ag;
                
                //incrémenter et rattacher le nombre d'agnts
                nbagents.nbAg++;
                (*(strucNbAg *)addrShNbAgent)=nbagents;
            }
            iAg=NBAGENT+1;//permet de sortir du while une fois le fork créé
            
            V(accesSHM);
            
        }
    }
    /*###################################################*/
    
    strucNbAg nBagents;
    nBagents=*(((strucNbAg *)addrShNbAgent));
    if(nBagents.nbAg==5)
    {
        lireAgent();
        
    }
    
    shmdt(addrShCliFile);
    shmdt(addrShNbAgent);
    shmdt(addrShNbAgent);
    
    shmdt(addrShNbCliFile);
    
    
    while (1){
        
        pause();
        traitementClientDeFile();
        
    }
    return 0;
}


//Pour redefinir le ctrl+c du fork du signal
void sigDeSig()
{
    
    
}

void sigCreaCli()
{
    //on est dans le fils et le pid est celui du prog principal
    printf("dans signal ctrl+c \n");
    if(fork()==0)
    {
        int probleme, langue, tpsAppel, numero;
        srand(time(NULL) ^ (getpid()<<16));
        probleme=(rand() % 3) + 0;
        srand(time(NULL) ^ (getpid()<<16));
        langue=(rand() % 2) + 0;
        srand(time(NULL) ^ (getpid()<<16));
        tpsAppel=(rand() % 4) + 2;
        numero=getpid();
        
        printf("Le client  : %d vient d'arriver, pb : %d, langue : %d  \n",numero, probleme, langue);
        
        if(traitementClient(probleme,langue,tpsAppel,numero)==1)
        {
            printf("Client traité correctement");
        }
        
        signal(SIGINT,sigDeSig);
        
        
    }
    
}


