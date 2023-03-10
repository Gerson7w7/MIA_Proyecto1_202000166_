//
// Created by gerson on 27/02/23.
//

#ifndef MIA_PROYECTO1_202000166__LOGGIN_H
#define MIA_PROYECTO1_202000166__LOGGIN_H

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <iomanip>
#include "../estructuras/disk.h"
#include "../comandos/mount.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/inodos.h"
#include "../estructuras/user.h"
#include "../estructuras/apuntador.h"
#include "../estructuras/mbr.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/apuntador.h"

using namespace std;

string getId();
bool validacionLogin();
void logout();
string quitarEspacios(string cadena);
string separarSalto(string cadena, UsuarioActual *objetoEstructura);
string separarComas(string cadena, UsuarioActual *objetoEstructura);
string readtriplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);
string readdoublepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);
string readsimplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);
string readpointer(APUNTADORES blockeapuntador, FILE *archivo, SUPER_BLOQUE superblock);
string leerArchivoCompleto(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock);
void recorrido(DISCO disco, char _id[], char path[]);
void validarLogin(char _id[]);
void analisisLogin(char comando[]);

#endif //MIA_PROYECTO1_202000166__LOGGIN_H
