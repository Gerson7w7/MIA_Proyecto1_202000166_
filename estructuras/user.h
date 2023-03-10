//
// Created by gerson on 26/02/23.
//

#ifndef MIA_PROYECTO1_202000166__USER_H
#define MIA_PROYECTO1_202000166__USER_H

#include <string>
using namespace std;

// ADMINISTRACION DE USUARIOS
typedef struct UsuarioActual
{
    bool sesionIniciada;
    string usuario, password, UID, grupo, particion;
} UsuarioActual;

#endif //MIA_PROYECTO1_202000166__USER_H
