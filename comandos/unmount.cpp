//
// Created by gerson on 24/02/23.
//
#include "unmount.h"
#include <iostream>
#include <cstring>
#include "../comandos/mount.h"
using namespace std;

void analisisUnmount(char comando[]){
    int contador=0;
    char valor_id[10]="";
    char lineacomando[100]="";
    bool flag_id=false;

    while(comando[contador]!=NULL){
        if (comando[contador] == ' ') {
            contador++;
            memset(lineacomando, 0, 100);
        } else {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        //validacion de unmount
        if(strcmp(lineacomando,"unmount")==0){
            cout << "Encontro: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            contador++;

        }else if(strcmp(lineacomando,">id=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            flag_id=true;
            //para obtener el id
            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = comando[contador];//se comenta el tolower()
                    strncat(valor_id, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: "<<valor_id<<endl;
        }
    }

    //para desmontar el id ingresado
    if(flag_id){
        cout << "Informacion -> Argumentos de unmount correcta"<<endl;
        desmontarMount(valor_id);
    }else{
        cout << "Error -> Argumentos de unmount incorrecta"<<endl;
    }
}