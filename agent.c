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
    
    char* grp;
    char* langu;
    int i;
    
    int id;
    void* addrShAgent[6];
    
    //struct Agent ag;
    
    if ((id = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget dans agent");
    if((*addrShAgent = shmat (id,(void *)0,0))==(int *) -1)
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
       
    
    int id;
    void* addrShAgent[6];
    if ((id = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget dans agent");
    if((*addrShAgent = shmat (id,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
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
    int shIdCliFile;
    void *addrShCliFile[10];
    
    void *addrShNbCliFile;
    int shIdNbCliFile;
    
    
    
    if (( shIdCliFile= shmget(KEYCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((*addrShCliFile = shmat (shIdCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    if ((shIdNbCliFile= shmget(KEYNBCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((*addrShCliFile = shmat (shIdNbCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    structNbClientsFile nbcliensFile;
    nbcliensFile=*(structNbClientsFile*)addrShNbCliFile;
    

    struct Client *cli = NULL;
    cli=*((struct CLient **)addrShCliFile);
    cli[nbcliensFile.nb].langue=langue;
    cli[nbcliensFile.nb].numero=numero;
    cli[nbcliensFile.nb].probleme=probleme;
    cli[nbcliensFile.nb].tempsAppel=tpsAppel;
    
    //rétache l'agent
    (*(struct Client **)addrShAgent) = cli;
    nbcliensFile.nb++;
    //ratachement
    (*(structNbClientsFile *)addrShNbCliFile)=nbcliensFile;
    
    printf("actuelement en file d'attente : \n");
    int j;
    for(j=0;j<nbcliensFile.nb;++j)
    {
        printf("Client : %d ",cli[j].probleme );
    }
    
    //struct Client cli;
    
    //printf("Le client %d est dans la file d'attente \n", cli->numero);
    //shNbCliEnFile[0]++;
    //shCliEnFile[shNbCliEnFile[0]]=*cli;
    
    /*-----DETACHEMENT--------*/
    /*if(shmdt (shNbCliEnFile)==(int *) -1)
        perror("pb shmataa");
    
    if(shmdt (shCliEnFile)==(int *) -1)
        perror("pb shmataa");*/
    /*-------------------------*/
    
}