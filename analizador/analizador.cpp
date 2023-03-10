//
// Created by gerson on 15/02/23.
//
#include "analizador.h"

using namespace std;
void pause()
{
    char res;
    printf("Pulse un caracter para continuar: ");
    cin >> res;
    cout << "comando~> " << res << endl;
}

void analizador(char comando[])
{
    char lineacomando[200] = ""; // aux para almacenar el primer comando y comparar
    int contador = 0;            // para iterar cada caracter

    if (comando == NULL)
    {
        cout << "Error~> comando no reconocido" << endl;
    }
    else
    {
        bool esComando = false; // con este bool miramos si se reconocio el comando o no
        while (comando[contador] != NULL)
        {
            if (comando[contador] == ' ')
            { // para separar los comandos
                contador++;
                break;
            }
            else
            { // para ir agregando cada caracter de un comando y después comparar
                char aux[1] = "";
                aux[0] = tolower(comando[contador]);
                strncat(lineacomando, aux, 1);
                contador++;
            }

            // para comparar los comandos
            if (strcmp(lineacomando, "execute") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                fexec(comando);
                esComando = true;
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "mkdisk") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisMkdisk(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "rmdisk") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisRmdisk(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "fdisk") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisFdisk(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "mount") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisMount(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "unmount") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisUnmount(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "mkfs") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisMkfs(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "mkfile") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisMkfile(comando);
                memset(lineacomando, 0, 200);
            } /*else if (strcmp(lineacomando,"rm")==0){
                 //falta el rm comand
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                 esComando = true;
                 memset(lineacomando,0,200);
             }*/
            else if (strcmp(lineacomando, "mkdir") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                memset(lineacomando, 0, 200);
                analisisMkdir(comando);
            }
            else if (strcmp(lineacomando, "cp") == 0)
            {
                // validar este comando
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "ls") == 0)
            {
                // falta el ls
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "pause") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                pause();
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "rep") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisRep(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "login") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                analisisLogin(comando);
                memset(lineacomando, 0, 200);
            }
            else if (strcmp(lineacomando, "logout") == 0)
            {
                cout << comando << endl;
                cout << "sistema~> Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                logout();
                memset(lineacomando, 0, 200);
            }
            /*else if(strcmp(lineacomando,"syncronice")==0){
                //analisisSyncronice(comando);
                cout << comando << endl;
                cout << "Comando ejecutando...: " << lineacomando << endl;
                esComando = true;
                memset(lineacomando,0,200);
            }*/
        }
        if (esComando == false)
        {
            cout << "sistema~> Comando no reconocido: " << lineacomando << endl;
        }
    }
}
