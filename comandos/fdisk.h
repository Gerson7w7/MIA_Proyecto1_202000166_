//
// Created by gerson on 17/02/23.
//

#ifndef MIA_PROYECTO1_202000166__FDISK_H
#define MIA_PROYECTO1_202000166__FDISK_H

#include "../estructuras/mbr.h"
#include "../estructuras/ebr.h"
using namespace std;
bool validacionSizeParticion(char tamano[]);
bool validacionDisco(char path[]);
void analsisFdisk(char comando[]);
bool validacionFit(char fit[]);
void crearParticion(char _size[],char _unit,char _path[],char _type,char _fit[],char _name[]);
int contadorPrimaria(MBR *mbr);
int contadorExtendida(MBR *mbr);
int contadorExtendida(MBR *mbr);
bool validacionNombrePE(MBR *mbr,char name[]);
int contadorLogicas(MBR*mbr,EBR*ebr,FILE*file);
bool validacionNombreLogica(MBR*mbr,EBR*ebr,FILE*file,char _name[]);
void deleteParticion(char _name[],char _path[],char _delete[]);
void addParticion(char _add[],char _unit,char _path[],char _name[]);

#endif //MIA_PROYECTO1_202000166__FDISK_H
