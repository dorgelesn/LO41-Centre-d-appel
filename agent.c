//
//  Agent.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 25/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//
#include "agent.h"

/*#####Mémoire partagé pour les client########*/
//Nombre de clients
typedef struct
{
    int nb;
}structNbClientsFile;

//Le client en lui même
void lireAgent()
{
    
    if (( shIdCliFile= shmget(KEYCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((*addrShCliFile = shmat (shIdCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
   
    char* grp;
    char* langu;
    int i;
    
    int id;
    void* addrShAgent[6];
    
    //struct Agent ag;
    
    if ((idShAgent = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget dans agent");
    if((*addrShAgent = shmat (idShAgent,(void *)0,0))==(int *) -1)
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

int traitementClient(int probleme,int langue,int tpsAppel,int numero)
{
    
    /*   if((idShAgent = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
     perror("shmget dans agent");
     if((*addrShAgent = shmat (idShAgent,(void *)0,0))==(int *) -1)
     perror("pb shmataa");*/
    
    if ((shIdNbCliFile= shmget(KEYNBCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((addrShNbCliFile = shmat (shIdNbCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");

    structNbClientsFile nbclientsFile;
    nbclientsFile=*(structNbClientsFile*)addrShNbCliFile;
    
    
    struct Agent *ag = NULL;
    
    ag=*(((struct Agent **)addrShAgent));
    
    int i;
    printf(" dans traitement \n");
    //on test le client sur chaque agent
    for(i=0; i<6; ++i)
    {
        //printf("test pour agent %d, pid : %d \n", i, ag[i].numero);
        if((ag[i].langue==2&&ag[i].groupe==0)&&ag[i].dispo==1 || (ag[i].langue==1 && langue==1&&ag[i].groupe==0)&&ag[i].dispo==1 || (ag[i].langue==2 && langue==0&&ag[i].groupe==0)&&ag[i].dispo==1
           || (ag[i].langue==2&&ag[i].groupe==1&&probleme==1)&&ag[i].dispo==1 || (ag[i].langue==1 && langue==1&&ag[i].groupe==1&&probleme==1)&&ag[i].dispo==1 || (ag[i].langue==2 && (langue==0)&&ag[i].groupe==1&&probleme==1&&ag[i].dispo==1))
        {
            /*----bloquer l'agent pour le temps d'attente...*/
            ag[i].dispo=0;
            (*(struct Agent **)addrShAgent) = ag;
            
            printf("Le client %d est entrain d'être traité par l'agent %d pendant sec %d \n",numero,ag[i].numero,tpsAppel);
            // P(ressourceProc);//on prend la ressource du processus
            //met le processus en attente
            sleep(tpsAppel);
            //          V(ressourceProc);
            
            /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..*/
            printf("Client %d à terminé ! \n", numero);
            kill(numero,SIGKILL);
            return 1;
        }
    }
    
    printf("Le client va rentrer dans la file ! \n ");
    
    if(nbclientsFile.nb<=10)
    {
        struct Client *cli = NULL;
        cli=*((struct Client **)addrShCliFile);
        printf("nb clients en file  : %d  \n", nbclientsFile.nb);
        
        cli[nbclientsFile.nb].rang=nbclientsFile.nb;
        cli[nbclientsFile.nb].langue=langue;
        cli[nbclientsFile.nb].numero=numero;
        cli[nbclientsFile.nb].probleme=probleme;
        cli[nbclientsFile.nb].tempsAppel=tpsAppel;
        
        (*(struct Client **)addrShCliFile) = cli;
        
        printf("actuelement en file d'attente : \n");
        
        int j;
        for(j=0;j<nbclientsFile.nb;++j)
        {
            printf("Client : %d \n",cli[j].numero );
        }
        //ratachement client
        nbclientsFile.nb++;
        //ratachement
        (*(structNbClientsFile *)addrShNbCliFile)=nbclientsFile;
        
    }
    else
    {
        printf("Trop de client en file d'attente, le client %d raccroche \n", numero);
        kill(numero,SIGKILL);
        
    }
    
}


void traitementClientDeFile()
{
    

    
    printf("apres \n");

    struct Agent *ag = NULL;
    ag=*(((struct Agent **)addrShAgent));
    

    struct Client *cli = NULL;
    cli=*((struct Client **)addrShCliFile);
    
     int idx;
    for(idx=0; idx<6; ++idx)
    {   int idx2;
        
      //  for(idx2=0;idx2<nbclientsFile.nb;++idx2)
        {
            //if(cli[idx2].rang==idx)
            {
//                if((ag[idx].langue==2&&ag[idx].groupe==0&&ag[idx].dispo==1 )|| (ag[idx].langue==1 && cli[idx2].langue==1&&ag[idx].groupe==0&&ag[idx].dispo==1 )|| (ag[idx].langue==2 && cli[idx2].langue==0&&ag[idx].groupe==0&&ag[idx].dispo==1)
//                   || (ag[idx].langue==2&&ag[idx].groupe==1&&cli[idx2].probleme==1&&ag[idx].dispo==1)|| (ag[idx].langue==1 && cli[idx2].langue==1&&ag[idx].groupe==1&&cli[idx2].probleme==1&&ag[idx].dispo==1)|| (ag[idx].langue==2 && cli[idx2].langue==0&&ag[idx].groupe==1&&cli[idx2].probleme==1&&ag[idx].dispo==1))
//                {
//                    ag[idx].dispo=0;
//                   
//                     (*(struct Agent **)addrShAgent) = ag;
//                    printf("Le client %d qui vient de la file d'attente est entrain d'être traité par l'agent %d pendant sec %d \n",cli[idx2].numero,ag[idx].numero,cli[idx2].tempsAppel);
//                    
//                    cli[idx2].rang=-1;
//                    cli[idx2].langue=-1;
//                    cli[idx2].numero=-1;
//                    cli[idx2].probleme=-1;
//                    cli[idx2].tempsAppel=-1;
//                    
//                    (*(struct Client **)addrShCliFile) = cli;
//                    
//                    nbclientsFile.nb--;
//                    //ratachement
//                    (*(structNbClientsFile *)addrShNbCliFile)=nbclientsFile;
//                    
//                    
//                    sleep(cli[idx2].tempsAppel);
//                    
//                    /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..*/
//                    printf("Client %d à terminé ! \n", cli[idx2].numero);
//                    ag[idx].dispo=1;
//                     (*(struct Agent **)addrShAgent) = ag;
//                    kill(cli[idx2].numero,SIGKILL);
//                }
            }
        }
    }
    
}