//
// Created by gerson on 16/02/23.
//
#include "mkdisk.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <dirent.h>
#include <cstdlib>
#include "../estructuras/mbr.h"
using namespace std;
//para convertir array de char a string
string charToString(char* a){
    string  s = a;
    return s;
}

/*mkdisk >size=3000 >unit=K >path=/home/user/Disco1.dsk
 * >size=unidad positivo
 * >u=K y M,
 * >path=/home/user/Disco1.dsk
 * */
string pathSName(char path[]){
    int contadorDia=0;//me obtiene cuantas diagonales Hay
    int contadorFind=0;
    int contador=0;
    char pathsiName[100]="";
    while(path[contador]!=NULL){
        if(path[contador]=='/'){
            contadorDia++;
        }
        contador++;
    }//aqui cuento cuatas DIAGONALES HAY
    contador=0;//se reincia contador
    while(path[contador]!=NULL){
        if(path[contador]=='/'){
            contadorFind++;
        }
        if(contadorDia!=contadorFind){

            char aux[1]="";
            aux[0]=path[contador];
            strncat(pathsiName,aux,1);
        }
        contador++;
    }
    string newname = charToString(pathsiName);
    return newname;
}

string nombrePath(char path[]){
    int contadorDia=0;//me obtiene cuantas diagonales Hay
    int contadorFind=0;
    int contador=0;
    char nuevoPath[100]="";
    char nombre[7]="Copia_";
    char valor_name[16]="";

    while(path[contador]!=NULL){
        if(path[contador]=='/'){
            contadorDia++;
        }
        contador++;
    }//aqui cuento cuatas DIAGONALES HAY
    contador=0;
    while(path[contador]!=NULL){//recorre todo el contador ejemplo /home/gerson/Escritorio/disco1.dsk
        if(path[contador]=='/'){
            contadorFind++;
        }
        if(contadorDia==contadorFind){
            //strncat(nuevoPath,nombre,7);
            if(path[contador]!='/'){
                char aux[1]="";
                aux[0]=path[contador];
                strncat(valor_name,aux,1);
            }
        }
        contador++;
    }//******************************************************************************
    string newname = charToString(valor_name);
    return newname;
}

//para validar el path:
bool validacionPath(string path){
    DIR *directorio;//puntero de un directorio
    if(directorio = opendir(path.c_str())){
        closedir(directorio);
        cout << "Directorio si Existe: "<<endl;
        return true;
    }else{
        closedir(directorio);
        cout << "Directorio NO Existe: "<<endl;
        return false;
    }

}

