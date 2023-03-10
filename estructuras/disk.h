//
// Created by gerson on 22/02/23.
//

#ifndef MIA_PROYECTO1_202000166__DISK_H
#define MIA_PROYECTO1_202000166__DISK_H

#include <iostream>
#include <ctime>
// estructuras
typedef struct EBRS
{
    char part_fit[3];
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
    char id[10];
    char formato[5];
    int sys;
} EBRS;

typedef struct PART
{
    char part_status;
    char part_type;
    char part_fit[3];
    int part_start;
    int part_size;
    char part_name[16];
    char id[10];
    char formato[5];
    int sys;
} PART;

typedef struct DISCO
{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    PART mbr_particion[4];
    EBRS ebr_logicas[24];
    char letra;
    char path[100];
    int numeral;
} DISCO;

#endif //MIA_PROYECTO1_202000166__DISK_H
