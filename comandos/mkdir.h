//
// Created by gerson on 28/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKDIR_H
#define MIA_PROYECTO1_202000166__MKDIR_H

#include <string>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <list>
#include "../estructuras/super_bloque.h"
#include "../estructuras/inodos.h"
#include "../estructuras/apuntador.h"
#include "../estructuras/disk.h"

using namespace std;
static list<string>ListaCarpetas;
static bool P=false;
static int control=0;
void analisisMkdir(char []);
void ejecutarMkdir(DISCO disco,char _id[],char _directorio[],char _path[]);
bool createRecursivo(FILE*file,SUPER_BLOQUE* super,list<string>*Lista,int *indiceactualnodo);
int searchFolder(INODOS *inodeFolder,FILE *file,SUPER_BLOQUE *super,string namecarpeta);
int BuscarEspacio(INODOS *inode,FILE *file,SUPER_BLOQUE *super,int *indiceactualinodo,string namecarpeta);
int searchPointerS(APUNTADORES *apuntadorsimple,FILE *file,SUPER_BLOQUE *super,int *bloquesdirectos,string namecarpeta);
int searchPointerD(APUNTADORES *apuntadordoble,FILE *file,SUPER_BLOQUE *super,int *i_apuntadorsimple,string namecarpeta);
int searchPointerT(APUNTADORES *apuntadortriple,FILE *file,SUPER_BLOQUE *super,int *i_apuntadordoble,string namecarpeta);
int buscarindirectoSimple(INODOS *inode,int indicedelinodo,FILE *file,SUPER_BLOQUE *super,string namecarpeta,int *indiceactualdeinodo);
int returnBlockAvailable(SUPER_BLOQUE *super,FILE *file);
int createFolder(FILE *file,SUPER_BLOQUE *super,int *indicepadre,string namecarpeta);
int returnBlockAvailable(SUPER_BLOQUE *super,FILE *file);



#endif //MIA_PROYECTO1_202000166__MKDIR_H
