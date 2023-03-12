//
// Created by gerson on 27/02/23.
//

#include "loggin.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <iomanip>

#include "../comandos/loggin.h"
#include "../comandos/mount.h"
#include "../estructuras/user.h"
#include "../estructuras/mbr.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/apuntador.h"

using namespace std;
bool ExisteUsuario=false,Seguir=true;

string USR="",PWD="",Id="";// esto me servira para almacenar el usuario pas y el ID

UsuarioActual usuG;
//Quitar espacios para verificacion
//para retornar el ID

string getId(){
    //solo retornamos el ID DEL DISCO para el mkdir y mkFIle
    return usuG.particion;
}
//--------------------------------------------------------------

bool validacionLogin(){
    if(usuG.sesionIniciada){
        return true;
    }
    return false;
}
void Logut(){
    if(usuG.sesionIniciada){
        cout<<"AVISO-> Cerradon sesion..."<<endl;
        usuG.sesionIniciada= false;
        usuG.UID="";
        usuG.password="";
        usuG.usuario="";
        usuG.grupo="";
        usuG.particion="";
    }else{
        cout<<"ERROR-> No existe sesion Iniciada "<<endl;
    }

}

string QuitarEspacios(string cadena){
    string salir="";
    int final=cadena.length();

    for(int i = 0; i <final; i++) {
        if((cadena[i]>='a'&&cadena[i]<='z')||(cadena[i]>='A'&&cadena[i]<='Z')||(cadena[i]>='0'&&cadena[i]<='9')||(cadena[i]=='\n')||(cadena[i]==',')) {
            salir += cadena[i];
        }
    }
    return salir;
}

//Lee linea a linea las credenciales en el archivo de usuarios y luego las compara en SepararComas
string SepararSalto(string cadena,UsuarioActual *objetoEstructura){
    cout<<"LLEGUE AQUI A SALTO"<<endl;
    string s = cadena;
    string delimiter = "\n";
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());

        if(Seguir){
            cout<<"EL token->"<<token<<endl;
            SepararComas(token,objetoEstructura);
        }
    }
    cout<<"El valor de S->"<<s<<endl;
    return s;
}

//Iterar y comparar para comparar las credenciales
string SepararComas(string cadena,UsuarioActual *objetoEstructura){
    cout<<"LLegamos a separar Comas"<<endl;
    string s = cadena;
    string delimiter = ",";
    int contador=0;
    bool comparar=false;
    string grupo1="",user1="",password1="",uid1="";
    //1,G,root
    //1,U,root,root,123
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        contador++;
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());

        if(contador==2&&(token=="U"||token=="u")){

            comparar=true;
        }

        if(contador==1){
            uid1=token;
        }else if (contador==3) {
            grupo1=token;
        }
        else if(contador==4){
            string unir=QuitarEspacios(token);
            user1=unir;

        }else if(contador==5){
            string unir=QuitarEspacios(token);
            password1=unir;
            cout<<"Pasword"<<password1<<endl;

        }
    }

    if(comparar){
        cout<<"Si entre a usuario"<<endl;
        string unir=QuitarEspacios(s);
        password1=unir;
        if(user1==USR){
            cout<<"PWD quemada: "<<password1<<" Ingresada: "<<PWD<<endl;
            if(PWD==password1){
                cout<<"Usuario Correcto, Bienvenido"<<endl;
                bool auxSe=Seguir;
                auxSe=false;
                objetoEstructura->sesionIniciada=true;
                objetoEstructura->grupo=grupo1;
                cout<<"Valor del Grupo: "<<endl;
                objetoEstructura->password=password1;
                cout<<"Valor del user1: "<<user1<<endl;
                objetoEstructura->usuario=user1;
                cout<<"Valor del UID: "<<uid1<<endl;
                objetoEstructura->UID=uid1;
                cout<<"Valor del Particion: "<<Id<<endl;
                objetoEstructura->particion=Id;
                comparar=false;

                bool auxEs=&ExisteUsuario;
                auxEs= true;
                cout<<"Se finalizara Analisis"<<endl;
            }else{
                cout<<"Credenciales Incorrectas"<<endl;
            }
        }
    }
    return s;

}

