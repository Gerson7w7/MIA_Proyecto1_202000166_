//
// Created by gerson on 16/02/23.
//
#include "leerArchivo.h"
using namespace std;

void leerArchivo(char path[])
{
    FILE *archivo;
    archivo = fopen(path, "r"); // se abre el archivo en modo read
    if (archivo == NULL)
    { // error al abrir el archivo
        cout << "Error~> No se puede abrir el archivo" << endl;
    }
    else
    { // se puede abrir el archivo
        char lineacomando[200] = "";
        char caracter;
        bool flag_comentario = false;

        // se procede a leer hasta el final
        while (feof(archivo) == 0)
        { // mientras no sea el fin del archivo, sigue
            while ((caracter = fgetc(archivo)) != '\n' && feof(archivo) == 0)
            {
                char aux[1] = "";
                aux[0] = caracter;
                strncat(lineacomando, aux, 1);
            }
            if (lineacomando[0] == '#')
            {
                // cout << lineacomando << endl;
                memset(lineacomando, 0, 200);
            }
            else
            {
                if (lineacomando[0] != NULL)
                {
                    analizador(lineacomando);
                    memset(lineacomando, 0, 200);
                }
            }
        }
    }
    fclose(archivo);
    return;
}