//para validar el size
bool validacionSize(char valor[]){
    string size_string = charToString(valor);
    int size = stoi(size_string);
    if (size>0){
        cout << "Validacion size OK "<<endl;
        return true;
    }
    cout << "warrin Validacion Size "<<endl;
    return false;

}
//validar si el nombre tiene la extenciondsk
bool validacionName(char name[]){
    int ct;
    cout<< name<<endl;
    for (int i = 0; i < 20; ++i) {
        if(name[i]!=NULL){
            if (!isalnum(name[i]) && name[i]!='.' && name[i]!='_'){
                return false;
            }
            if(name[i]=='.'){
                ct=i;
            }
        }
    }
    //validacion extension
    if(name[ct+1]=='d'){
        if (name[ct+2]=='s'){
            if (name[ct+3]=='k'){
                cout << "Extension si valida "<<endl;
                return true;
            }else{
                return false;
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool validacionUnit(char _unit){
    if(_unit=='k'){

        //cout << "U Valida"<<endl;
        return true;
    }else if(_unit=='m'){
        //cout << "U Valida"<<endl;
        return true;
        //add_valor = stoi(s_add)*1024;
    }
    //cout << "U invalida"<<endl;
    return false;
}

void crearDiscos(char path[],char name[],char size[],char _unit){
    MBR* mbr = (MBR*) malloc(sizeof (MBR));
    string s_size = charToString(size);
    string s_path = charToString(path);
    string s_name = charToString(name);
    string path_disco = s_path+s_name;
    int _size = stoi(s_size);
    int size_bytes;
    //**********************************
    if(_unit=='k'){
        size_bytes = _size*1024;
    }else if(_unit=='m'){
        size_bytes = _size*1024*1024;
    }
    cout<<"EL tamaño asignado del Disco es: "<<size_bytes<<endl;
    //se inicializa el mbr
    int start = (int)sizeof(MBR);
    mbr->mbr_dsk_signature = rand()%100; //se asigna el signature con un random
    mbr->mbr_tamano = size_bytes;//se asigna en tamaño en bytes
    time_t tiempoactual;
    tiempoactual = time(NULL);
    mbr->mbr_fecha_creacion = tiempoactual;//se asigna el timepo de creacion

    //se inicializa las particiones
    for (int i = 0; i < 4; ++i) {
        mbr->mbr_particion[i].part_status = '0';
        mbr->mbr_particion[i].part_type = '-';
        mbr->mbr_particion[i].part_start = 0;
        mbr->mbr_particion[i].part_s = 0;
    }

    //se escribe el disco
    FILE *file; //se crea el file para escribir en ello
    file = fopen(path_disco.c_str(),"w+b");// en modo escribir binario

    fseek(file,0,SEEK_SET);
    for (int i = 0; i < size_bytes; ++i) {
        fwrite("0",1,1,file);
    }
    fseek(file,0,SEEK_SET);//se posiciona al principio del disco
    fwrite(mbr,sizeof (MBR),1,file);// se escribe el mbr

    fclose(file);
    cout << "Aviso -> El disco se creo exitosamente "<<endl;
}

void analisisMkdisk(char comando[]){
    int contador=0;
    char lineacomando[200]="";
    char valor_size[20]="";
    char valor_unit='m';
    char valor_path[100]="";
    char valor_path2[100]="";
    char valor_name[16]="";//Este valor se obtiene del Path
    //argumentos opcionales
    bool flag_size = false;
    bool flag_unidad=false;
    bool flag_path=false;
    bool flag_name=false;

    while(comando[contador]!=NULL){
        if(comando[contador]==' '){
            contador++;
            memset(lineacomando,0,200);
        } else {
            char aux[1]="";
            aux[0]= tolower(comando[contador]);
            strncat(lineacomando,aux,1);
            contador++;
        }
        //validacion argumentos y valores
        if(strcmp(lineacomando,"mkdisk")==0){
            cout << "Encontro: "<<lineacomando<<endl;
            memset(lineacomando,0,200);
            contador++;
        }else if (strcmp(lineacomando,">size=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,200);

            while(comando[contador]!=NULL){
                if(comando[contador]==' ' || comando[contador]=='\n'){
                    contador++;
                    break;
                }else{
                    char aux[1]="";
                    aux[0]=comando[contador];
                    strncat(valor_size,aux,1);
                    contador++;
                }
            }
            cout << "Valor: "<<valor_size<<endl;
        }else if (strcmp(lineacomando,">path=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,200);

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
            //aqui se obtiene el nombre
            string pathcopia = nombrePath(valor_path);

            strcpy(valor_name,pathcopia.c_str());
            cout << "EL nombre encontrado: "<<valor_name<<endl;

            //path sin cnombre:
            string pathcopia2 = pathSName(valor_path);
            strcpy(valor_path2,pathcopia2.c_str());
            cout << "EL path sin name es: "<<valor_path2<<endl;

        }else if(strcmp(lineacomando,">unit=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            flag_unidad = true;
            memset(lineacomando,0,200);
            valor_unit = tolower(comando[contador]);
            contador++;
            cout << "Valor: "<<valor_unit<<endl;
        }
    }

    //>>>>>>>>>>>>>>>>>>Validaciones para crear el disco<<<<<<<<<<<<<<<<<<<<<<<<<<<
    string path = charToString(valor_path2);//valido si existe el directorio
    flag_path = validacionPath(path);//continuasion de lo de arriba

    flag_size = validacionSize(valor_size);
    flag_name = validacionName(valor_name);
    flag_unidad= validacionUnit(valor_unit);

    //para agregar la la barra
    char barra[1];
    strcpy(barra,"/");
    strncat(valor_path2,barra,1);
    if (flag_path){//existe directorio
        if(flag_unidad){
            if (flag_size){// si el size es mayoy a cero y es multiplo de 8
                if (flag_name){// si el name es válido en nombre y extension
                    // se procede a crear diso
                    crearDiscos(valor_path,valor_name,valor_size,valor_unit);
                    cout << "el disco cumple con los parametros solicitados: "<<endl;
                }else{
                    cout << "Error -> Extencion no válido "<<endl;
                }
            }else{
                cout << "Error -> Size no válido"<<endl;
            }
        }else{
            cout << "Error -> unit no válido"<<endl;
        }
    }else{//no existe directorio
        cout << "Alerta -> Directorio no existe"<<endl;
        cout << "Aviso -> Se creo directorio"<<endl;
        //Como no existe el directorio se procede a crear
        string dir = "mkdir -p \""+path+"\"";
        system(dir.c_str());
        if(flag_unidad){
            if (flag_size){// si el size es mayor a cero y es multiplo de 8
                if (flag_name){// si el name es válido en nombre y extension
                    // se procede a crear diso
                    crearDiscos(valor_path,valor_name,valor_size,valor_unit);
                    cout << "Se crea disco en nuevo directorio"<<endl;
                }else{
                    cout << "Error -> Name no válido "<<endl;
                }
            }else{
                cout << "Error -> Size no válido"<<endl;
            }
        }else{
            cout << "Error -> unit no válido"<<endl;
        }
    }
}
