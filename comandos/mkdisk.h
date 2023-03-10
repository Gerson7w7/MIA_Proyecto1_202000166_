//
// Created by gerson on 16/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKDISK_H
#define MIA_PROYECTO1_202000166__MKDISK_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <dirent.h>
#include <cstdlib>
#include "../estructuras/mbr.h"
#include "../analizador/utilidades.h"

using namespace std;
void analisisMkdisk(char []);
string nombrePath(char path[]);
string pathName(char path[]);
bool validacionName(char name[]);
bool validacionUnit(char _unit);
void crearDiscos(char path[],char name[],char size[],char _unit);

#endif //MIA_PROYECTO1_202000166__MKDISK_H
