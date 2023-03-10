//
// Created by gerson on 16/02/23.
//

#ifndef MIA_PROYECTO1_202000166__MBR_H
#define MIA_PROYECTO1_202000166__MBR_H

#include <iostream>
#include <ctime>

typedef struct PARTICION{
    char part_status;
    char part_type;
    char part_fit[3];
    int part_start;
    int part_s;
    char part_name[16];
}PARTICION;

typedef struct MBR{
    int mbr_tamano;
    time_t  mbr_fecha_creacion;
    int mbr_dsk_signature;
    char dsk_fit;
    PARTICION mbr_particion[4];
}MBR;

#endif //MIA_PROYECTO1_202000166__MBR_H
