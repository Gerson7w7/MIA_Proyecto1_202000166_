//
// Created by gerson on 22/02/23.
//
#include "mount.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <iomanip>

#include "../estructuras/disk.h"
#include "../estructuras/mbr.h"
#include "../estructuras/ebr.h"

using namespace std;
DISCO disco[99];

char abdecedario[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int contador_abc=0;
int contador_disco=0;
vector <string> nodos;

//#----------------------------------------Metodo para el mkfs:
//para mkfs
DISCO buscarDisco(char _id[]){
    DISCO nulo;
    for (int i = 0; i < 99; ++i) {
        if(disco[i].mbr_tamano!=0){
            for (int j = 0; j < 4; ++j) {
                if(strcmp(disco[i].mbr_particion[j].id,_id)==0){
                    strcpy(disco[i].mbr_particion[j].formato,"full");
                    disco[i].formato = 3;
                    return disco[i];
                    break;
                }

            }
        }
    }
    return nulo;
}
bool buscarDiscoPath(char _Particion[], char _Path[]){

    for (int i = 0; i < 99; ++i) {
        if(disco[i].mbr_tamano!=0){
            for (int j = 0; j < 4; ++j) {
                if((strcmp(disco[i].mbr_particion[j].part_name,_Particion)==0) && (strcmp(disco[i].path,_Path)==0) && (strcmp(disco[i].mbr_particion[j].id,"")!=0)){
                    cout << disco[i].mbr_particion[j].id<<endl;
                    return true;
                }

            }
        }
    }
    return false;
}


//#----------------------------------Metodos en el fdisk
//metodo de monteje cuando existan cambios de Disco:
void agregarParticionMount(char _path[],char _name[],int _size,char _type, char _fit[],int _start,int _next){
    for (int i = 0; i < 99; ++i) {//primero buscar el path o sea disco
        if(strcmp(disco[i].path,_path)==0){
            //que tipo de particion es
            if(_type=='p'){
                //busca espacio para p
                for (int j = 0; j < 4; ++j) {
                    if(disco[i].mbr_particion[j].part_type=='\000'){
                        //se copia
                        disco[i].mbr_particion[j].part_type = _type;
                        disco[i].mbr_particion[j].part_status = '1';
                        disco[i].mbr_particion[j].part_size = _size;
                        disco[i].mbr_particion[j].part_start = _start;
                        strcpy(disco[i].mbr_particion[j].part_name,_name);
                        strcpy(disco[i].mbr_particion[j].part_fit,_fit);
                        break;
                    }
                }
            }else if(_type=='e'){
                //busca espacio para e
                for (int j = 0; j < 4; ++j) {
                    if(disco[i].mbr_particion[j].part_type=='\000'){
                        //se copia
                        disco[i].mbr_particion[j].part_type = _type;
                        disco[i].mbr_particion[j].part_status = '1';
                        disco[i].mbr_particion[j].part_size = _size;
                        disco[i].mbr_particion[j].part_start = _start;
                        strcpy(disco[i].mbr_particion[j].part_name,_name);
                        strcpy(disco[i].mbr_particion[j].part_fit,_fit);
                        break;
                    }
                }
            }else if(_type=='l'){
                //busco espacio para l
                for (int j = 0; j < 24; ++j) {
                    if(disco[i].ebr_logicas[j].part_size!=0){
                        disco[i].ebr_logicas[j].part_size = _size;
                        disco[i].ebr_logicas[j].part_start = _start;
                        disco[i].ebr_logicas[j].part_next = _next;
                        strcpy(disco[i].ebr_logicas[j].part_fit,_fit);
                        strcpy(disco[i].ebr_logicas[j].part_name,_name);
                        break;
                    }
                }
            }
        }//si no existe el path montado hya no hay clavo.
    }//fuera del for
}

//metodo cuando se elimina una Particion y borrarla de aqui de Disco:
//para eliminar
void elimininarParticionMount(char _path[],char _name[]){
    //busco en disco
    for (int i = 0; i < 99; ++i) {
        if(strcmp(disco[i].path,_path)==0){
            //busco en las 4 basica
            for (int j = 0; j < 4; ++j) {
                if(strcmp(disco[i].mbr_particion[j].part_name,_name)==0){
                    disco[i].mbr_particion[j].part_type = '\000';
                    disco[i].mbr_particion[j].part_size = 0;
                    disco[i].mbr_particion[j].part_start = 0;
                    memset(disco[i].mbr_particion[j].part_fit,0,3);
                    memset(disco[i].mbr_particion[j].part_name,0,16);
                    memset(disco[i].mbr_particion[j].id,0,10);
                    break;
                }
                //busco en extendida
                if(disco[i].mbr_particion[j].part_type=='e'){
                    //buscao en las logicas
                    for (int k = 0; k < 24; ++k) {
                        if(strcmp(disco[i].ebr_logicas[k].part_name,_name)==0){
                            disco[i].ebr_logicas[k].part_size  = 0;
                            disco[i].ebr_logicas[k].part_next = 0;
                            disco[i].ebr_logicas[k].part_start = 0;
                            memset(disco[i].ebr_logicas[k].part_fit,0,3);
                            memset(disco[i].ebr_logicas[k].part_name,0,16);
                            memset(disco[i].ebr_logicas[k].id,0,10);
                            break;
                        }
                    }
                }
            }
        }
    }
}

//para desmontar del comando unmoun
void desmontarMount(char _id[]){
    bool flag_unmount=false;
    for (int i = 0; i < 99; ++i) {
        if(disco[i].mbr_tamano!=0){//si existo disco
            for (int j = 0; j < 4; ++j) {
                if(strcmp(disco[i].mbr_particion[j].id,_id)==0){// si está en las 4 basica
                    //se desmonta
                    memset(disco[i].mbr_particion[j].id,0,10);
                    cout << "Aviso -> Se ha desmontado particion con id "<<_id<<endl;
                    flag_unmount=true;
                    break;
                }
                if(disco[i].mbr_particion[j].part_type=='e'){//si es extendida y nombre no es igual se busca en logicas
                    for (int k = 0; k < 24; ++k) {
                        if(strcmp(disco[i].ebr_logicas[k].id,_id)==0){
                            memset(disco[i].ebr_logicas[k].id,0,10);
                            cout << "Aviso -> Se ha desmontado particion con id "<<_id<<endl;
                            flag_unmount=true;
                            break;
                        }
                    }
                }
            }

        }
    }

    //para validar error
    if(flag_unmount==false){
        cout << "Error -> No se ha encontrado particion con id "<<_id<<endl;
    }
}
//----------------------------------------------------------Metodos de montaje Normales------------

string charToStringM(char* a){
    string  s = a;
    return s;
}

//para validar existe disco
bool validacionPathMount(char path[]){
    FILE *file;
    if(file = fopen(path,"r")){
        fclose(file);
        return true;
    }
    return false;
}

//encontrar el nombre
string getName(char path[]){
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
        if(contadorDia==contadorFind){
            if(path[contador]!='/' and path[contador]!='.'){
                char aux[1]="";
                aux[0]=path[contador];
                strncat(pathsiName,aux,1);
            } else if(path[contador]=='.'){

                string newname = charToStringM(pathsiName);
                return newname;
            }
            //cout << "char leido: "<<path[contador]<<endl;
        }
        contador++;
    }
    string newname = charToStringM(pathsiName);
    return newname;
}

