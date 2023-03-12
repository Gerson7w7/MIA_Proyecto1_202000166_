#include <iostream>
#include <cstring>
#include "./analizador/analizador.h"

using namespace std;
int main() {
    bool salir=false;
    char comando [100]="";

    while(!salir){
        cout << "╔═══════════════════════════════════════════╗"<<endl;
        cout << "║                 PROYECTO 1                ║"<<endl;
        cout << "║ MANEJO E IMPLEMENTACION DE ARCHIVOS       ║"<<endl;
        cout << "║ GERSON RUBEN QUIROA DEL CID 202000166     ║"<<endl;
        cout << "║ SEGUNDO SEMESTRE 2022                     ║"<<endl;
        cout << "╚═══════════════════════════════════════════╝"<<endl;
        cout << "COMANDO >> ";

        cin.getline(comando,100);

        if (strcmp(comando,"salir")==0){
            salir = true;
        }else{
            if (strcmp(comando,"")!=0){
                analizadorGeneral(comando);
            }else{
                cout << "Error -> Comando vacio"<<endl;
            }
        }

    }
    return 0;
}