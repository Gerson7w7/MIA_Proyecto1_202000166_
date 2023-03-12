#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <string>
#include "../comandos/exec.h"
#include "../comandos/mkdisk.h"
#include "../comandos/mkdir.h"
#include "../comandos/rmdisk.h"
#include "../comandos/fdisk.h"
#include "../comandos/mount.h"
#include "../comandos/unmount.h"
#include "../comandos/rep.h"
#include "../comandos/mkfs.h"
#include "../comandos/loggin.h"
#include "../comandos/mkfile.h"


using namespace std;
void Pause(){
    char res;
    printf("En espera de caracter: \n");
    cin>>res;
    cout << "Aviso -> Comando leido: "<<res<<endl;
}

void analizadorGeneral(char comando[]){
    char lineacomando[200]="";//aux para almacenar el primer comando y comparar
    int contador=0; //para iterar cada caracter

    if(comando==NULL){
        cout << "Error -> Comando vacio"<<endl;
    }else{
        bool flagName = false;//flagName = true;
        while(comando[contador]!=NULL){
            if (comando[contador]==' '){//para separar los comandos
                contador++;
                break;
            }else{//para ir agregando cada caracter de un comando y después comparar
                char aux[1]="";
                aux[0]= tolower(comando[contador]);
                strncat(lineacomando,aux,1);
                contador++;
            }

            //para comparar los comandos
            if (strcmp(lineacomando,"exec")==0){
                analisisExec(comando);
                cout << "Comando leido: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"mkdisk")==0){
                analisisMkdisk(comando);
                cout << "Comando leido: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"rmdisk")==0){
                analisisRmdisk(comando);
                cout << "Comando leido:" <<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"fdisk")==0){
                analsisFdisk(comando);
                cout << "Comando leido fd: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"mount")==0){
                analisisMount(comando);
                cout << "Comando leido mo : "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"unmount")==0){
                analisisUnmount(comando);
                cout << "Comando leido un: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"mkfs")==0){
                analisisMkfs(comando);
                cout << "Comando leido mkfs: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"mkfile")==0){
                analisisMkfile(comando);
                cout << "Comando leido mkf: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }/*else if (strcmp(lineacomando,"rm")==0){
                //falta el rm comand
                cout << "Comando leido: rm"<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }*/else if (strcmp(lineacomando,"mkdir")==0){
                analisisMkdir(comando);
                cout << "Comando leido mkdir: "<<comando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"cp")==0){
                //validar este comando
                cout << "Encontro cp: "<< lineacomando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"ls")==0){
                //falta el ls
                cout << "Encontro ls: "<< lineacomando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"pause")==0){
                //comando pause
                Pause();
                cout << "Encontro:  "<< lineacomando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"rep")==0){
                analisisRep(comando);
                cout << "Encontro:  "<< lineacomando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"login")==0){
                analisisLogin(comando);

                cout << "Encontro:  "<< lineacomando<<endl;
                flagName = true;
                memset(lineacomando,0,200);
            }else if (strcmp(lineacomando,"logout")==0){
                //analisisLogin(comando);
                cout << "Encontro:  "<< lineacomando<<endl;
                Logut();
                flagName = true;
                memset(lineacomando,0,200);
            }
        }
        if(flagName == false){
            cout << "Comando no definido : "<< lineacomando<<endl;
        }
    }
}