//montaje
void montajeMount(char _path[],char _name[]){
    //variables
    char tipoParticion = ' '; //validamos el tipo de particion
    bool existe_particion = false;
    bool existe_disk=false;
    char id_generado[50]="";//id que se genera
    FILE*file;
    file = fopen(_path,"rb+");
    MBR *mbr = (MBR*) malloc(sizeof(MBR));
    EBR *ebread =(EBR*) malloc(sizeof(EBR));
    fseek(file,0,SEEK_SET);
    fread(mbr,sizeof (MBR),1,file);

    //para validar particion
    int cont_Part=0;
    int posSig=0;
    int posStart=0;

    //buscar en particiones primarias en el mbr escrito en disco
    //****************************************************************************************
    for (cont_Part = 0; cont_Part < 4; ++cont_Part) {
        if(strcmp(mbr->mbr_particion[cont_Part].part_name,_name)==0){//se busca el nombre de la particion a montar
            if(mbr->mbr_particion[cont_Part].part_type=='p'){
                tipoParticion='p';
                existe_particion=true;
                break;
            }else if(mbr->mbr_particion[cont_Part].part_type=='e'){
                tipoParticion='e';
                existe_particion=true;
                break;
            }
        }
        //para buscar en las logicas
        if(mbr->mbr_particion[cont_Part].part_type=='e'){
            //se busca en el ebr escrito en el disco
            int pos = mbr->mbr_particion[cont_Part].part_start +sizeof(EBR)+1;
            fseek(file,pos,SEEK_SET);
            fread(ebread,sizeof(EBR),1,file);
            posSig = ebread->part_next;
            while(posSig!=-1){
                if(strcmp(ebread->part_name,_name)==0){
                    existe_particion=true;
                    posStart = ebread->part_start;
                    tipoParticion='l';
                    break;
                }
                fseek(file,posSig,SEEK_SET);
                fread(ebread,sizeof (EBR),1,file);
                posSig = ebread->part_next;
            }
        }
    }

    //****************************************************************************************
    //para validar path
    int cont_disk=0;
    string ruta = charToStringM(_path);
    for (cont_disk = 0; cont_disk < 99; ++cont_disk) { //aqui valido si ya se ha registrado el path, osea el disco
        if(strcmp(disco[cont_disk].path,ruta.c_str())==0){
            existe_disk = true;
            break;
        }
    }
    if(existe_particion){//si existe nombre particion
        if(existe_disk){//ya se ha ingresado una particion del mismo disco
            // esto es si el disco ya esta en el arreglo
            if(tipoParticion=='p'){//el path ya existe y es tipo primaria
                //busco el nombre en el disco
                //Esto es para obtener el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;

                for (int i = 0; i < 4; ++i) {
                    if(strcmp(disco[cont_disk].mbr_particion[i].part_name,_name)==0){
                        //solo se le genera el id
                        id_generado[0]='6';
                        id_generado[1]='6';

                        //into to string
                        disco[cont_disk].numeral++;
                        string s_num = to_string(disco[cont_disk].numeral);
                        char const *num_char = s_num.c_str();
                        strncat(id_generado,num_char,1);//aqui le agrego el numero de disco

                        //aqui le meto el nombre del Disco
                        int contadorP=0;
                        while(valor_nameE[contadorP]!=NULL){
                            char aux[1]="";
                            aux[0]= valor_nameE[contadorP];
                            strncat(id_generado,aux,1);
                            contadorP++;
                        }
                        //aqui le meto de una el ID
                        strcpy(disco[cont_disk].mbr_particion[cont_Part].id,id_generado);
                        cout << "Aviso -> ID generado:  "<<disco[cont_disk].mbr_particion[cont_Part].id<<endl;
                        memset(id_generado,0,50);
                        cout << "Aviso -> Particiona primaria ha sido montada "<<_name<<endl;
                        break;
                    }
                }
                fclose(file);
            }
            else if(tipoParticion=='e'){
                //Esto es para obtener el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;
                char letra = disco[cont_disk].letra;
                //se copia
                disco[cont_disk].mbr_particion[cont_Part].part_status = mbr->mbr_particion[cont_Part].part_status;
                disco[cont_disk].mbr_particion[cont_Part].part_type = mbr->mbr_particion[cont_Part].part_type;
                disco[cont_disk].mbr_particion[cont_Part].part_size = mbr->mbr_particion[cont_Part].part_s;
                disco[cont_disk].mbr_particion[cont_Part].part_start = mbr->mbr_particion[cont_Part].part_start;
                strcpy(disco[cont_disk].mbr_particion[cont_Part].part_name,mbr->mbr_particion[cont_Part].part_name);
                strcpy(disco[cont_disk].mbr_particion[cont_Part].part_fit,mbr->mbr_particion[cont_Part].part_fit);
                //generando id
                id_generado[0]='6';
                id_generado[1]='6';
                //into to string
                disco[cont_disk].numeral++;
                string s_num = to_string(disco[cont_disk].numeral);
                char const *num_char = s_num.c_str();
                strncat(id_generado,num_char,1);

                //aqui le meto el nombre del Disco
                int contadorP=0;
                while(valor_nameE[contadorP]!=NULL){
                    char aux[1]="";
                    aux[0]= valor_nameE[contadorP];
                    strncat(id_generado,aux,1);
                    contadorP++;
                }
                //-------------------------------fin de agregar NOmbre
                strcpy(disco[cont_disk].mbr_particion[cont_Part].id,id_generado);
                cout << "Aviso -> ID generado:  "<<disco[cont_disk].mbr_particion[cont_Part].id<<endl;
                memset(id_generado,0,50);

                EBR *ebr =(EBR*) malloc(sizeof(EBR));
                //se guardan las logicas
                fseek(file,mbr->mbr_particion[cont_Part].part_start+sizeof (EBR)+1,SEEK_SET);
                fread(ebr,sizeof (EBR),1,file);
                int pos=0;
                int cont_logicas=0;

                while(ebr->part_next!=-1){
                    pos = ebr->part_next;
                    //guardo ebr
                    disco[cont_disk].ebr_logicas[cont_logicas].part_next = ebr->part_next;
                    disco[cont_disk].ebr_logicas[cont_logicas].part_start =ebr->part_start;
                    disco[cont_disk].ebr_logicas[cont_logicas].part_size  = ebr->part_s;
                    strcpy(disco[cont_disk].ebr_logicas[cont_logicas].part_fit,ebr->part_fit);
                    strcpy(disco[cont_disk].ebr_logicas[cont_logicas].part_name,ebr->part_name);
                    fseek(file,pos,SEEK_SET);
                    fread(ebr,sizeof (EBR),1,file);
                    cont_logicas++;
                }
                cout << "Aviso -> Particiona extendida ha sido montada "<<_name<<endl;
            }
            else if(tipoParticion=='l'){
                //Esto es para obtener el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;

                //busca la particion que es extendida
                char letra = disco[cont_disk].letra;
                for (int i = 0; i < 4; ++i) {
                    if(disco[cont_disk].mbr_particion[i].part_type=='e'){
                        for (int j = 0; j < 24; ++j) {
                            if(strcmp(disco[cont_disk].ebr_logicas[j].part_name,_name)==0){
                                //generar id
                                id_generado[0]='6';
                                id_generado[1]='6';

                                //into to string
                                disco[cont_disk].numeral++;
                                string s_num = to_string(disco[cont_disk].numeral);
                                char const *num_char = s_num.c_str();
                                strncat(id_generado,num_char,1);

                                //aqui le meto el nombre del Disco
                                int contadorP=0;
                                while(valor_nameE[contadorP]!=NULL){
                                    char aux[1]="";
                                    aux[0]= valor_nameE[contadorP];
                                    strncat(id_generado,aux,1);
                                    contadorP++;
                                }

                                strcpy(disco[cont_disk].ebr_logicas[j].id,id_generado);
                                cout << "Aviso -> ID generado:  "<<disco[cont_disk].ebr_logicas[j].id<<endl;
                                memset(id_generado,0,50);
                                cout << "Aviso -> Particiona primaria ha sido montada "<<_name<<endl;
                                break;
                            }
                        }
                    }
                }
                fclose(file);
            }
        }
        else{//disco es nuevo, primera vez que se ingresa en el array
            if(tipoParticion=='p'){
                //Esto es para obtener el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;
                //se copia el disco
                disco[contador_disco].letra = abdecedario[contador_abc];
                disco[contador_disco].mbr_disk_signature = mbr->mbr_dsk_signature;
                disco[contador_disco].mbr_tamano = mbr->mbr_tamano;
                disco[contador_disco].mbr_fecha_creacion  = mbr->mbr_fecha_creacion;

                //para ver si es copia
                string ruta = charToStringM(_path);
                strcpy(disco[contador_disco].path,ruta.c_str());

                //todo aqui me quede tengo que validar el nombre del disco

                //se copian las particion basicas
                for (int i = 0; i < 4; ++i) {
                    if(mbr->mbr_particion[i].part_s!=0){// solo se agregan las que tengan tamaño
                        if(strcmp(mbr->mbr_particion[i].part_name,_name)==0){//se agrega id a la basica que se monta
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit,mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name,mbr->mbr_particion[i].part_name);
                            //generando id
                            id_generado[0]='6';
                            id_generado[1]='6';
                            //aqui se valida el numero de la particion
                            disco[contador_disco].numeral++;
                            string s_num = to_string(disco[contador_disco].numeral);
                            char const *num_char = s_num.c_str();
                            strncat(id_generado,num_char,1);

                            //aqui le meto el nombre del Disco
                            int contadorP=0;
                            while(valor_nameE[contadorP]!=NULL){
                                char aux[1]="";
                                aux[0]= valor_nameE[contadorP];
                                strncat(id_generado,aux,1);
                                contadorP++;
                            }
                            strcpy(disco[contador_disco].mbr_particion[i].id,id_generado);
                            cout << "Aviso -> ID generado:  "<<disco[contador_disco].mbr_particion[i].id<<endl;
                            memset(id_generado,0,50);
                            cout << "Aviso -> Particiona primaria ha sido montada "<<_name<<endl;


                        }else{//se copian las demas
                            if(mbr->mbr_particion[i].part_type=='e'){//para buscar las logicas
                                disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                                disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                                disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                                disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                                strcpy(disco[contador_disco].mbr_particion[i].part_fit,mbr->mbr_particion[i].part_fit);
                                strcpy(disco[contador_disco].mbr_particion[i].part_name,mbr->mbr_particion[i].part_name);

                                //valido si existe logicas
                                EBR *ebr_log = (EBR*) malloc(sizeof (EBR));
                                fseek(file,mbr->mbr_particion[i].part_start+sizeof (EBR)+1,SEEK_SET);
                                fread(ebr_log,sizeof (EBR),1,file);
                                int next = ebr_log->part_next;
                                int incre=0;
                                if(ebr_log->part_next!=0){// solo si existe ebr escritas
                                    while(next!=-1){
                                        disco[contador_disco].ebr_logicas[incre].part_size = ebr_log->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = ebr_log->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = ebr_log->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit,ebr_log->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name,ebr_log->part_name);
                                        fseek(file,next,SEEK_SET);
                                        fread(ebr_log,sizeof(EBR),1,file);
                                        next = ebr_log->part_next;
                                        incre++;
                                    }
                                }

                            }else{// sigue siendo primaria
                                disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                                disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                                disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                                disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                                strcpy(disco[contador_disco].mbr_particion[i].part_fit,mbr->mbr_particion[i].part_fit);
                                strcpy(disco[contador_disco].mbr_particion[i].part_name,mbr->mbr_particion[i].part_name);
                            }
                        }
                    }
                }
                contador_abc++;//aqui se aumenta para qsue cambia a,b,c ,d e
                contador_disco++;//aqui suma el DIsco , 1,2,3...
                fclose(file);
                //todo fin de la validacion de la primarias
            }
            else if(tipoParticion=='e'){
                char letra = disco[cont_disk].letra;
                //se obtiene el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;
                //se copia
                //disco[contador_disco].letra = abdecedario[contador_abc];//agrege este para ver si jala :c
                disco[contador_disco].mbr_particion[cont_Part].part_status = mbr->mbr_particion[cont_Part].part_status;
                disco[contador_disco].mbr_particion[cont_Part].part_type = mbr->mbr_particion[cont_Part].part_type;
                disco[contador_disco].mbr_particion[cont_Part].part_size = mbr->mbr_particion[cont_Part].part_s;
                disco[contador_disco].mbr_particion[cont_Part].part_start = mbr->mbr_particion[cont_Part].part_start;
                strcpy(disco[contador_disco].mbr_particion[cont_Part].part_name,mbr->mbr_particion[cont_Part].part_name);
                strcpy(disco[contador_disco].mbr_particion[cont_Part].part_fit,mbr->mbr_particion[cont_Part].part_fit);
                //generando id
                id_generado[0]='6';
                id_generado[1]='6';
                //into to string aqui se copia el Numero de particion
                disco[contador_disco].numeral++;
                string s_num = to_string(disco[contador_disco].numeral);
                char const *num_char = s_num.c_str();
                strncat(id_generado,num_char,1);

                //aqui le meto el nombre del Disco
                int contadorP=0;
                while(valor_nameE[contadorP]!=NULL){
                    char aux[1]="";
                    aux[0]= valor_nameE[contadorP];
                    strncat(id_generado,aux,1);
                    contadorP++;
                }
                strcpy(disco[contador_disco].mbr_particion[cont_Part].id,id_generado);
                cout << "Aviso -> ID generado:  "<<disco[contador_disco].mbr_particion[cont_Part].id<<endl;
                memset(id_generado,0,50);
                EBR *ebr =(EBR*) malloc(sizeof(EBR));
                //se guardan las logicas
                fseek(file,mbr->mbr_particion[cont_Part].part_start+sizeof (EBR)+1,SEEK_SET);
                fread(ebr,sizeof (EBR),1,file);
                int pos=0;
                int cont_logicas=0;
                while(ebr->part_next!=-1){
                    pos = ebr->part_next;
                    //guardo ebr
                    disco[contador_disco].ebr_logicas[cont_logicas].part_next = ebr->part_next;
                    disco[contador_disco].ebr_logicas[cont_logicas].part_start =ebr->part_start;
                    disco[contador_disco].ebr_logicas[cont_logicas].part_size  = ebr->part_s;
                    strcpy(disco[contador_disco].ebr_logicas[cont_logicas].part_fit,ebr->part_fit);
                    strcpy(disco[contador_disco].ebr_logicas[cont_logicas].part_name,ebr->part_name);
                    fseek(file,pos,SEEK_SET);
                    fread(ebr,sizeof (EBR),1,file);
                    cont_logicas++;
                }
                contador_abc++;
                contador_disco++;
                cout << "Aviso -> Particiona extendida ha sido montada "<<_name<<endl;
            }
            else if(tipoParticion=='l'){
                // se copia disco+
                //se obtiene el nombre
                char valor_nameE[16]="";
                string NombreD = getName(_path);
                strcpy(valor_nameE,NombreD.c_str());
                cout << "EL name es: "<<valor_nameE<<endl;

                //se copia el disco
                disco[contador_disco].letra = abdecedario[contador_abc];
                disco[contador_disco].mbr_disk_signature = mbr->mbr_dsk_signature;
                disco[contador_disco].mbr_tamano = mbr->mbr_tamano;
                disco[contador_disco].mbr_fecha_creacion  = mbr->mbr_fecha_creacion;
                //para ver si es copia
                string ruta = charToStringM(_path);
                strcpy(disco[contador_disco].path,ruta.c_str());
                //las particiones primarias basica
                //se copian las particion basicas
                for (int i = 0; i < 4; ++i) {
                    if(mbr->mbr_particion[i].part_s!=0){// solo se agregan las que tengan tamaño
                        if(mbr->mbr_particion[i].part_type=='e'){//se agrega id a la basica que se monta
                            //se copia la particion extendida
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit,mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name,mbr->mbr_particion[i].part_name);

                            //valido si existe logicas
                            EBR *ebr_log = (EBR*) malloc(sizeof (EBR));
                            fseek(file,mbr->mbr_particion[i].part_start+sizeof (EBR)+1,SEEK_SET);
                            fread(ebr_log,sizeof (EBR),1,file);
                            int next = ebr_log->part_next;
                            int incre=0;
                            if(ebr_log->part_next!=0){// solo si existe ebr escritas
                                while(next!=-1){
                                    //si es el ebr que estoy buscando para montar
                                    if(strcmp(ebr_log->part_name,_name)==0) {
                                        disco[contador_disco].ebr_logicas[incre].part_size = ebr_log->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = ebr_log->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = ebr_log->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit, ebr_log->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name, ebr_log->part_name);

                                        //generar id
                                        id_generado[0] = '6';
                                        id_generado[1] = '6';
                                        //este es el numero de la particion
                                        disco[contador_disco].numeral++;
                                        string s_num = to_string(disco[contador_disco].numeral);
                                        char const *num_char = s_num.c_str();
                                        strncat(id_generado, num_char, 1);

                                        //aqui le meto el nombre del Disco
                                        int contadorP = 0;
                                        while (valor_nameE[contadorP] != NULL) {
                                            char aux[1] = "";
                                            aux[0] = valor_nameE[contadorP];
                                            strncat(id_generado, aux, 1);
                                            contadorP++;
                                        }
                                        strcpy(disco[contador_disco].ebr_logicas[incre].id, id_generado);
                                        cout << "Aviso -> ID generado:  "<<disco[contador_disco].ebr_logicas[incre].id<<endl;
                                        memset(id_generado, 0, 50);
                                        cout << "Aviso -> Particiona logica ha sido montada "<<_name<<endl;
                                        //leer siguiente
                                        fseek(file,ebr_log->part_next,SEEK_SET);
                                        fread(ebr_log,sizeof (EBR),1,file);
                                        next = ebr_log->part_next;
                                        incre++;
                                    }else{
                                        disco[contador_disco].ebr_logicas[incre].part_size = ebr_log->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = ebr_log->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = ebr_log->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit,ebr_log->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name,ebr_log->part_name);
                                        fseek(file,next,SEEK_SET);
                                        fread(ebr_log,sizeof(EBR),1,file);
                                        next = ebr_log->part_next;
                                        incre++;
                                    }

                                }
                            }

                        }else{//se copian las demas
                            // sigue siendo primaria
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit,mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name,mbr->mbr_particion[i].part_name);

                        }
                    }
                }
                contador_disco++;
                contador_abc++;
                fclose(file);
            }
        }
    }
    else{
        cout << "Error -> No existe particion para ser montada "<<_name<<endl;
    }
}

