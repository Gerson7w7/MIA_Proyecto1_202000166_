#include <iostream>
#include <cstring>
#include "./analizador/analizador.h"

using namespace std;
int main() {
    bool salir=false;
    char comando [100]="";

    while(!salir){
        cout << "===============================================" <<endl;
        cout << "||                 PROYECTO 1                ||" <<endl;
        cout << "||   MANEJO E IMPLEMENTACION DE ARCHIVOS     ||" <<endl;
        cout << "|| GERSON RUBEN QUIROA DEL CID - 202000166   ||" <<endl;
        cout << "||            PRIMER SEMESTRE 2023           ||" <<endl;
        cout << "===============================================" <<endl;
        cout << "comando~> ";

        cin.getline(comando,100);

        if (strcmp(comando,"exit") == 0){
            salir = true;
        }else{
            analizador(comando);
        }

    }
    return 0;
}