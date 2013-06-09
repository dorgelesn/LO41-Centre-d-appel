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
#include "fileAttente.h"
#include"semaphore.h"

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
    
    
};

//Variables
int ressourceProc;
/*---mémoire partagé pour les client en file d'attente---*/
//Nombre de clients
int *shNbCliEnFile;
int shIdNbCliEnFile;

//Le client en lui même
int shIdCliEnFile;
struct Client *shCliEnFile;
/*------------------------------------*/

//Fontions
void lireAgent(struct Agent ag, int nbAg);
int traitementClient(struct Agent *ag, struct Client *cli);

#endif
