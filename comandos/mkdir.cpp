//
// Created by gerson on 28/02/23.
//

#include "mkdir.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <list>
#include "../comandos/mount.h"
#include "../comandos/mkdisk.h"
#include "../comandos/mkdir.h"
#include "../comandos/loggin.h"

//Estructuras
#include "../estructuras/disk.h"
#include "../estructuras/mbr.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/journaling.h"
#include "../estructuras/bloque_carpeta.h"
#include "../estructuras/apuntador.h"

using namespace std;
int returnBlockAvailable(SUPER_BLOQUE *super,FILE *file){
    int numerobloques = super->s_block_count;
    char bitbloques[numerobloques];
    int indicebloque=-1;
    fseek(file,super->s_bm_block_start,SEEK_SET);
    fread(&bitbloques,64,1,file);
    for (int i = 0; i < numerobloques; ++i) {
        if(bitbloques[i]=='0'){
            indicebloque = i;
            break;
        }
    }
    return indicebloque;
}
int returnInodeAvailable(SUPER_BLOQUE *super,FILE *file){
    int numerodeinodos = super->s_inode_count;
    int indiceinodo=-1;
    char bitinodos[numerodeinodos];
    fseek(file,super->s_bm_inode_start,SEEK_SET);
    fread(&bitinodos,sizeof(char),numerodeinodos,file);
    for (int i = 0; i < numerodeinodos; ++i) {
        if(bitinodos[i]=='0'){
            indiceinodo = i;
            break;
        }
    }
    return indiceinodo;
}

int createFolder(FILE *file,SUPER_BLOQUE *super,int *indicepadre,string namecarpeta){
    int indiceaotorgarbloque = returnBlockAvailable(super,file);
    int  indiceaotorgarinodo = returnInodeAvailable(super,file);//editar por returInodeavalible

    if(indiceaotorgarinodo >0 && indiceaotorgarbloque >0){
        if(file){
            char bitinodo='1';
            fseek(file,super->s_bm_inode_start+indiceaotorgarinodo*sizeof(char),SEEK_SET);
            fwrite(&bitinodo,sizeof(char),1,file);

            char bitbloque='1';
            fseek(file,super->s_bm_block_start+indiceaotorgarbloque*sizeof(char),SEEK_SET);
            fwrite(&bitbloque,sizeof(char),1,file);

            INODOS newCarpeta;
            newCarpeta.i_gid = 1;
            newCarpeta.i_uid = 1;
            newCarpeta.i_s = 0;
            time_t timeactual;
            timeactual = time(NULL);
            newCarpeta.i_ctime = timeactual;
            newCarpeta.i_atime = timeactual;
            newCarpeta.i_mtime = timeactual;
            newCarpeta.i_type = '0';
            for (int i = 0; i < 15; ++i) {
                newCarpeta.i_block[i]=-1;
            }
            newCarpeta.i_block[0] = indiceaotorgarbloque;
            //se escribe el inodo
            fseek(file,super->s_inode_start+indiceaotorgarinodo*sizeof (INODOS),SEEK_SET);
            fwrite(&newCarpeta,sizeof(INODOS),1,file);
            super->s_free_inodes_count--;

            BLOQUECARPETA carpetaroot;
            CONTENT contentraiz;
            strcpy(contentraiz.b_name,".");
            contentraiz.b_inodo = indiceaotorgarinodo;
            carpetaroot.b_content[0] = contentraiz;
            contentraiz.b_inodo = (*indicepadre);
            strcpy(contentraiz.b_name,"..");
            carpetaroot.b_content[1]=contentraiz;
            memset(&contentraiz.b_name,0,sizeof(contentraiz.b_name));
            strcpy(contentraiz.b_name,"-");
            contentraiz.b_inodo = -1;
            carpetaroot.b_content[2] = contentraiz;
            carpetaroot.b_content[3] = contentraiz;

            //se escribe el bloque carpeta raiz
            fseek(file,super->s_block_start+indiceaotorgarbloque*64,SEEK_SET);
            fwrite(&carpetaroot,sizeof(BLOQUECARPETA),1,file);
            super->s_free_blocks_count--;

        }
    }else{
        cout << "Error -> No hay espacio en particion"<<endl;
    }
    return indiceaotorgarinodo;
}

