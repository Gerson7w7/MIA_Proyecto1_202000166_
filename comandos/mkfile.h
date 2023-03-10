//
// Created by gerson on 28/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKFILE_H
#define MIA_PROYECTO1_202000166__MKFILE_H

#include <string>
#include <iostream>
#include <list>
#include <cstring>
#include <ctime>
#include "../estructuras/super_bloque.h"
#include "../estructuras/disk.h"
#include "../estructuras/mbr.h"
#include "../estructuras/journaling.h"
#include "../estructuras/inodos.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/bloque_carpeta.h"
#include "../comandos/loggin.h"
#include "../comandos/mount.h"
#include "../comandos/mkdisk.h"
#include "../comandos/mkdir.h"

using namespace std;

static list<string> ListaCarpetasm;
static bool pm = false;
static int controlm = 0;
static bool omitirarchivo = false;
static string cont = "";

int returnBlockAvailable2(SUPER_BLOQUE *super, FILE *file);
string filename2(string ruta);
void linktofile(int indiceinodocarpeta, int indicebloquearchivo, string namefile, FILE *file, SUPER_BLOQUE *super);
void rewriteBmBlock(SUPER_BLOQUE *super, FILE *file, int indice, char value);
void rewriteBmInode(SUPER_BLOQUE *super, FILE *file, int indice, char value);
int createFile(FILE *file, SUPER_BLOQUE *super, string texto);
bool createRecursivom(FILE *file, SUPER_BLOQUE *auxsuper, list<string> *Lista, int *indiceactualdeinodo);
string filePath(string);
int comprobarEspacio(string);
void ejecutarMkfile(DISCO disco, char[], char[], char[]);
void analisisMkfile(char[]);

#endif //MIA_PROYECTO1_202000166__MKFILE_H
