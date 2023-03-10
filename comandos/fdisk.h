//
// Created by gerson on 17/02/23.
//

#ifndef MIA_PROYECTO1_202000166__FDISK_H
#define MIA_PROYECTO1_202000166__FDISK_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>
#include <fstream>
#include "mkdisk.h"
#include "rmdisk.h"
#include "../comandos/mount.h"
#include "../estructuras/mbr.h"
#include "../estructuras/ebr.h"

bool validacionFit(char fit[]);
int contadorPrimaria(MBR *mbr);
int contadorExtendida(MBR *mbr);
bool existeNamePart(MBR *mbr,char name[]);
int contadorLogicas(MBR*mbr,EBR*ebr,FILE*file);
bool existeNameLog(MBR*mbr,EBR*ebr,FILE*file,char _name[]);

void analisisFdisk(char comando[]);
void crearParticion(char _size[],char _unit,char _path[],char _type,char _fit[],char _name[]);
void deleteParticion(char _name[],char _path[],char _delete[]);
void addParticion(char _add[],char _unit,char _path[],char _name[]);

#endif //MIA_PROYECTO1_202000166__FDISK_H
