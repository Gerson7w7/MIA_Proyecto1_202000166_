//
// Created by gerson on 11/03/23.
//

#include "archivo.h"

#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio>

//importaciones
#include "../analizador/analizador.h"
using namespace std;

void leerArchivo(char path[]){
    FILE * archivo;
    archivo = fopen(path,"r");//se abre el archivo en modo read
    if (archivo==NULL){// error al abrir el archivo script
        cout << "Error -> No se puede abrir el archivo"<<endl;
    }else{// se puede abrir el archivo script
        char lineacomando [200]="";
        char caracter;
        bool flag_comentario =false;

        //se procede a leer
        while(feof(archivo)==0){//mientras no sea el fin del archivo, sigue
            while((caracter = fgetc(archivo))!='\n' && feof(archivo)==0 ){
                char aux[1]="";
                aux[0]=caracter;
                strncat(lineacomando,aux,1);
            }
            if(lineacomando[0]=='#'){ //&& lineacomando[1]=='*'){
                cout << "Esto es un comentario: "<<lineacomando<<endl;
                memset(lineacomando,0,200);

            }else{
                if (lineacomando[0]!=NULL){
                    analizadorGeneral(lineacomando);
                    memset(lineacomando,0,200);
                }
            }


        }
    }
    fclose(archivo);
    return;
}