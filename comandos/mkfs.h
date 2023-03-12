//
// Created by gerson on 24/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKFS_H
#define MIA_PROYECTO1_202000166__MKFS_H

#include <string>
#include "../estructuras/disk.h"
using namespace std;
void analisisMkfs(char comando[]);
void formatoMkfs(char _id[], char _type[]);
//void crearEXT3(DISCO disco,char _id[],char path[]);
void crearEXT32(DISCO disco,char _id[],char path[]);

#endif //MIA_PROYECTO1_202000166__MKFS_H