int buscarindirectoSimple(INODOS *inode,int indicedelinodo,FILE *file,SUPER_BLOQUE *super,string namecarpeta,int *indiceactualdeinodo){
    int valorinodo=-1;
    bool hayespacioenelsimple=false;
    if(indicedelinodo > 0){
        APUNTADORES apuntadorsimple;
        fseek(file,super->s_block_start+((int)inode->i_block[12])*64,SEEK_SET);
        fread(&apuntadorsimple,64,1,file);
        for (int i = 0; i < 16; ++i) {
            if((apuntadorsimple.b_apuntadores[i]>0)){
                BLOQUECARPETA carpetalectura;
                fseek(file,super->s_block_start+apuntadorsimple.b_apuntadores[i]*64,SEEK_SET);
                fread(&carpetalectura,64,1,file);
                for (int j = 0; j < 4 && valorinodo==-1; j++) {
                    if(carpetalectura.b_content[j].b_inodo==-1){
                        int indicevalorinodo = createFolder(file,super,indiceactualdeinodo,namecarpeta);
                        carpetalectura.b_content[j].b_inodo = indicevalorinodo;
                        strcpy(carpetalectura.b_content[j].b_name,namecarpeta.c_str());
                        fseek(file,super->s_block_start+(apuntadorsimple.b_apuntadores[i])*64,SEEK_SET);
                        fwrite(&carpetalectura,64,1,file);
                        valorinodo = indicevalorinodo;
                        return  indicevalorinodo;
                    }
                }
            }else{
                hayespacioenelsimple=true;
            }
        }

        if(hayespacioenelsimple){
            for (int i = 0; i < 16; ++i) {
                if(apuntadorsimple.b_apuntadores[i]==-1){
                    int valorasignamientoalbloque = returnBlockAvailable(super,file);
                    if(valorasignamientoalbloque>0){
                        apuntadorsimple.b_apuntadores[i] = valorasignamientoalbloque;
                        fseek(file,super->s_block_start+indicedelinodo*64,SEEK_SET);
                        fwrite(&apuntadorsimple,64,1,file);

                        char bitbloque='1';
                        fseek(file,super->s_bm_block_start+valorasignamientoalbloque*sizeof(char),SEEK_SET);
                        fwrite(&bitbloque,sizeof(char),1,file);

                        BLOQUECARPETA carpetadirectorio;
                        CONTENT contentpadre;
                        strcpy(contentpadre.b_name,"-");
                        contentpadre.b_inodo = -1;
                        for (int j = 0; j < 4; ++j) {
                            carpetadirectorio.b_content[j]=contentpadre;
                        }
                        int nvalorinodo = createFolder(file,super,indiceactualdeinodo,namecarpeta);
                        strcpy(contentpadre.b_name,namecarpeta.c_str());
                        contentpadre.b_inodo = nvalorinodo;
                        carpetadirectorio.b_content[0] = contentpadre;
                        fseek(file,super->s_block_start+valorasignamientoalbloque*64,SEEK_SET);
                        fwrite(&carpetadirectorio,64,1,file);
                        super->s_free_blocks_count--;

                        return nvalorinodo;
                    }else{
                        cout << "Error -> Memoria insuficiente"<<endl;
                    }
                }
            }
        }
    }else{
        int valorapuntador = returnBlockAvailable(super,file);

        inode->i_block[12] = valorapuntador;
        fseek(file,super->s_inode_start+(*indiceactualdeinodo)*sizeof(INODOS),SEEK_SET);
        fwrite(inode,sizeof(INODOS),1,file);
        if(valorapuntador>0){
            APUNTADORES apuntadorsimple;
            for (int i = 0; i < 16; ++i) {
                apuntadorsimple.b_apuntadores[i]=-1;
            }
            char bitbloque='3';
            fseek(file,super->s_bm_block_start+valorapuntador*sizeof(char),SEEK_SET);
            fwrite(&bitbloque,sizeof(char),1,file);

            int valorasignamientoalbloque = returnBlockAvailable(super,file);
            apuntadorsimple.b_apuntadores[0] = valorasignamientoalbloque;
            fseek(file,super->s_block_start+valorapuntador*sizeof(APUNTADORES),SEEK_SET);
            fwrite(&apuntadorsimple,sizeof(APUNTADORES),1,file);

            bitbloque='1';
            fseek(file,super->s_bm_block_start+valorasignamientoalbloque*sizeof(char),SEEK_SET);
            fwrite(&bitbloque,sizeof(char),1,file);
            BLOQUECARPETA carpetadirectorio;
            CONTENT contentpadre;
            strcpy(contentpadre.b_name,"-");
            contentpadre.b_inodo=-1;
            for (int i = 0; i < 4; ++i) {
                carpetadirectorio.b_content[i]=contentpadre;
            }
            int indicebloquenuevo = createFolder(file,super,indiceactualdeinodo,namecarpeta);
            strcpy(contentpadre.b_name,namecarpeta.c_str());
            contentpadre.b_inodo = indicebloquenuevo;
            carpetadirectorio.b_content[0] = contentpadre;
            fseek(file,super->s_block_start+valorasignamientoalbloque*64,SEEK_SET);
            fwrite(&carpetadirectorio,64,1,file);
            super->s_free_blocks_count--;
            return  indicebloquenuevo;

        }else{
            cout << "Error -> Memoria insuficiente"<<endl;
        }
    }
    return -1;
}

