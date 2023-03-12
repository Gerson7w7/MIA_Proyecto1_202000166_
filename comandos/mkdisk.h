//
// Created by gerson on 16/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MKDISK_H
#define MIA_PROYECTO1_202000166__MKDISK_H

#include <iostream>
#include <string>
using namespace std;
void analisisMkdisk(char []);
string charToString(char* a);
string nombrePath(char path[]);
bool validacionSize(char valor[]);
string pathSName(char path[]);
bool validacionPath(string path);
bool validacionName(char name[]);
bool validacionUnit(char _unit);
void crearDiscos(char path[],char name[],char size[],char _unit);

#endif //MIA_PROYECTO1_202000166__MKDISK_H
