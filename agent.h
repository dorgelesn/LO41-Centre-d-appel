//
//  agent.h
//  LO41-Projet
//
//  Created by Ludovic Lardies on 30/05/13.
//  Copyright (c) 2013 -. All rights reserved.
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
void* addrShAgent[6];

int shIdCliFile;
void *addrShCliFile[10];

void *addrShNbCliFile;
int shIdNbCliFile;

static int accesShmFile=0;

struct Client *cli=NULL;
struct Agent *ag = NULL;

//Fontions
void lireAgent();
//int traitementClient(struct Agent *ag, struct Client *cli);
int traitementClient(int probleme,int langue,int tpsAppel,int numero);
void traitementClientDeFile();

#endif