//buscar espacio en carpetas
int BuscarEspacio(INODOS *inode,FILE *file,SUPER_BLOQUE *super,int *indiceactualinodo,string namecarpeta){
    int valorinodo=-1;
    //buscar en los directos
    bool hayespaciosenindirectos=false;//cambio a true
    for (int i = 0; i < 12 && valorinodo==-1; i++) {
        if(inode->i_block[i]!=-1){
            BLOQUECARPETA carpetalectura;
            fseek(file,super->s_block_start+(inode->i_block[i])*64,SEEK_SET);
            fread(&carpetalectura,64,1,file);
            for (int j = 0; j < 4; ++j) {
                if(carpetalectura.b_content[j].b_inodo==-1){
                    //para crear carpeta raiz
                    valorinodo = createFolder(file,super,indiceactualinodo,namecarpeta);
                    strcpy(carpetalectura.b_content[j].b_name,namecarpeta.c_str());
                    carpetalectura.b_content[j].b_inodo = valorinodo;
                    fseek(file,super->s_block_start+(inode->i_block[i])*64,SEEK_SET);
                    fwrite(&carpetalectura,sizeof(BLOQUECARPETA),1,file);
                    break;
                }
            }
        }else{
            hayespaciosenindirectos = true;
        }
    }

    if(hayespaciosenindirectos){
        for (int i = 0; i < 12 && valorinodo==-1; i++) {
            if(inode->i_block[i]==-1){
                int nuevobloque= returnBlockAvailable(super,file);
                char bitbloque='1';
                fseek(file,super->s_bm_block_start+nuevobloque*sizeof (char),SEEK_SET);
                fwrite(&bitbloque,sizeof(char),1,file);
                inode->i_block[i] = nuevobloque;
                BLOQUECARPETA carpetalectura;
                CONTENT contenido;
                strcpy(contenido.b_name,"-");
                contenido.b_inodo=-1;
                for (int j = 0; j < 4; ++j) {
                    carpetalectura.b_content[j]=contenido;
                }
                valorinodo = createFolder(file,super,indiceactualinodo,namecarpeta);
                strcpy(carpetalectura.b_content[0].b_name,namecarpeta.c_str());
                carpetalectura.b_content[0].b_inodo = valorinodo;
                //se escribe el bloque del inodo
                fseek(file,super->s_block_start+(nuevobloque)*64,SEEK_SET);
                fwrite(&carpetalectura,64,1,file);
                //reescribimos el inodo
                fseek(file,super->s_inode_start+(*indiceactualinodo)*sizeof(INODOS),SEEK_SET);
                fwrite(inode,sizeof (INODOS),1,file);

            }else{

            }
        }
    }

    //se busca el indirecto en caso de no hay
    if(valorinodo>0){
        return valorinodo;
    }else{
        int simple = buscarindirectoSimple(inode,inode->i_block[12],file,super,namecarpeta,indiceactualinodo);
        if(simple>0){
            return simple;
        }else{
            valorinodo=-1;
        }
    }
    return  valorinodo;
}