//ver montodos:
void showMount(){
    cout <<"╔═══════════════════════════════════════════════════╗"<<endl;
    cout <<"╔══════════LISTA DE PARTICIONES MONTADAS════════════╗"<<endl;
    cout <<"╠═════════════════╦════════════╦════════════════════╣"<<endl;
    cout <<"║ Nombre          ║     ID     ║  Tipo              ║"<<endl;
    cout <<"╚═════════════════╩════════════╩════════════════════╝"<<endl;
    for (int i = 0; i < 99; ++i) {
        if(disco[i].letra!='\000'){//cambios
            for (int j = 0; j < 4; ++j) {
                if(disco[i].mbr_particion[j].part_type=='p'){//es primaria agregare un or
                    if(disco[i].mbr_particion[j].part_status=='1' && disco[i].mbr_particion[j].id[0]!='\000'){
                        cout << "║ "<<disco[i].mbr_particion[j].part_name<<"  ║  "<<disco[i].mbr_particion[j].id<<"║  "<<disco[i].mbr_particion[j].part_type<<"  ║"<<endl;
                    }
                }if(disco[i].mbr_particion[j].part_type=='e'){ //enves de  else if solo dejare el if
                    if(disco[i].mbr_particion[j].part_status=='1'){
                        for (int k = 0; k < 24; ++k) {
                            if (disco[i].ebr_logicas[k].part_size!=0 && disco[i].ebr_logicas[k].id[0]!='\000'){
                                cout << "║ "<<disco[i].ebr_logicas[k].part_name<<"  ║  "<<disco[i].ebr_logicas[k].id<<"║  "<<"l"<<"  ║"<<endl;
                            }
                        }
                    }
                }
            }
        }
    }
    cout <<""<<endl;
    cout <<""<<endl;
}

