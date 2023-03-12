//
// Created by gerson on 22/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MOUNT_H
#define MIA_PROYECTO1_202000166__MOUNT_H

#include <iostream>
#include <string>
#include "../estructuras/disk.h"
using namespace std;
void analisisMount(char[]);
bool validacionPathMount(char path[]);
void montajeMount(char _path[],char _name[]);
string charToStringM(char* a);
string getName(char path[]);
void showMount();
//metodos para cuando ya existe el path montado_+:

void agregarParticionMount(char _path[],char _name[],int size,char _type, char _fit[],int _start,int _next);
void elimininarParticionMount(char _path[],char _name[]);
void desmontarMount(char []);
//#REPORTES
void repMbr(char _id[],char _namerep[],char _path[],char _dir[]);
bool validacionDotM(string _path);
void repDisk(char [],char [],char [],char []);
// para el mkfs
DISCO buscarDisco(char []);
//para el montaje que no deje meter la misma particion
bool buscarDiscoPath(char _Part[], char _Path[]);

#endif //MIA_PROYECTO1_202000166__MOUNT_H