string readpointer(APUNTADORES blockeapuntador,FILE *archivo,SUPER_BLOQUE superblock){
    string Lectura="";
    //Apartir de aca ya hemos extraido la informacion del bloquepunterosimple
    //procedemos a leer los indices de los 16 bloques directos
    for(int j=0;j<16;j++){
        int indicearchivo = blockeapuntador.b_apuntadores[j];
        if(indicearchivo != -1){
            BLOQUEARCHIVO blockfile;
            fseek(archivo,superblock.s_block_start+indicearchivo*sizeof(BLOQUEARCHIVO),SEEK_SET);//PROBAR SI SE PONE EL +1
            fread(&blockfile,sizeof (BLOQUEARCHIVO),1,archivo);
            //Ya hemos obtenido el bloque de archivo
            //procedemos a concatenar la informacion
            for(int g=0;g<64;g++){
                Lectura+=blockfile.b_content[g];
            }
        }
    }
    return Lectura;
}

//------------------------------Lectura de los apuntadores:
//SIRVE PARA LEER INODOS DOBLES DE ARCHIVOS EN CASO DE QUE HUBIERAN

//SIRVE PARA LEER APUNTADORES TRIPLES DE ARCHIVOS EN CASO DE QUE HUBIERAN
string readtriplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock){
    int indicepunterotriple = 14;
    string lecturapunterotriple= "";
    /*PASOS
        1. Ir al bloque 14 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros triple que contiene 16 indirectos
        4. Leer los 16 indlogin -usr->root -pass->123 -id->281Dpirectos uno a uno que contienen otro indirecto
        5. Obtener el indirecto
        6. Leer el texto de cada directo
        7. concatenar el texto
    */
    int indiceBloque = inodo.i_block[indicepunterotriple];
    if(indiceBloque != -1){
        APUNTADORES bloqueapuntadortriple;
        fseek(archivo,superblock.s_block_start+indiceBloque*sizeof(APUNTADORES),SEEK_SET);
        fread(&bloqueapuntadortriple,sizeof (APUNTADORES),1,archivo);
        //Apartir de aca ya hemos extraido la informacion del bloquepunterotriple
        //procedemos a leer los indices de los 16 bloques indirectos
        for(int j=0;j<16;j++){
            int indiceapuntadorindirecto = bloqueapuntadortriple.b_apuntadores[j];
            if(indiceapuntadorindirecto != -1){
                APUNTADORES bloqueapuntadorIndirecto;
                fseek(archivo,superblock.s_block_start+indiceapuntadorindirecto*sizeof(APUNTADORES),SEEK_SET);
                fread(&bloqueapuntadorIndirecto,sizeof(APUNTADORES),1,archivo);
                //Ya obtuvimos el apuntador Indirecto
                //Procedemos a leer sus siguientes 16 apuntadores a directos
                int indiceapuntadordirecto = bloqueapuntadorIndirecto.b_apuntadores[j];
                if(indiceapuntadordirecto != -1){
                    for(int m=0;m<16;m++){
                        APUNTADORES bloqueapuntadordirecto;
                        fseek(archivo,superblock.s_block_start+indiceapuntadordirecto*sizeof(APUNTADORES),SEEK_SET);
                        fread(&bloqueapuntadordirecto,sizeof(APUNTADORES),1,archivo);
                        lecturapunterotriple += readpointer(bloqueapuntadordirecto,archivo,superblock);
                    }

                }
            }
        }

    }
    return lecturapunterotriple;
}

string readdoublepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock){
    int indicepunterodoble = 13;
    string lecturapunterodouble= "";
    /*PASOS
        1. Ir al bloque 13 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros doble que contiene 16 indirectos
        4. Leer los 16 indirectos uno a uno que contienen un directo
        5. Obtener el texto de cada directo
        6. concatenar el texto
    */
    int indiceBloque= inodo.i_block[indicepunterodoble];
    if(indiceBloque != -1){
        APUNTADORES bloquepunterodoble;
        fseek(archivo,superblock.s_block_start+indiceBloque*sizeof(APUNTADORES),SEEK_SET);
        fread(&bloquepunterodoble,sizeof (APUNTADORES),1,archivo);
        //Apartir de aca ya hemos extraido la informacion del bloquepunterodoble
        //procedemos a leer los indices de los 16 bloques indirectos
        for(int j=0;j<16;j++){
            int indiceapuntadordirecto = bloquepunterodoble.b_apuntadores[j];
            if(indiceapuntadordirecto != -1){
                APUNTADORES bloqueapuntadordirecto;
                fseek(archivo,superblock.s_block_start+indiceapuntadordirecto*sizeof(APUNTADORES),SEEK_SET);
                fread(&bloqueapuntadordirecto,sizeof(APUNTADORES),1,archivo);
                //Ya obtuvimos el apuntador directo que obtiene informacion
                //Procedemos a leer
                lecturapunterodouble += readpointer(bloqueapuntadordirecto,archivo,superblock);
            }
        }
    }
    return lecturapunterodouble;
}


string readsimplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock){
    int indicepunterosimple = 12;                           //ESTA ES LA POSICION DE LOS PUNTEROS SIMPLES
    string lecturapunterosimple = "";                       //VALOR DE RETORNO
    /*PASOS
        1. Ir al bloque 12 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros debido a que es simple los apuntadores del apuntador son directos
        4. Ir leyendo los 16 bloques que contienen texto
        5. concatenar el texto
    */
    int indiceBloque= inodo.i_block[indicepunterosimple];    //indice del bloque indirecto simple
    if(indiceBloque!=-1){ //El indice del puntero simple debe estar ocupado

        APUNTADORES bloquepunterosimple;    //Bloque de apuntador a buscar
        fseek(archivo,superblock.s_block_start+indiceBloque*sizeof(APUNTADORES),SEEK_SET);
        fread(&bloquepunterosimple,sizeof (APUNTADORES),1,archivo);

        //Iniciamos la lectura del bloque de apuntadores
        lecturapunterosimple += readpointer(bloquepunterosimple,archivo,superblock);

    }
    return lecturapunterosimple;
}


string LeerArchivoCompleto(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock){
    string archivotexto ="";

    //LEER APUNTADORES DIRECTOS
    for (int i=0;i<12;i++) {
        int indiceBloque=inodo.i_block[i];
        if(indiceBloque!=-1){
            BLOQUEARCHIVO blockfile;
            fseek(archivo,superblock.s_block_start+indiceBloque*sizeof (BLOQUEARCHIVO),SEEK_SET);
            fread(&blockfile,sizeof (BLOQUEARCHIVO),1,archivo);
            for(int j=0;j<64;j++){
                if(blockfile.b_content[j] =='\0'){

                } else{

                    archivotexto+=blockfile.b_content[j];
                }

            }
        }
    }
    cout<<"lo que retornare: "<<archivotexto<<endl;
    //LEER APUNTADORES INDIRECTOS
    //archivotexto += readsimplepointer(inodo,archivo,superblock);
    //archivotexto += readdoublepointer(inodo,archivo,superblock);
    //archivotexto += readtriplepointer(inodo,archivo,superblock);
    return archivotexto;
}