//#-------------------------------------------------------------Para el Reporte del MBR  y del DISK--------------
bool validacionDotM(string _path){
    FILE *file;
    if(file = fopen(_path.c_str(),"r")){
        fclose(file);
        return true;
    }
    return false;
}
void repMbr(char _id[],char _namerep[],char _path[],char _dir[]){
    bool flag_rep = false;
    string dir = charToStringM(_dir);
    string name_dot = dir+charToStringM(_namerep)+".dot";
    string name_svg = dir+charToStringM(_namerep)+".svg";
    bool existedot = validacionDotM(name_dot);//VALIDO EL DOT y no la imagen ya que la imagen no es .png

    if(!existedot){
        for (int i = 0; i < 99; ++i) {
            if(disco[i].mbr_tamano!=0){//si existo disco
                for (int j = 0; j < 4; ++j) {
                    if(strcmp(disco[i].mbr_particion[j].id,_id)==0){// si está en las 4 basica
                        //se realiza reporte mbr
                        flag_rep=true;
                        char *tt = ctime(&disco[i].mbr_fecha_creacion);
                        string time = charToStringM(tt);
                        ofstream fs(name_dot);
                        fs << "digraph G {"<<endl;
                        fs << "rankdir=LR;"<<endl;
                        fs << "mbr["<<endl;
                        fs << "shape=plaintext "<<endl;
                        fs << "label=<"<<endl;
                        fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>"<<endl;
                        fs << "<tr><td>MBR</td></tr>"<<endl;
                        fs << "<tr><td cellpadding='4'>"<<endl;
                        fs << "<table color='orange' cellspacing='0'>"<<endl;
                        fs << "<tr><td> MBR  </td></tr>"<<endl;
                        fs << "<tr><td> Mbr_tamaño: "+to_string(disco[i].mbr_tamano)+"</td></tr>"<<endl;
                        fs << "<tr><td> Mbr_fecha_creacion: "+time+"</td></tr>"<<endl;
                        fs << "<tr><td> Mbr_signature: "+ to_string(disco[i].mbr_disk_signature)+"</td></tr>"<<endl;
                        fs << "</table>"<<endl;
                        fs << "</td>"<<endl;
                        fs << "</tr>"<<endl;
                        fs <<"\n"<<endl;
                        //para los 4 basicos
                        for (int k = 0; k < 4; ++k) {
                            if(disco[i].mbr_particion[k].part_size!=0){
                                string s;
                                s += disco[i].mbr_particion[k].part_status;
                                string t;
                                t += disco[i].mbr_particion[k].part_type;
                                fs <<"<tr><td cellpadding='4'>"<<endl;
                                fs <<"<table color='orange' cellspacing='0'>"<<endl;
                                fs <<" <tr><td> PARTICION  </td></tr>"<<endl;
                                fs << "<tr><td> Nombre: "+ charToStringM(disco[i].mbr_particion[k].part_name)+"</td></tr>"<<endl;
                                fs << "<tr><td> Status: "+s+"</td></tr>"<<endl;
                                fs << "<tr><td> Tipo: "+t+"</td></tr>"<<endl;
                                fs << "<tr><td> Fit: "+charToStringM(disco[i].mbr_particion[k].part_fit)+"</td></tr>"<<endl;
                                fs << "<tr><td> Start: "+to_string(disco[i].mbr_particion[k].part_start)+"</td></tr>"<<endl;
                                fs << "<tr><td> Size: "+to_string(disco[i].mbr_particion[k].part_size)+"</td></tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;
                                fs << "</tr>"<<endl;
                                fs << "\n"<<endl;
                            }
                        }
                        //finaliza para el mbr
                        fs << "</table>"<<endl;
                        fs << ">];"<<endl;

                        //buscar las logicas
                        for (int k = 0; k < 24; ++k) {
                            if(disco[i].ebr_logicas[k].part_size!=0){
                                nodos.push_back("ebr"+to_string(k));
                                fs << "ebr"+to_string(k)+" ["<<endl;
                                fs << "shape=plaintext"<<endl;
                                fs << "label=<"<<endl;
                                fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>"<<endl;
                                fs << "<tr><td>ebr</td></tr>" <<endl;
                                fs << "<tr><td cellpadding='4'>"<<endl;
                                fs << "<table color='orange' cellspacing='0'>"<<endl;
                                fs << "<tr><td> Nombre: "+charToStringM(disco[i].ebr_logicas[k].part_name)+" </td></tr>"<<endl;
                                fs << "<tr><td> Fit: "+charToStringM(disco[i].ebr_logicas[k].part_fit)+" </td></tr>"<<endl;
                                fs << "<tr><td> Start: "+to_string(disco[i].ebr_logicas[k].part_start)+" </td></tr>"<<endl;
                                fs << "<tr><td> Size: "+to_string(disco[i].ebr_logicas[k].part_size)+" </td></tr>"<<endl;
                                fs << "<tr><td> Next: "+to_string(disco[i].ebr_logicas[k].part_next)+" </td></tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;
                                fs << "</tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << ">];\n"<<endl;
                                fs << "\n"<<endl;
                            }
                        }
                        int tam_vec = nodos.size();
                        if(tam_vec>=1){
                            //fs << nodos[0] +" -> "+ nodos[1] <<endl;
                            for (int k = 0; k < tam_vec-1; ++k) {
                                fs << nodos[k] +" -> "+ nodos[k+1]+"\n"<<endl;
                            }
                        }
                        fs << "}\n"<<endl;
                        fs.close();
                        string  rutajpg = charToStringM(_path);
                        string render= "dot -T jpg "+name_dot+" -o "+rutajpg;
                        system(render.c_str());
                        break;
                    }
                    if(disco[i].mbr_particion[j].part_type=='e'){//si es extendida y nombre no es igual se busca en logicas
                        for (int k = 0; k < 24; ++k) {
                            if(strcmp(disco[i].ebr_logicas[k].id,_id)==0){
                                //se desmonta
                                flag_rep=true;
                                ofstream fs(name_dot);
                                char *tt = ctime(&disco[i].mbr_fecha_creacion);
                                string time = charToStringM(tt);
                                fs << "digraph G {"<<endl;
                                fs << "rankdir=LR;"<<endl;
                                fs << "mbr["<<endl;
                                fs << "shape=plaintext "<<endl;
                                fs << "label=<"<<endl;
                                fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>"<<endl;
                                fs << "<tr><td>MBR</td></tr>"<<endl;
                                fs << "<tr><td cellpadding='4'>"<<endl;
                                fs << "<table color='orange' cellspacing='0'>"<<endl;
                                fs << "<tr><td> MBR  </td></tr>"<<endl;
                                fs << "<tr><td> Mbr_tamaño: "+to_string(disco[i].mbr_tamano)+"</td></tr>"<<endl;
                                fs << "<tr><td> Mbr_fecha_creacion: "+time+"</td></tr>"<<endl;
                                fs << "<tr><td> Mbr_signature: "+ to_string(disco[i].mbr_disk_signature)+"</td></tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;
                                fs << "</tr>"<<endl;
                                fs <<"\n"<<endl;
                                //para los 4 basicos
                                for (int k = 0; k < 4; ++k) {
                                    if(disco[i].mbr_particion[k].part_size!=0){
                                        string s;
                                        s += disco[i].mbr_particion[k].part_status;
                                        string t;
                                        t += disco[i].mbr_particion[k].part_type;
                                        fs <<"<tr><td cellpadding='4'>"<<endl;
                                        fs <<"<table color='orange' cellspacing='0'>"<<endl;
                                        fs <<" <tr><td> PARTICION  </td></tr>"<<endl;
                                        fs << "<tr><td> Nombre: "+ charToStringM(disco[i].mbr_particion[k].part_name)+"</td></tr>"<<endl;
                                        fs << "<tr><td> Status: "+s+"</td></tr>"<<endl;
                                        fs << "<tr><td> Tipo: "+t+"</td></tr>"<<endl;
                                        fs << "<tr><td> Fit: "+charToStringM(disco[i].mbr_particion[k].part_fit)+"</td></tr>"<<endl;
                                        fs << "<tr><td> Start: "+to_string(disco[i].mbr_particion[k].part_start)+"</td></tr>"<<endl;
                                        fs << "<tr><td> Size: "+to_string(disco[i].mbr_particion[k].part_size)+"</td></tr>"<<endl;
                                        fs << "</table>"<<endl;
                                        fs << "</td>"<<endl;
                                        fs << "</tr>"<<endl;
                                        fs << "\n"<<endl;
                                    }
                                }
                                //finaliza para el mbr
                                fs << "</table>"<<endl;
                                fs << ">];"<<endl;

                                //buscar las logicas
                                for (int k = 0; k < 24; ++k) {
                                    if(disco[i].ebr_logicas[k].part_size!=0){
                                        nodos.push_back("ebr"+to_string(k));
                                        fs << "ebr"+to_string(k)+" ["<<endl;
                                        fs << "shape=plaintext"<<endl;
                                        fs << "label=<"<<endl;
                                        fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>"<<endl;
                                        fs << "<tr><td>ebr</td></tr>" <<endl;
                                        fs << "<tr><td cellpadding='4'>"<<endl;
                                        fs << "<table color='orange' cellspacing='0'>"<<endl;
                                        fs << "<tr><td> Nombre: "+charToStringM(disco[i].ebr_logicas[k].part_name)+" </td></tr>"<<endl;
                                        fs << "<tr><td> Fit: "+charToStringM(disco[i].ebr_logicas[k].part_fit)+" </td></tr>"<<endl;
                                        fs << "<tr><td> Start: "+to_string(disco[i].ebr_logicas[k].part_start)+" </td></tr>"<<endl;
                                        fs << "<tr><td> Size: "+to_string(disco[i].ebr_logicas[k].part_size)+" </td></tr>"<<endl;
                                        fs << "<tr><td> Next: "+to_string(disco[i].ebr_logicas[k].part_next)+" </td></tr>"<<endl;
                                        fs << "</table>"<<endl;
                                        fs << "</td>"<<endl;
                                        fs << "</tr>"<<endl;
                                        fs << "</table>"<<endl;
                                        fs << ">];\n"<<endl;
                                        fs << "\n"<<endl;
                                    }
                                }
                                int tam_vec = nodos.size();
                                if(tam_vec>=1){
                                    //fs << nodos[0] +" -> "+ nodos[1] <<endl;
                                    for (int k = 0; k < tam_vec-1; ++k) {
                                        fs << nodos[k] +" -> "+ nodos[k+1]+"\n"<<endl;
                                    }
                                }
                                fs << "}\n"<<endl;
                                fs.close();
                                string  rutajpg = charToStringM(_path);
                                string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
                                system(render.c_str());
                                break;

                            }
                        }
                    }
                }

            }
        }
        //para validar error
        if(flag_rep==false){
            cout << "Error -> No se puede generer reporte porque no se ha encontrado particion con id "<<_id<<endl;
        }else{
            //render .dot
            /* string rutajpg = charToString(_path);
             string render = "dot "+name_dot+" -Tsvg -o "+name_svg;
             cout << "Aviso -> Reporte "<< render <<" Generado exitosamente"<<endl;
             system(render.c_str());*/
        }
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre "<<name_dot<<endl;
    }
}

