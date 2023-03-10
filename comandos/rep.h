//
// Created by gerson on 2/03/23.
//

#ifndef MIA_PROYECTO1_202000166__REP_H
#define MIA_PROYECTO1_202000166__REP_H

#include <iostream>
#include <cstring>
#include <dirent.h>
#include <string>
#include <fstream>
#include "../comandos/mount.h"
#include "../analizador/utilidades.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/mbr.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/apuntador.h"
#include "../estructuras/journaling.h"
#include "../estructuras/inodos.h"
#include "../estructuras/bloque_carpeta.h"

using namespace std;

void analisisRep(char []);//analisis de rep
bool validacionDirectorioReporte(char _direc[]);//valido si existe directorio
bool validacionReporte(char _path[]);//valido si exite un reporte con nombre similar.
void repTree(char _id[], char _name[], char _path[],char _dir[]);
bool validacionDot(string _path);
string InodoTabla(int indice,INODOS * inode,int tree);
string BlockFolder_Tabla(int indice,BLOQUECARPETA *blockfolder,int tree);
string BlockFile_Tabla(int indice,BLOQUEARCHIVO *blockfile,int tree);
string BlockPointer_Tabla(int indice,APUNTADORES *blockpointer,int tree);
void repInodos(char _id[], char _name[],char _path[],char _dir[]);
void repBlock(char _id[], char _name[], char _path[],char _dir[]);
void repSuper(char _id[], char _name[], char _path[],char _dir[]);
void repBitmapInode(char _id[],char _name[],char _path[],char _dir[]);

#endif //MIA_PROYECTO1_202000166__REP_H
