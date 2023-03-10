//
// Created by gerson on 22/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MOUNT_H
#define MIA_PROYECTO1_202000166__MOUNT_H

#include <cstdio>
#include <fstream>
#include <cstring>
#include <vector>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include "../estructuras/disk.h"
#include "../estructuras/mbr.h"
#include "../estructuras/ebr.h"
#include "../analizador/utilidades.h"

using namespace std;

bool buscarDiscoPath(char _Part[], char _Path[]);
void analisisMount(char[]);
void montajeMount(char _path[],char _name[]);
string getName(char path[]);
void showMount();

void agregarParticionMount(char _path[],char _name[],int size,char _type, char _fit[],int _start,int _next);
void elimininarParticionMount(char _path[],char _name[]);
void desmontarMount(char []);

// reportezzz
void repMbr(char _id[],char _namerep[],char _path[],char _dir[]);
bool validacionDotM(string _path);
void repDisk(char [],char [],char [],char []);

// para el mkfs
DISCO buscarDisco(char [], int sys);

#endif //MIA_PROYECTO1_202000166__MOUNT_H
