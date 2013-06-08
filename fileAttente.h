//
//  fileAttente.h
//  LO41-Projet
//
//  Created by Ludovic Lardies on 30/05/13.
//  Copyright (c) 2013 -. All rights reserved.
//

#ifndef LO41_Projet_fileAttente_h
#define LO41_Projet_fileAttente_h
#include "client.h"

struct FileAttente
{
    struct Client *listeClient;
    int size;
};

void fileAttenteAdd(struct FileAttente file, struct Client cli);
void fileAttenteDel(struct FileAttente file, int i);
void affichageFileAttente(struct FileAttente file);

#endif
