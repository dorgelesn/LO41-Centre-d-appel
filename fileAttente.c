//
//  fileAttente.c
//  LO41-Projet
//
//  Created by Ludovic Lardies on 30/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//

#include <stdio.h>
#include "fileAttente.h"



void fileAttenteAdd(struct FileAttente file, struct Client cli)
{
    // file = malloc(sizeof(struct Client));
    file.listeClient[file.size+1]=cli;
    file.size++;
}

void fileAttenteDel(struct FileAttente file, int i)
{
    // free(file.listeClient[i]);
    file.size--;
}
void affichageFileAttente(struct FileAttente file)
{
    int i;
    fflush(NULL);//libération buffer pour affichage
    printf("|--------------------------------------------| \n");
    
    for(i=0;i<file.size;i++)
    {
        char* grp;
        char* langu;
        /*----Pour affichage----*/
        if(file.listeClient[i].langue==0)
            langu="Français";
        else
            langu="Anglais";
        
        if(file.listeClient[i].probleme==0)
            grp="Technicien";
        else
            grp="Commercial";
        /*----Pour affichage----*/
        
        fflush(NULL);
        printf("| Client %d                              \n", getpid());
        printf("| %s parant %s                \n", grp,langu);
        printf("| Place dans la filet %d                     | \n", file.listeClient[i].rang);
        
        fflush(NULL);//libération buffer pour affichage
    }
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|                                            | \n");
    printf("|============================================| \n");
    printf("\n");
    
}
/*#######A PLACER DANS fileAttente.c#########*/
