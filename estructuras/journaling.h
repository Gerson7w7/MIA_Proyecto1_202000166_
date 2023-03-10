//
// Created by gerson on 24/02/23.
//

#ifndef MIA_PROYECTO1_202000166__JOURNALING_H
#define MIA_PROYECTO1_202000166__JOURNALING_H

#include <ctime>

typedef struct JOURNALING
{
    char jorunal_tipo_operacion[10];
    char journal_tipo;
    char journal_nombre[200];    // cambie el nombre pro 200
    char journal_contenido[200]; // contenido del archivo de texto
    // char journal_fecha[16];
    time_t journal_fecha;
    char journal_propietario[10];
    int journal_persimo;
    // char journal_persimo[10];

} JOURNALING;

#endif //MIA_PROYECTO1_202000166__JOURNALING_H
