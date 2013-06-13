//
//  Agent.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 25/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//
#include "agent.h"

/*#####Mémoire partagé pour les client########*/

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
    
    // void* addrShAgent[6];
    
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
    
    
    //initialisation de la file d'attente
    if (( shIdCliFile= shmget(KEYCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((*addrShCliFile = shmat (shIdCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    struct Client *cli = NULL;
    cli=*((struct Client **)addrShCliFile);
    
    int j;
    for(j=0; j<10; ++j)
    {
        cli[j].rang=-1;
        cli[j].langue=-1;
        cli[j].numero=-1;
        cli[j].probleme=-1;
        cli[j].tempsAppel=-1;
        printf("j  : %d \n ", j);
        
    }
    (*(struct Client **)addrShCliFile) = cli;
    
    // shmdt(addrShCliFile);
    // shmdt(addrShNbCliFile);
}

int traitementClient(int probleme,int langue,int tpsAppel,int numero)
{
    
    if (( shIdCliFile= shmget(KEYCLIENFILE, sizeof(struct Client), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((*addrShCliFile = shmat (shIdCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    
    if ((shIdNbCliFile= shmget(KEYNBCLIENFILE, sizeof(structNbClientsFile), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((addrShNbCliFile = shmat (shIdNbCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmata addrShNbCliFile dans traitementClient");
    
    
    if ((idShAgent = shmget(1234, sizeof(struct Agent), 0777 | IPC_CREAT)) < 0)
        perror("shmget dans agent");
    if((*addrShAgent = shmat (idShAgent,(void *)0,0))==(int *) -1)
        perror("pb shmataa");
    
    
    struct Client *cli=NULL;
    struct Agent *ag = NULL;
    
    cli=*(((struct Client **)addrShCliFile));
    ag=*(((struct Agent **)addrShAgent));
    
    int i;
    
    //on test le client sur chaque agent
    
    for(i=0; i<6; ++i)
    {
        //SI la langue agent est Fr/an (2) ET agent tech(0) et agent dispo ca passe
        //SI langue agent anglais (1) ET langue du client anglais (1) ET agent tech(0) ET dispo ca passe
        //SI langue agent Francais (0) ET langue client Français (0) ET agent tech (0) ET dispo ca passe
        //SI la langue agent est Fr/an (2) ET agent com et client com (1) ET agent dispo ca passe
        //SI langue agent anglais (1) ET langue du client anglais (1) ET agent com(1) ET client com(1) ET dispo ca passe
        //SI langue agent Francais (0) ET langue client Français (0) ET agent com(1) ET client com(1) ET dispo ca passe
        if((ag[i].langue==2 && ag[i].groupe==0 && ag[i].dispo==1)
           || (ag[i].langue==1 && langue==1 && ag[i].groupe==0&&ag[i].dispo==1)
           || (ag[i].langue==0 && langue==0 && ag[i].groupe==0&&ag[i].dispo==1)
           || (ag[i].langue==2 && ag[i].groupe==1 && probleme==1&&ag[i].dispo==1)
           || (ag[i].langue==1 && langue==1&&ag[i].groupe==1&&probleme==1&&ag[i].dispo==1)
           || (ag[i].langue==0 && langue==0 &&ag[i].groupe==1&&probleme==1&&ag[i].dispo==1))
        {
            /*----bloquer l'agent pour le temps d'attente...*/
            ag[i].dispo=0;
            (*(struct Agent **)addrShAgent) = ag;
            
            printf("Le client %d est entrain d'être traité par l'agent %d pendant sec %d \n",numero,ag[i].numero,tpsAppel);
            //met le processus en attente
            sleep(tpsAppel);
            //          V(ressourceProc);
            ag[i].dispo=1;
            /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..*/
            printf("Client %d à terminé ! \n", numero);
            kill(numero,SIGKILL);
            return 1;
        }
    }
    
    printf("Le client va rentrer dans la file ! \n ");
    structNbClientsFile nbclientsFile;
    nbclientsFile=*(structNbClientsFile*)addrShNbCliFile;
    
    if(nbclientsFile.nb<10)
    {
        nbclientsFile.nb++;
        //ratachement nb clients
        (*(structNbClientsFile *)addrShNbCliFile)=nbclientsFile;
        
        //printf("nb clients en file  : %d  \n", nbclientsFile.nb);
        
        cli[nbclientsFile.nb].rang=nbclientsFile.nb;
        cli[nbclientsFile.nb].langue=langue;
        cli[nbclientsFile.nb].numero=numero;
        cli[nbclientsFile.nb].probleme=probleme;
        cli[nbclientsFile.nb].tempsAppel=tpsAppel;
        
        //ratachement clients
        printf("################ \n");
        printf("actuelement en file d'attente : \n");
        
        int j;
        for(j=0;j<nbclientsFile.nb;++j)
        {
            if(cli[j].rang!=-1)
                printf("Client : %d de rang %d \n",cli[j].numero,cli[j].rang);
        }
        printf("################ \n");

        (*(struct Client **)addrShCliFile) = cli;
    }
    else
    {
        printf("Trop de client en file d'attente, le client %d raccroche \n", numero);
        kill(numero,SIGKILL);
    }
    
    shmdt(addrShCliFile);
    shmdt(addrShNbCliFile);
    shmdt(addrShAgent);
}


void traitementClientDeFile()
{
    
    if ((shIdNbCliFile= shmget(KEYNBCLIENFILE, sizeof(structNbClientsFile), 0777 | IPC_CREAT)) < 0)
        perror("shmget shidagent ");
    
    if((addrShNbCliFile = shmat (shIdNbCliFile,(void *)0,0))==(int *) -1)
        perror("pb shmata addrShNbCliFile dans traitementClient");
    
    
    struct Agent *ag = NULL;
    struct Client *cli = NULL;
    cli=*(struct Client **)addrShCliFile;
    ag=*(struct Agent **)addrShAgent;
    
    structNbClientsFile nbclientsFile;
    nbclientsFile=*(structNbClientsFile*)addrShNbCliFile;
    
    //  shmdt(addrShCliFile);
    // shmdt(addrShNbCliFile);
    if(nbclientsFile.nb>=1)
    {

        int idx;
        int idx2;
        
        //on test pour chaque agent
        // int placeDansListe;
        
        for(idx=0; idx<6;idx++)
        {
            //ont test pour chaque client dans la file
            for(idx2=0;idx2<nbclientsFile.nb;++idx2)
            {
                
                /*######A IMPLEMENTER######*/
                //int rankMini=100;
                //int idx3;
                
                //on cherche plus petit rang
                /*for(idx3=0; idx3<10;++idx3)
                 {
                 //printf(" CLI %d \n",cli[0].rang);
                 if(cli[idx3].rang<=rankMini && cli[idx3].rang!=-1 )
                 {  // printf("debug \n");
                 
                 rankMini=cli[idx3].rang;
                 placeDansListe=idx3;
                 //  printf("place : %d \n",placeDansListe);
                 
                 }
                 }*/
                
                //printf(" FIN TRAITEMENT FILE !! idx : %d  idx2 : %d \n", idx, idx2);
                usleep(100);
                
                
                //  printf(" cli %d \n",cli[idx].numero);
                
                
                //SI la langue agent est Fr/an (2) ET agent tech(0) et agent dispo ca passe
                //SI langue agent anglais (1) ET langue du client anglais (1) ET agent tech(0) ET dispo ca passe
                //SI langue agent Francais (0) ET langue client Français (0) ET agent tech (0) ET dispo ca passe
                //SI la langue agent est Fr/an (2) ET agent com et client com (1) ET agent dispo ca passe
                //SI langue agent anglais (1) ET langue du client anglais (1) ET agent com(1) ET client com(1) ET dispo ca passe
                //SI langue agent Francais (0) ET langue client Français (0) ET agent com(1) ET client com(1) ET dispo ca passe
                
                if((ag[idx].langue==2&&ag[idx].groupe==0&&ag[idx].dispo==1 &&cli[idx2].numero!=-1&&cli[idx2].numero!=0 &&ag[idx].dispo==1)
                   || (ag[idx].langue==1 && cli[idx2].langue==1&&ag[idx].groupe==0 &&cli[idx2].numero!=-1&&cli[idx2].numero!=0&&ag[idx].dispo==1)
                   || (ag[idx].langue==0 && cli[idx2].langue==0&&ag[idx2].groupe==0&&cli[idx2].numero!=-1&&cli[idx2].numero!=0&&ag[idx].dispo==1)
                   || (ag[idx].langue==2 && ag[idx].groupe==1 && cli[idx2].probleme==1&&cli[idx2].numero!=-1&&cli[idx2].numero!=0 &&ag[idx].dispo==1)
                   || (ag[idx].langue==1 && cli[idx2].langue==1&&ag[idx].groupe==1&&cli[idx2].probleme==1&&cli[idx2].numero!=-1&&cli[idx2].numero!=0&&ag[idx].dispo==1)
                   || (ag[idx].langue==0 && cli[idx2].langue==0&&ag[idx].groupe==1&&cli[idx2].probleme==1&&cli[idx2].numero!=-1&&cli[idx2].numero!=0&&ag[idx].dispo==1))
                {
                    
                    ag[idx].dispo=0;
                    
                    printf("Le client %d qui vient de la file d'attente est entrain d'être traité par l'agent %d pendant sec %d \n",cli[idx2].numero,ag[idx].numero,cli[idx2].tempsAppel);
                    
                    (*(struct Agent **)addrShAgent) = ag;
                    
                    ag=*(((struct Agent **)addrShAgent));
                    
                    nbclientsFile.nb--;
                    *(structNbClientsFile *)addrShNbCliFile=nbclientsFile;//CETTE FONCTION PLANTE

                    //on place à -1 le client traité
                    printf("Client %d à terminé après avoir été dans la file ! \n", cli[idx2].numero);

                    int pidForKill = cli[idx2].numero;
                    cli[idx2].rang=-1;
                    cli[idx2].langue=-1;
                    cli[idx2].numero=-1;
                    cli[idx2].probleme=-1;
                    cli[idx2].tempsAppel=-1;
                    
                    *(struct Client *)addrShCliFile= *cli;
                    
                    sleep(cli[idx2].tempsAppel);
                    ag[idx].dispo=1;
                    (*(struct Agent **)addrShAgent) = ag;
                    
                    //                    /*--Tuer le processus processus client et débloquer l'agent une fois le temps passé..--*/
                    
                    
                    if(pidForKill>0)
                    {
                        printf("KILL %d ", pidForKill);
                        kill(pidForKill,SIGKILL);
                    }
                    
                }
            }
            
        }
        
    }
    
    shmdt(addrShCliFile);
    shmdt(addrShNbCliFile);
    shmdt(addrShAgent);
    
}