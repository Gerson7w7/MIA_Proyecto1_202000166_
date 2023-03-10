//
// Created by gerson on 24/02/23.
//
#include "unmount.h"

void analisisUnmount(char comando[])
{
    int contador = 0;
    char _id[10] = "";
    char lineacomando[100] = "";
    bool gotId = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
        {
            contador++;
            memset(lineacomando, 0, 100);
        }
        else
        {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }
        // argumentos de unmount
        if (strcmp(lineacomando, "unmount") == 0)
        {
            cout << "sistema~> ejecutando umount... " << endl;
            memset(lineacomando, 0, 100);
            contador++;
        }
        else if (strcmp(lineacomando, ">id=") == 0)
        {
            memset(lineacomando, 0, 100);
            gotId = true;
            // para obtener el id
            while (comando[contador] != NULL)
            {
                if (comando[contador] == ' ' || comando[contador] == '\n')
                {
                    contador++;
                    break;
                }
                else
                {
                    char aux[1] = "";
                    aux[0] = comando[contador]; // se comenta el tolower()
                    strncat(_id, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> particion a desmontar con id:  " << _id << endl;
        }
    }

    // verificamos que tengamos un id
    if (!gotId)
    {
        cout << "Error~> Se necesita especificar un id para poder desmontar una particion." << endl;
        return;
    }
    // empezamos a desmontar id
    desmontarMount(_id);
}