//buscador de apuntadores simples
int searchPointerS(APUNTADORES *apuntadorsimple,FILE *file,SUPER_BLOQUE *super,int *bloquesdirectos,string namecarpeta){
    int valor=-1;
    int i_bloque = apuntadorsimple->b_apuntadores[(*bloquesdirectos)];
    if(i_bloque!=-1){
        BLOQUECARPETA carpetalectura;
        fseek(file,super->s_block_start+(i_bloque)*64,SEEK_SET);
        fread(&carpetalectura,64,1,file);
        for (int i = 0; i < 4; ++i) {
            if(charToString(carpetalectura.b_content[i].b_name)==namecarpeta){
                valor = carpetalectura.b_content[i].b_inodo;
                break;
            }
        }
    }
    //si no hay
    if(valor!=-1){
        return valor;
    }else{
        (*bloquesdirectos) = (*bloquesdirectos)+1;
        if((*bloquesdirectos)==16){
            return valor;
        }else{
            return searchPointerS(apuntadorsimple,file,super,bloquesdirectos,namecarpeta);
        }
    }
}

//buscar de apuntadores dobles
int searchPointerD(APUNTADORES *apuntadordoble,FILE *file,SUPER_BLOQUE *super,int *i_apuntadorsimple,string namecarpeta){
    int valor=-1;
    int indicesimple=0;
    int index = apuntadordoble->b_apuntadores[(*i_apuntadorsimple)];
    if(index!=-1){
        APUNTADORES pointersimple;
        fseek(file,super->s_block_start+index*64,SEEK_SET);
        fread(&pointersimple,64,1,file);
        valor = searchPointerS(&pointersimple,file,super,&indicesimple,namecarpeta);
    }
    if(valor!=-1){
        return valor;
    }else{
        (*i_apuntadorsimple)= (*i_apuntadorsimple)+1;
        if((*i_apuntadorsimple)==16){
            return valor;
        }else{
            return searchPointerD(apuntadordoble,file,super,i_apuntadorsimple,namecarpeta);
        }
    }
}

//buscar apuntadores triples
int searchPointerT(APUNTADORES *apuntadortriple,FILE *file,SUPER_BLOQUE *super,int *i_apuntadordoble,string namecarpeta){
    int valor=-1;
    int indicedoble=0;
    int index = apuntadortriple->b_apuntadores[(*i_apuntadordoble)];
    if(index!=-1){
        APUNTADORES pointerdoble;
        fseek(file,super->s_block_start+index*64,SEEK_SET);
        fread(&pointerdoble,64,1,file);
        valor = searchPointerD(&pointerdoble,file,super,&indicedoble,namecarpeta);
    }

    if((valor!=-1)){
        return valor;
    }else{
        (*i_apuntadordoble) = (*i_apuntadordoble)+1;
        if((*i_apuntadordoble)==16){
            return valor;
        }else{
            return searchPointerT(apuntadortriple,file,super,i_apuntadordoble,namecarpeta);
        }
    }
}

int searchFolder(INODOS *inodeFolder,FILE *file,SUPER_BLOQUE *super,string namecarpeta){
    int valor=-1;
    cout << "se llego a serachFolder"<<endl;
    if(inodeFolder->i_type=='0'){
        //buscar en carpetas directas
        for (int i = 0; i < 12; ++i) {
            if(inodeFolder->i_block[i]!=-1){
                BLOQUECARPETA carpetab;
                fseek(file,super->s_block_start+(inodeFolder->i_block[i])*64,SEEK_SET);
                fread(&carpetab,64,1,file);
                for (int j = 0; j < 4; ++j) {
                    if(charToString(carpetab.b_content[j].b_name)==namecarpeta){
                        valor = carpetab.b_content[j].b_inodo;
                        return carpetab.b_content[j].b_inodo;
                    }
                }
            }
        }
        //si no encuentro nada reviso en los indirectos
        if(valor=!-1){
            return valor;
        }else{
            if(inodeFolder->i_block[12]!=-1 && valor==-1){

                APUNTADORES apuntador_simple;
                fseek(file,super->s_block_start+(inodeFolder->i_block[12])*64,SEEK_SET);
                fread(&apuntador_simple,64,1,file);
                int bloquesdirecto=0;
                valor = searchPointerS(&apuntador_simple,file,super,&bloquesdirecto,namecarpeta);
            }
            if(inodeFolder->i_block[13]!=-1 && valor==-1){
                APUNTADORES apuntador_doble;
                fseek(file,super->s_block_start+(inodeFolder->i_block[13])*64,SEEK_SET);
                fread(&apuntador_doble,64,1,file);
                int i_apuntadorsimple = 0;
                valor = searchPointerD(&apuntador_doble,file,super,&i_apuntadorsimple,namecarpeta);
            }
            if(inodeFolder->i_block[14]!=-1 && valor ==-1){
                APUNTADORES apuntador_tripe;
                fseek(file,super->s_block_start+(inodeFolder->i_block[14])*64,SEEK_SET);
                fread(&apuntador_tripe,64,1,file);
                int i_apuntadordoble=0;
                valor = searchPointerT(&apuntador_tripe,file,super,&i_apuntadordoble,namecarpeta);
            }
            if(valor>0){
                return valor;
            }else{
                return -1;
            }
        }
    }else{
        cout << "Error -> Inodo de archivos"<<endl;
        return valor;
    }
}

