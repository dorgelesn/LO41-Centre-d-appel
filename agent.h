//
//  agent.h
//  LO41-Projet
//

#ifndef LO41_Projet_agent_h
#define LO41_Projet_agent_h
#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <signal.h>
//files
#include "client.h"
#include"semaphore.h"

#define NBAGENT 6
#define KEYCLI 123
#define SEMNOMFile "SemaphoreLIREfile"

#define KEYNBCLIENFILE 4321
#define KEYCLIENFILE 321
struct Agent
{

    /*------GROUPE-----*/
    //0 => technique
    //1 => commercial
    int groupe;
    /*----------------*/
    
    //pid du processus
    int numero;
    
    /*-----Langue----*/
    //0 => francais
    //1 => anglais
    //2=> français + anglais
    int langue;
    /*---------------*/
    
    int dispo;
        
};

int idShAgent;
void* addrShAgent[NBAGENT];

int shIdCliFile;
void *addrShCliFile[10];

void *addrShNbCliFile;
int shIdNbCliFile;


//Fontions
void lireAgent(int nbagent);
//int traitementClient(struct Agent *ag, struct Client *cli);
int traitementClient(int probleme,int langue,int tpsAppel,int numero, int nbagent);
void traitementClientDeFile(int nbagent);

#endif
