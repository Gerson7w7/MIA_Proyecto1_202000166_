//
// Created by gerson on 27/02/23.
//

#ifndef MIA_PROYECTO1_202000166__LOGGIN_H
#define MIA_PROYECTO1_202000166__LOGGIN_H

#include "../estructuras/disk.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/inodos.h"
#include "../estructuras/user.h"
#include "../estructuras/apuntador.h"
using namespace std;

void analisisLogin(char comando[]);//
void valiLogin(char _id[]);//
void recorrido(DISCO disco,char _id[],char path[]);//
string LeerArchivoCompleto(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);//
string readsimplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);//
string readdoublepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);//
string readtriplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);//
string readpointer(APUNTADORES blockeapuntador,FILE *archivo,SUPER_BLOQUE superblock);//
string SepararComas(string cadena,UsuarioActual *objetoEstructura);//
string SepararSalto(string cadena,UsuarioActual *objetoEstructura);//
string QuitarEspacios(string cadena);//
bool validacionLogin();//
void Logut();//
string getId();

#endif //MIA_PROYECTO1_202000166__LOGGIN_H