bool createRecursivo(FILE*file,SUPER_BLOQUE* super,list<string>*Lista,int *indiceactualnodo){

    INODOS inodoactual;
    int indice = (*indiceactualnodo);
    fseek(file,super->s_inode_start+indice*sizeof(INODOS),SEEK_SET);
    fread(&inodoactual,sizeof (INODOS),1,file);

    //crear carpeta
    string nombreCarpeta;
    int contador=0;
    for(string item:*Lista){
        if(contador==0){
            nombreCarpeta=item;
            break;
        }
    }

    if(nombreCarpeta=="#"){return true;}
    int nuevaCarpeta = searchFolder(&inodoactual,file,super,nombreCarpeta);
    bool recursividad = P;

    if(nuevaCarpeta!=-1){ recursividad = true; ++control;}

    if(recursividad || (control-2==(int)Lista->size()) || control+2 == (int)Lista->size()){
        Lista->pop_front();
        if(nuevaCarpeta!=-1){
            (*indiceactualnodo) = nuevaCarpeta;
            return createRecursivo(file,super,Lista,indiceactualnodo);
        }else{
            //int espacio_carpeta = Buscar
            int espacio_carpeta = BuscarEspacio(&inodoactual,file,super,indiceactualnodo,nombreCarpeta);
            if(espacio_carpeta!=-1){
                cout << "Suficiente memoria en create recursivo"<<endl;
                (*indiceactualnodo) = espacio_carpeta;
            }else{
                cout << "Error -> Memoria insuficiente"<<endl;
                return true;
            }

        }
        int sizelist = Lista->size();
        if(sizelist==0){
            return true;
        }else{
            return createRecursivo(file,super,Lista,indiceactualnodo);
        }
    }else{
        cout << "Aviso -> necesita @p"<<endl;
        return true;
    }
}