void recorrido(DISCO disco,char _id[],char path[]){
    FILE* file;
    file = fopen(path,"rb+");
    //Se leer el mbr
    //Usuario,Password,UID,Grupo,Particion;
    MBR *mbr = (MBR*) malloc(sizeof (MBR));
    fseek(file,0,SEEK_SET);
    fread(mbr,sizeof (MBR),1,file);//se leer el mbr
    //busco la particion en disco fisico
    int indice=0;
    bool val_ind=false;
    string nombre_buscar;
    for (int i = 0; i < 4; ++i) {
        if(strcmp(disco.mbr_particion[i].id,_id)==0){//encuetro la particion
            nombre_buscar = charToStringM(disco.mbr_particion[i].part_name);//validar si creo uno o deja la del mount
            break;
        }
    }
    for (int i = 0; i < 4; ++i) {
        if(strcmp(mbr->mbr_particion[i].part_name,nombre_buscar.c_str())==0){
            indice = i;
            val_ind=true;
            break;
        }
    }
    if(val_ind){
        SUPER_BLOQUE auxsuperbloque;
        fseek(file,mbr->mbr_particion[indice].part_start+sizeof(mbr->mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuperbloque,sizeof(SUPER_BLOQUE),1,file);

        if(auxsuperbloque.s_magic!=0){
            INODOS inodoUsuario;
            fseek(file,auxsuperbloque.s_inode_start+sizeof(INODOS)+1,SEEK_SET);//LEO EL INODE DEL USUARIO
            fread(&inodoUsuario,sizeof (INODOS),1,file);
            string filetext = LeerArchivoCompleto(inodoUsuario,file,auxsuperbloque);
            cout<<"\nArchivo de usuarios: ";
            cout<<endl;
            cout<<"De la lectura: \n"<<filetext;
            cout<<"din de la lecturas-------"<<endl;
            cout<<endl;
            SepararSalto(filetext,&usuG); ///antes de validar el el suario se manda a salto
            cout<<"Sali de serparar Salto"<<endl;
            if(ExisteUsuario){
                cout<<"\nEl usuario no existe";
            }
        }else{
            cout<<"\n\t El disco no se encuentra formateado";
            fclose(file);
        }
    }
}

void valiLogin(char _id[]){
    DISCO disco = buscarDisco(_id);
    if(disco.mbr_tamano!=0){
        bool disk_or = validacionPathMount(disco.path);
        if(disk_or){
            cout << "Alerta-->existe Particion Montada se validara Usuario"<<endl;
            if(USR=="root" && PWD=="123"){
                cout << "Credenciales correctas"<<endl;
                usuG.sesionIniciada= true;
                usuG.grupo="";
                usuG.password="123";
                usuG.usuario="root";
                usuG.UID="1";
                usuG.particion=_id;
            } else{
                cout << "Credenciales incorrectas"<<endl;
            }
            cout << "FInal de valiLOgin"<<endl;
        }else{
            cout << "Error -> El disco no existe para aplicar mkfs"<<endl;
        }
    }else{
        cout << "Error -> ID no existe para realizar el Formato"<<endl;
    }
}

void analisisLogin(char comando[]){
    int contador=0;
    char lineacomando[200]="";

    char valor_usr[100]="";
    char valor_pass[100]="";
    char valor_id[50]="";

    bool flag_usr=false;
    bool flag_pass=false;
    bool flag_id=false;

    while(comando[contador]!=NULL) {
        if (comando[contador] == ' ' || comando[contador] == '\n') {
            contador++;
            memset(lineacomando, 0, 100);
        } else {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }
        //validacion de argumentos de mount
        if (strcmp(lineacomando, "login") == 0) {
            cout << "Encontro: " << lineacomando << endl;
            memset(lineacomando, 0, 200);
            contador++;
        }else if (strcmp(lineacomando, ">user=") == 0) {
            cout << "Argumento: " << lineacomando << endl;
            memset(lineacomando, 0, 200);
            flag_usr = true;
            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = comando[contador];
                    strncat(valor_usr, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: " << valor_usr << endl;
        }else if (strcmp(lineacomando, ">pass=") == 0) {
            cout << "Argumento: " << lineacomando << endl;
            memset(lineacomando, 0, 200);
            flag_pass = true;
            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = comando[contador];
                    strncat(valor_pass, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: " << valor_pass << endl;
        }else if (strcmp(lineacomando, ">id=") == 0) {
            cout << "Argumento: " << lineacomando << endl;
            memset(lineacomando, 0, 200);
            flag_id = true;
            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = comando[contador];
                    strncat(valor_id, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: " << valor_id << endl;
        }
    }

    if(flag_id && flag_usr && flag_pass){
        if(!usuG.sesionIniciada){
            cout << "Inicializo la estructura Usuario ya que no hay ningun login" << endl;
            usuG.sesionIniciada= false;
            usuG.UID="";
            usuG.password="";
            usuG.usuario="";
            usuG.grupo="";
            usuG.particion="";
            //--------------------------------------------------------
            cout << "Se encontraton los parametros obligatorios" << endl;
            USR=charToStringM(valor_usr);
            PWD=charToStringM(valor_pass);
            Id=charToStringM(valor_id);;
            valiLogin(valor_id);
            cout << "Final de metodo" << endl;
        } else{
            cout << "Existe sesion Abierta debe de Cerrarlo" << endl;
        }
    } else{
        cout << "Faltan parametros obligaros" << endl;
    }
}
