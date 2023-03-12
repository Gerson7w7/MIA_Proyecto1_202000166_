//
// Created by gerson on 28/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKFILE_H
#define MIA_PROYECTO1_202000166__MKFILE_H

#include <string>
#include <iostream>
#include <list>
#include "../estructuras/super_bloque.h"
#include "../estructuras/disk.h"

using namespace std;
static list<string>ListaCarpetasm;
static bool Pm=false;
static int controlm=0;
static bool omitirarchivo =false;
static string Cont="";

int createFile(FILE *file,SUPER_BLOQUE *super,string texto);
void rewriteBmInode(SUPER_BLOQUE* super,FILE *file,int indice, char value);
void rewriteBmBlock(SUPER_BLOQUE* super,FILE *file,int indice,char value);
void Linktofile(int indiceinodocarpeta,int indicebloquearchivo,string namefile,FILE *file,SUPER_BLOQUE *super);
string filename2(string ruta);


void analisisMkfile(char []);
void ejecutarMkfile(DISCO *disco,char [],char [],char []);
string filePath(string);
int comprobarEspacio(string);
bool createRecursivom(FILE *file,SUPER_BLOQUE *auxsuper,list<string>*Lista,int *indiceactualdeinodo);
int returnBlockAvailable2(SUPER_BLOQUE *super,FILE *file);

#endif //MIA_PROYECTO1_202000166__MKFILE_H