void ejecutarMkdir(DISCO disco,char _id[],char _directorio[],char _path[]){
    if(disco.mbr_tamano!=0){// si existe
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToString(disco.mbr_particion[i].part_name);
                break;
            }
        }

        FILE *file;
        file = fopen(_path,"rb+");
        MBR *mbr = (MBR*) malloc(sizeof (MBR));
        fseek(file,0,SEEK_SET);
        fread(mbr,sizeof (MBR),1,file);
        int indice;
        for (int i = 0; i < 4; ++i) {//se extrae el indice
            if(strcmp(mbr->mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice = i;
                break;
            }
        }
        //leo el super bloque
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr->mbr_particion[indice].part_start+sizeof(mbr->mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        if(auxsuper.s_magic!=0){
            //update jorunaling
            JOURNALING auxjournaling;
            int inicio_jour = mbr->mbr_particion[indice].part_start+sizeof(mbr->mbr_particion[indice])+sizeof(SUPER_BLOQUE)+1;
            fseek(file,inicio_jour,SEEK_SET);
            fread(&auxjournaling,sizeof (JOURNALING),1,file);
            auxjournaling.journal_tipo = '1';//es directorio
            strcpy(auxjournaling.journal_propietario,"root");
            auxjournaling.journal_persimo=664;
            strcpy(auxjournaling.journal_nombre,"directorio");
            strcpy(auxjournaling.journal_contenido,"ruta");
            strcpy(auxjournaling.jorunal_tipo_operacion,"crear");
            //generar el tiempo
            cout << "Se genera guarda accion en el journaling"<<endl;
            time_t timecurrent;
            timecurrent = time(NULL);
            auxjournaling.journal_fecha = timecurrent;
            //se escribe
            fseek(file,inicio_jour,SEEK_SET);
            fwrite(&auxjournaling,sizeof (JOURNALING),1,file);
            //se separa las carpetas

            string s = charToString(_directorio);
            string delimitador = "/";
            int cont = 0;
            size_t pos = 0;
            string token;
            while((pos = s.find(delimitador))!=std::string::npos){
                token = s.substr(0,pos);
                s.erase(0,pos+delimitador.length());
                if(cont==0){
                    // se imite la primera
                }else{
                    ListaCarpetas.push_back(token);

                }
                cont++;
            }

            ListaCarpetas.push_back(s);
            if(ListaCarpetas.size()>0){//hay varias carpetas
                ListaCarpetas.push_back("#");//ultimo item para control
                int indiceinodo =0;//empezara en la carpeta raiz
                createRecursivo(file,&auxsuper,&ListaCarpetas,&indiceinodo);
            }
        }else{
            cout << "Error -> La particion no se encuentra formato"<<endl;
            fclose(file);
        }
    }else{
        cout << "Error -> El ID ingresado no existe, en el disco especificado"<<endl;
    }
    ListaCarpetas.clear();
}


void analisisMkdir(char comando[]){
    int contador=0;
    char lineacomando[200]="";

    char valor_id[10]="";
    char valor_path[100]="";
    char valor_p[10]="";

    bool flag_p=false;

    while(comando[contador]!=NULL){
        if (comando[contador] == ' ') {
            contador++;
            memset(lineacomando, 0, 200);
        } else {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        //validaciond de arguments
        if(strcmp(lineacomando,"mkdir")==0){
            cout << "Encontro: "<<lineacomando<<endl;
            memset(lineacomando,0,200);
            contador++;
        }else if(strcmp(lineacomando,">id=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,200);
            //para obtener id
            while(comando[contador]!=NULL){
                if(comando[contador]==' '||comando[contador]=='\n'){
                    contador++;
                    break;
                }else{
                    char aux[1]="";
                    aux[0] = tolower(comando[contador]);
                    strncat(valor_id,aux,1);
                    contador++;
                }
            }
            cout << "Valor: "<<valor_id<<endl;
        }else if(strcmp(lineacomando,">path=")==0){
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
        }else if(strcmp(lineacomando,">r")==0){
            cout << "Argumento :"<<lineacomando<<endl;
            cout << "Encontre r"<<endl;
            memset(lineacomando,0,200);
            flag_p = true;
        }
    }
    if(flag_p){
        bool valiLO=validacionLogin();
        if(valiLO){
            cout << "Aviso -> Ya existe la raiz"<<endl;
            P = true;
            cout << "Hay sesion Iniciada si se puede ejecutar el comando"<<endl;
            string idAux=getId();
            cout << "ID de la particion montada es: "<<idAux<<endl;
            strcpy(valor_id,idAux.c_str());//aqui el ID  que tenga el login lo cambio a valor ID
            DISCO disco = buscarDisco(valor_id);
            ejecutarMkdir(disco,valor_id,valor_path,disco.path);
        } else{
            cout << "para ejecutar este comando debe de realizar inicio de sesion"<<endl;
        }
    }else{
        bool valiLO=validacionLogin();
        if(valiLO){
            cout << "Aviso -> Se procede a crear carpetas..."<<endl;
            cout << "Hay sesion Iniciada si se puede ejecutar el comando"<<endl;
            string idAux=getId();
            cout << "ID de la particion montada es: "<<idAux<<endl;
            strcpy(valor_id,idAux.c_str());//aqui el ID  que tenga el login lo cambio a valor ID
            DISCO disco = buscarDisco(valor_id);
            ejecutarMkdir(disco,valor_id,valor_path,disco.path);

        }else{
            cout << "para ejecutar este comando debe de realizar inicio de sesion"<<endl;
        }
    }
}
