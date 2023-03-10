//
// Created by gerson on 24/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKFS_H
#define MIA_PROYECTO1_202000166__MKFS_H

#include <string>
#include <iostream>
#include <cstring>
#include <cmath>
//cargar de Estructuras:
#include "../estructuras/mbr.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/inodos.h"
#include "../estructuras/bloque_carpeta.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/journaling.h"

#include "../comandos/mount.h"
#include "../estructuras/disk.h"
#include "../analizador/utilidades.h"

using namespace std;

void analisisMkfs(char comando[]);
void formatoMkfs(char _id[], char _type[], int sys);
void crearEXT2(DISCO disco,char _id[],char path[]);
void crearEXT3(DISCO disco,char _id[],char path[]);


#endif //MIA_PROYECTO1_202000166__MKFS_H