void repDisk(char _id[],char _namerep[],char _path[],char _dir[]){
    bool flag_rep=false;
    string dir = charToStringM(_dir);
    string name_dot = dir+charToStringM(_namerep)+".dot";
    int suma=0;
    float espaLibre=100.0;//lo puse
    bool flag_existe = validacionDotM(name_dot);
    if(!flag_existe){
        for (int i = 0; i < 99; ++i) {
            if(disco[i].mbr_tamano!=0){
                for (int j = 0; j < 4; ++j) {
                    if(strcmp(disco[i].mbr_particion[j].id,_id)==0){// sin son las 4 basicas
                        cout << "Encuentra particion"<<endl;
                        //generando .dot
                        ofstream fs(name_dot);
                        fs << "digraph G{"<<endl;
                        fs << "tbl ["<<endl;
                        fs << "shape=plaintext"<<endl;
                        fs << "label=<"<<endl,
                                fs << "<table border='2' cellborder='0' color='blue' cellspacing='1'>"<<endl;
                        fs << "<tr>"<<endl;
                        fs << "<td colspan='1' rowspan='1'>"<<endl;
                        fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>"<<endl;
                        fs << "<tr><td>MBR</td></tr>"<<endl;
                        //fs << "<tr><td>Espacion Libre: "+res2+"</td></tr>"<<endl;
                        fs << "</table>"<<endl;
                        fs << "</td>"<<endl;

                        for (int k = 0; k < 4; ++k) {
                            if(disco[i].mbr_particion[k].part_type=='p'){
                                int sizeP, sizeOP = 0;
                                sizeP = disco[i].mbr_particion[k].part_size;
                                sizeOP = (sizeP*100)/disco[i].mbr_tamano;
                                string tam = to_string(sizeOP);
                                suma +=sizeP;

                                float sizeF, sizeOF = 0;//
                                sizeF=disco[i].mbr_particion[k].part_size*1.00;
                                sizeOF=(sizeF*100)/disco[i].mbr_tamano;
                                string tam2 = to_string(sizeOF);
                                cout << "***************************************"<< endl;
                                cout << "El tmaño es: "<<disco[i].mbr_tamano << endl;
                                cout << "antes del string: "<< sizeOF<< endl;
                                cout << "El % es: "<<tam << endl;
                                espaLibre -=sizeOF;
                                fs << "<td colspan='1' rowspan='1'>"<<endl;
                                fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>"<<endl;
                                fs << "<tr><td>Primaria <br/>Nombre:"+ charToStringM(disco[i].mbr_particion[k].part_name)+"<br/> Formato: "+charToStringM(disco[i].mbr_particion[k].formato)+" <br/>"+tam2+" % del disco</td></tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;

                            }else if(disco[i].mbr_particion[k].part_size==0){
                                fs << "<td colspan='1' rowspan='1'>"<<endl;
                                fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>"<<endl;
                                fs << "<tr><td>Libre</td></tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;
                            }else if(disco[i].mbr_particion[k].part_type=='e'){
                                fs << "<td colspan='1' rowspan='1'>"<<endl;
                                fs << "<table color='red' border='1' cellborder='1' cellpadding='10' cellspacing='0'>"<<endl;
                                fs << "<tr>"<<endl;
                                int sizeP, sizeOP = 0;
                                sizeP = disco[i].mbr_particion[k].part_size;
                                sizeOP = (sizeP*100)/disco[i].mbr_tamano;
                                string tam = to_string(sizeOP);
                                suma+= sizeP;

                                for (int l = 0; l < 24; ++l) {
                                    if(disco[i].ebr_logicas[l].part_size!=0){
                                        double size = disco[i].ebr_logicas[l].part_size;
                                        double sizedisk = disco[i].mbr_tamano;
                                        double op = (size*100)/sizedisk;
                                        stringstream tmp;
                                        tmp << setprecision(3) << fixed << op;
                                        string res = tmp.str();
                                        cout << res << endl;
                                        espaLibre -=op;
                                        fs << "<td>EBR</td><td>"+charToStringM(disco[i].ebr_logicas[l].part_name) +"<br/> "+res+" % del disco</td>"<<endl;
                                    }
                                }
                                fs << "<td>Libre</td>"<<endl;
                                fs <<"</tr>"<<endl;
                                fs << "</table>"<<endl;
                                fs << "</td>"<<endl;
                            }
                        }
                        fs << "<td colspan='1' rowspan='1'>"<<endl;
                        fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>"<<endl;
                        stringstream tmp2;
                        tmp2 << setprecision(4) << fixed << espaLibre;
                        string res2 = tmp2.str();
                        fs << "<tr><td> Porcentaje Libre del disco: "+res2+" % </td></tr>"<<endl;
                        fs << "</table>"<<endl;
                        fs << "</td>"<<endl;
                        fs << "</tr>"<<endl;
                        fs << "</table>"<<endl;
                        fs << ">];"<<endl;
                        fs << "}"<<endl;
                        fs.close();
                        //puse lo siguiente:
                        string  rutajpg = charToStringM(_path);
                        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
                        system(render.c_str());
                    }
                }
            }
        }
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}
//#---------------------------------------------------------------------------------------------------------------

void analisisMount(char comando[]){
    int contador=0;
    char lineacomando[100]="";

    char valor_path[100]="";
    char valor_name[100]="";

    bool flag_path=false;
    bool flag_name=false;

    while(comando[contador]!=NULL){
        if(comando[contador]==' '|| comando[contador]=='\n'){
            contador++;
            memset(lineacomando,0,100);
        } else{
            char aux[1]="";
            aux[0]= tolower(comando[contador]);
            strncat(lineacomando,aux,1);
            contador++;
        }
        //validacion de argumentos de mount
        if(strcmp(lineacomando,"mount")==0){
            cout << "Encontro: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            contador++;
        }else if(strcmp(lineacomando,">path=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            flag_path=true;
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
        }else if(strcmp(lineacomando,">name=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            flag_name=true;
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
                        aux[0] = comando[contador];
                        strncat(valor_name,aux,1);
                        contador++;
                    }
                }
            }
            cout << "Valor: "<<valor_name<<endl;
        }
    }
    if(flag_name && flag_path){//es para montar
        bool flag_disk = validacionPathMount(valor_path);//valida que si exista el archivo

        if(flag_disk){//existe disco
            //disco original
            cout << "Si existe disco se validaran los demas parametros"<<endl;
            bool exi=buscarDiscoPath(valor_name,valor_path);
            if(!exi){
                cout << "Particion nose repite se realizara montaje"<<endl;
                montajeMount(valor_path,valor_name);
            } else{
                cout << "Error-> Particion se repite no se realizara montaje"<<endl;
            }

        }else{
            cout << "Error -> No existe el disco que se intenta montar "<<valor_path<<endl;
        }
    }else{//solo para listar
        showMount();
    }
}