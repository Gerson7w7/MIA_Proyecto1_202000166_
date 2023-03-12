//
// Created by gerson on 17/02/23.
//
#include "rmdisk.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <dirent.h>
#include <cstdlib>

#include "../comandos/mkdisk.h"
using namespace std;

//para validar el path del disco
bool validacionPathDisco(char path[]){
    FILE *file;
    if (file= fopen(path,"r")){
        fclose(file);
        return true;
    }
    return false;
}

void eliminarDisco(char path[]){
    string _path = charToString(path);
    string linea = "rm "+_path;
    system(linea.c_str());
    cout << "Aviso -> El disco se ha eliminado exitosamente "<<path<<endl;
}

void analisisRmdisk(char comando[]){
    int contador=0;
    char lineacomando[200]="";
    char valor_path[100]="";

    while(comando[contador]!=NULL){
        if (comando[contador]==' '){
            contador++;
            memset(lineacomando,0,200);
        } else {
            char aux[1];
            aux[0]= tolower(comando[contador]);
            strncat(lineacomando,aux,1);
            contador++;
        }

        //validacion del los argumentos de rmdisk
        if(strcmp(lineacomando,"rmdisk")==0){
            cout << "Encontro: "<<lineacomando<<endl;
            memset(lineacomando,0,200);
            contador++;
        }else if(strcmp(lineacomando,">path=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,200);
            //valor de path
            while(comando[contador]!=NULL){
                if(comando[contador]=='"'){//cuando viene con comillas el path
                    contador++;
                    while(comando[contador]!=NULL){
                        if(comando[contador]=='"'){
                            contador++;
                            break;
                        }else{
                            char aux[1]="";
                            aux[0]= comando[contador];
                            strncat(valor_path,aux,1);
                            contador++;
                        }
                    }
                }else{
                    if(comando[contador]==' ' || comando[contador]=='\n'){
                        contador++;
                        break;

                    }else{
                        char aux[1]="";
                        aux[0]= comando[contador];
                        strncat(valor_path,aux,1);
                        contador++;
                    }
                }

            }
            cout << "Valor: "<<valor_path<<endl;
        }
    }
    //validacion path disco
    bool flag_path = validacionPathDisco(valor_path);
    if (flag_path){
        char ans;
        cout << "Desea eliminar (S/N): ";
        cin >> ans;
        if (ans=='S' || ans=='s'){
            //eliminar disco
            eliminarDisco(valor_path);
            cout << "FIN DE COMANDO"<<endl;
        }
    }else{
        cout << "Error -> El path no existe para ser eliminado "<< valor_path<<endl;
    }
}