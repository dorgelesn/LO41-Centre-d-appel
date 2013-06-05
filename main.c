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


//mémoire partagé pour le nombre de client
int *shNbClients;//adresse d'atachement
int shId;
int shIdCli;



//mémoire partagé pour le nombre de client
struct Client *shCli;//adresse d'atachement


#define mainPid getpid()

#define KEY 123


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



void supAllProc()
{
    shmctl(shId,IPC_RMID , NULL);
    shmctl(shIdCli,IPC_RMID , NULL);

    
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
    
    if ((shIdCli = shmget(KEY, sizeof(int), 0777 | IPC_CREAT)) < 0)
    {
        perror("shmget");
    }
    
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
        
        int j;
        if((shNbClients = shmat (shId,shNbClients ,0))==(int *) -1)
        {
            perror("pb shmataa");
        }
        
        if((shCli = shmat (shIdCli,(void *)0,0))==(int *) -1)
        {
            perror("pb shmataa");
        }
        
        //shNbClients[0]++;
        int forSwitch = shNbClients[0]+1;
        printf("nb client:  %d \n",shNbClients[0]);
        switch (forSwitch){
            case 1:
                shCli[1].probleme=(rand() % 3) + 0;
                shCli[1].langue=(rand() % 2) + 0;
                shCli[1].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli[1].numero);
                shNbClients[0]++;
                break;
            case 2:
                shCli[2].probleme=(rand() % 3) + 0;
                shCli[2].langue=(rand() % 2) + 0;
                shCli[2].numero=getpid();
                printf("Le client  precedant est : %d \n",shCli[1].numero);
                printf("Le client  : %d vient d'arriver  \n",shCli[2].numero);
                shNbClients[0]++;
                break;
            case 3:
                shCli[3].probleme=(rand() % 3) + 0;
                shCli[3].langue=(rand() % 2) + 0;
                shCli[3].numero=getpid();
                printf("Le client  : %d vient d'arriver  \n",shCli[3].numero);
                shNbClients[0]++;
                break;

            default:
                for(j=1; j<=shNbClients[0];++j)
                    printf("Actuelement en file d'attente : %d  \n",shCli[j].numero);
                shNbClients[0]++;
                break;
        }
        

        //détachement mémoire partagée nombre de clients
        if(shmdt (shNbClients)==(int *) -1)
        {
            perror("pb shmataa");
        }
        
        //détachement mémoire partagée des clients
        if(shmdt (shCli)==(int *) -1)
        {
            perror("pb shmataa");
        }
        
        signal(SIGINT,sigDeSig);
        
        
    }
    
}


