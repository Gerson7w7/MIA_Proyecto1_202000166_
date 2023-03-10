//
// Created by gerson on 16/02/23.
//
#include "exec.h"

using namespace std;

void fexec(char comando[])
{
    int contador = 0;
    char lineacomando[200] = "";
    char path[100] = "";

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
        { // para separar argumentos del comando exec
            contador++;
            memset(lineacomando, 0, 200);
        }
        else if (comando[contador] == '=')
        { // para separar clave y valor
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }
        else
        { // para obtener clave
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        // validaciones de la clave
        if (strcmp(lineacomando, "execute") == 0)
        {
            memset(lineacomando, 0, 200);
            contador++;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 200);
            // para obtener el valor de la clave
            while (comando[contador] != NULL)
            {
                if (comando[contador] == '"')
                { // cuando viene con comillas el path
                    contador++;
                    while (comando[contador] != NULL)
                    {
                        if (comando[contador] == '"')
                        {
                            contador++;
                            break;
                        }
                        else
                        {
                            // aquí concatenamos el path
                            char aux[1] = "";
                            aux[0] = comando[contador];
                            strncat(path, aux, 1);
                            contador++;
                        }
                    }
                }
                else
                {
                    if (comando[contador] == ' ' || comando[contador] == '\n')
                    {
                        contador++;
                        break;
                    }
                    else
                    {
                        // aquí concatenamos el path
                        char aux[1] = "";
                        aux[0] = comando[contador];
                        strncat(path, aux, 1);
                        contador++;
                    }
                }
            }
        }
    }
    // se abre y lee el archivo script
    leerArchivo(path);
}