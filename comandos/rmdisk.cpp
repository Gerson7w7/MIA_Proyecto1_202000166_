//
// Created by gerson on 17/02/23.
//
#include "rmdisk.h"

using namespace std;

// función para eliminar el disco indicado
void eliminarDisco(char path[])
{
    string pathSTR = charToString(path);
    string linea = "rm " + pathSTR;
    system(linea.c_str());
    cout << "sistema~> El disco se ha eliminado exito." << endl;
}

void analisisRmdisk(char comando[])
{
    int contador = 0;
    char lineacomando[200] = "";
    char _path[100] = "";

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
        {   // aquí separamos los comandos
            contador++;
            memset(lineacomando, 0, 200);
        }
        else
        {
            char aux[1];
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        // validacion del las claves de rmdisk
        if (strcmp(lineacomando, "rmdisk") == 0)
        {
            cout << "sistema~> Esta acción no se podra deshacer, asegurese de tener una copia del disco." << endl;
            memset(lineacomando, 0, 200);
            contador++;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 200);
            // valor de path
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
                            char aux[1] = "";
                            aux[0] = comando[contador];
                            strncat(_path, aux, 1);
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
                        char aux[1] = "";
                        aux[0] = comando[contador];
                        strncat(_path, aux, 1);
                        contador++;
                    }
                }
            }
            cout << "sistema~> el disco a eliminar es el siguiente: " << _path << endl;
        }
    }
    // validacion path disco
    bool gotPath = existeArchivo(_path);

    if (!gotPath)
    {
        cout << "Error~> El disco a eliminar no existe, o la ruta es equivocada: " << _path << endl;
    }

    char confirmacion;
    cout << "sistema~> ¿Esta seguro de querer eliminar el disco? Esta accion no se podra deshacer (Y/n): ";
    cin >> confirmacion;
    if (tolower(confirmacion) == 'y')
    {
        // procedemos a eliminar el disco
        eliminarDisco(_path);
    }
}