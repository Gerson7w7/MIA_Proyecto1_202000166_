//
// Created by gerson on 24/02/23.
//

#ifndef MIA_PROYECTO1_202000166__INODOS_H
#define MIA_PROYECTO1_202000166__INODOS_H

#include <ctime>

typedef struct INODOS
{
    int i_uid;
    int i_gid;
    int i_s;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15];
    char i_type; // 1 = archivo, 0 = carpeta
    int i_perm;  // Bytes que guardan los permisos del archivo
} INODOS;

#endif //MIA_PROYECTO1_202000166__INODOS_H
