//
// Created by gerson on 2/03/23.
//

#include "rep.h"
#include "../comandos/mount.h"
#include "../estructuras/mbr.h"
#include "../estructuras/super_bloque.h"
#include "../estructuras/bloque_archivo.h"
#include "../estructuras/journaling.h"

using namespace std;

string charToStringR(char* a){
    string  s = a;
    return s;
}

bool validacionDot(string _path){
    FILE *file;
    if(file = fopen(_path.c_str(),"r")){
        fclose(file);
        return true;
    }
    return false;
}

//#------------------------------------------Metodos para el Tre-------------------------------------------
string InodoTabla(int indice,INODOS * inode,int tree){
    string tableinode = "";
    string conexionesinodos ="";
    //SOLO POSEE CONEXIONES A BLOQUES
    tableinode += "Inodo"+to_string(indice)+" [\n";
    tableinode += "shape=plaintext\n";
    tableinode += "label=<\n";
    tableinode += "<table border=\'0\' cellborder=\'1\' color=\'green\' cellspacing=\'0\'>\n";
    tableinode += "\t<tr><td PORT = \"I"+to_string(indice)+"\" >INODO"+to_string(indice)+"</td></tr>\n";
    tableinode += "\t<tr><td cellpadding=\'4\'>\n";
    tableinode += "\t\t<table color=\'cyan4\' cellspacing=\'0\'>\n";
    //INICIO DE INFORMACION
    tableinode += "\t\t\t<tr><td>Nombre</td><td>Valor</td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_uidt</td><td>";
    tableinode += to_string(inode->i_uid);
    tableinode +="</td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_gid</td><td>";
    tableinode += to_string(inode->i_gid);
    tableinode += "</td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_size</td><td>";
    tableinode += to_string(inode->i_s);
    tableinode += "</td></tr>\n";
    char *tta = ctime(&inode->i_atime);
    string timea = charToStringR(tta);
    char *ttc = ctime(&inode->i_ctime);
    string timec = charToStringR(ttc);
    char *ttm = ctime(&inode->i_mtime);
    string timem = charToStringR(ttm);
    cout << timem <<endl;
    tableinode += "\t\t\t<tr><td>i_atime</td><td> "+timea+" </td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_ctime</td><td> "+timec+" </td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_mtime</td><td> "+timem+" </td></tr>\n";
    for(int i=0;i<15;i++){
        string indiceconexion = (int)inode->i_block[i]>=0?"PORT = \"B"+to_string(i)+"\"":"";
        tableinode += "\t\t\t<tr><td>i_block"+to_string(i)+"</td><td "+indiceconexion+">";
        tableinode += to_string(inode->i_block[i]);
        tableinode += "</td></tr>\n";
        //CONEXIONES INODOS A BLOQUES
        conexionesinodos +=indiceconexion != "" ?"\nInodo"+to_string(indice)+":"+"B"+to_string(i)+" -> Bloque"+to_string(inode->i_block[i])+":BL"+to_string(inode->i_block[i])+";":"";
        cout<<conexionesinodos<<endl;
    }       //InodoA:BB -> BloqueC:BC
    /*
        Donde:
        A = Numero del Inodo en Lectura
        B = Numero del Bloque apuntador del Inodo en Lectura
        C = Numero del Bloque al que apunta el Bloque B
        El siguiente tiene el mismo valor C ya que es para que apunte al encabezado del bloque
    */
    tableinode += "\t\t\t<tr><td>i_type</td><td>";
    tableinode += inode->i_type;
    tableinode += "</td></tr>\n";
    tableinode += "\t\t\t<tr><td>i_perm</td><td>";
    tableinode += "Si";
    tableinode += "</td></tr>\n";
    //FINAL DE INFORMACION
    tableinode += "\t\t</table>\n";
    tableinode += "\t</td>\n";
    tableinode += "\t</tr>\n";
    tableinode += "\t<tr><td>Inodo</td></tr>\n";
    tableinode += "</table>\n";
    tableinode += " >];\n";
    if(tree){
        tableinode +=  conexionesinodos ;
    }
    return tableinode;
}

string BlockFolder_Tabla(int indice,BLOQUECARPETA *blockfolder,int tree){
    string tablefolder="";
    //SOLO POSEE CONEXION A OTROS INODOS QUE PUEDEN SER INODOS DE ARCHIVOS O CARPETAS
    string conexionesainodos ="";
    string indicebloque;
    tablefolder += "Bloque"+to_string(indice)+" [\n";
    tablefolder += "shape=plaintext\n";
    tablefolder += "label=<\n";
    tablefolder += "<table border=\'0\' cellborder=\'1\' color=\'yellow\' cellspacing=\'0\'>\n";
    tablefolder += "\t<tr><td PORT =\"BL"+to_string(indice)+"\">Bloque de Carpeta</td></tr>\n";
    tablefolder += "\t<tr><td cellpadding=\'4\'>\n";
    tablefolder += "\t\t<table color=\'gray\' cellspacing=\'0\'>\n";
    //INICIO DE INFORMACION
    tablefolder += "\t\t\t<tr><td>B_NAME</td><td>B_INODO</td></tr>\n";
    for(int i=0;i<4;i++){
        indicebloque =  (((int)blockfolder->b_content[i].b_inodo)>0)?"PORT = \"B"+to_string(i)+"\"":"";
        tablefolder += "\t\t\t<tr><td>"+charToStringR(blockfolder->b_content[i].b_name)+"</td><td "+indicebloque+">"+to_string(blockfolder->b_content[i].b_inodo)+"</td></tr>\n";
        if(charToStringR(blockfolder->b_content[i].b_name) =="."|| charToStringR(blockfolder->b_content[i].b_name) ==".."){
        }else{
            conexionesainodos += indicebloque != ""?"\nBloque"+to_string(indice)+":"+"B"+to_string(i)+" -> Inodo"+to_string(blockfolder->b_content[i].b_inodo)+":I"+to_string(blockfolder->b_content[i].b_inodo)+";":"";
        }
    }
    //FINAL DE INFORMACION
    tablefolder += "\t\t</table>\n";
    tablefolder += "</td>\n";
    tablefolder += "</tr>\n";
    tablefolder += "\t<tr><td>CARPETA</td></tr>\n";
    tablefolder += "</table>\n";
    tablefolder += " >];\n";
    if(tree){
        tablefolder +="\n\n";
        tablefolder += conexionesainodos;
    }
    return tablefolder;
}

string BlockFile_Tabla(int indice,BLOQUEARCHIVO *blockfile,int tree){
    string tablefile="";
    //NO POSEE CONEXION A OTRO BLOQUE O INODO
    tablefile += "Bloque"+to_string(indice)+" [\n";
    tablefile += "shape=plaintext\n";
    tablefile += "label=<\n";
    tablefile += "<table border=\'0\' cellborder=\'1\' color=\'black\' cellspacing=\'0\'>\n";
    tablefile += "\t<tr><td PORT =\"BL"+to_string(indice)+"\">Bloque de Archivo</td></tr>\n";
    tablefile += "\t<tr><td cellpadding=\'4\'>\n";
    tablefile += "\t\t<table color=\'red\' cellspacing=\'0\'>\n";
    //INICIO DE INFORMACION
    tablefile += "\t\t\t<tr><td>Contenido</td></tr>\n";
    tablefile += "\t\t\t<tr><td>";
    for(int i=0;i<64;i++){
        if(blockfile->b_content[i]!='\0')
            tablefile += to_string(blockfile->b_content[i]);
    }
    tablefile +="</td></tr>\n";
    tablefile += "\t\t</table>\n";
    tablefile +="</td>\n";
    tablefile += "</tr>\n";
    //FINAL DE INFORMACION
    tablefile += "\t<tr><td>ARCHIVO</td></tr>\n";
    tablefile += "</table>\n";
    tablefile += " >];\n";
    return tablefile;
}

string BlockPointer_Tabla(int indice,APUNTADORES *blockpointer,int tree){
    string tablepointer = "";
    string conexionesBloque;    //BLOQUE - BLOQUE
    //SOLO POSEE CONEXIONES A BLOQUES SOLAMENTE
    tablepointer += "Bloque"+to_string(indice)+" [\n";
    tablepointer += "shape=plaintext\n";
    tablepointer += "label=<\n";
    tablepointer += "<table border=\'0\' cellborder=\'1\' color=\'brown\' cellspacing=\'0\'>\n";
    tablepointer += "\t<tr><td PORT = \"BL"+to_string(indice)+"\" >Bloque de Apuntadores</td></tr>\n";
    tablepointer += "\t<tr><td cellpadding=\'4\'>\n";
    tablepointer += "\t\t<table color=\'orange\' cellspacing=\'0\'>\n";
    //INICIO DE INFORMACION
    for(int i=0; i<16;i++){
        string indiceBloque =  (((int)blockpointer->b_apuntadores[i])>0)?"PORT = \"B"+to_string(i)+"\"":"";
        tablepointer += "\t\t\t<tr><td>i_block"+to_string(i)+"</td><td "+indiceBloque+" >"+to_string(blockpointer->b_apuntadores[i])+"</td></tr>\n";
        conexionesBloque += indiceBloque !="" ?"\nBloque"+to_string(indice)+":"+"B"+to_string(i)+" -> Bloque"+to_string(blockpointer->b_apuntadores[i])+":BL"+to_string(blockpointer->b_apuntadores[i])+";":"";
    }
    //FINAL DE INFORMACION
    tablepointer += "\t\t</table>\n";
    tablepointer +="</td>\n";
    tablepointer += "</tr>\n";
    tablepointer += "\t<tr><td>APUNTADOR</td></tr>\n";
    tablepointer += "</table>\n";
    tablepointer += " >];\n";
    if(tree){
        tablepointer +="\n\n";
        tablepointer += conexionesBloque;
    }
    return tablepointer;
}
//reporte del tree
void repTree(char _id[], char _name[], char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";
    string name_image = dir+ charToStringR(_name)+".svg";

    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);

    char pathdisk[100]="";
    bool disK_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disK_or){
        strcpy(pathnew,disco.path);
    }
    if(!existedot){
        string graphsuper;
        //se lee
        FILE *file;
        file = fopen(pathnew,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se lee el super bloque
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        int num_de_inodos = auxsuper.s_inode_count;
        int inodo_en_uso = auxsuper.s_inode_count-auxsuper.s_free_inodes_count;
        string graphInodos="";
        char bit_nodos[num_de_inodos];
        fseek(file,auxsuper.s_bm_inode_start,SEEK_SET);
        fread(&bit_nodos,sizeof(char),num_de_inodos,file);
        //inicia contenidos
        for (int i = 0; i < num_de_inodos; ++i) {
            if(bit_nodos[i]=='1'){
                INODOS inodo_read;
                fseek(file,auxsuper.s_inode_start+i*sizeof (INODOS),SEEK_SET);
                fread(&inodo_read,sizeof (INODOS),1,file);
                graphInodos += InodoTabla(i,&inodo_read,1);
                graphInodos += "\n\n";
            }
        }
        //Siguen los bloques
        string allbloques;
        int num_bloques = auxsuper.s_block_count;
        char bit_bloques[num_bloques];
        int bloque_en_uso = auxsuper.s_block_count-auxsuper.s_free_blocks_count;
        //se lee
        fseek(file,auxsuper.s_bm_block_start,SEEK_SET);
        fread(&bit_bloques,sizeof (char),num_bloques,file);
        //se procede a conectarlos
        for (int i = 0; i < num_bloques; ++i) {
            if(bit_bloques[i] == '1'){
                //para carpetas
                BLOQUECARPETA readcarpeta;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&readcarpeta,64,1,file);
                allbloques+= BlockFolder_Tabla(i,&readcarpeta,1);
            }else if(bit_bloques[i]=='2'){
                //para archivos
                BLOQUEARCHIVO readarchivo;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&readarchivo,64,1,file);
                allbloques+= BlockFile_Tabla(i,&readarchivo,1);

            }else if(bit_bloques[i]=='3'){
                //para apuntadores
                APUNTADORES readapuntador;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&readapuntador,64,1,file);
                allbloques+= BlockPointer_Tabla(i,&readapuntador,1);
            }
        }
        //finalizando
        string arbol = "digraph {\n rankdir=LR;\n"+graphInodos+"\n\n\n"+allbloques+"\n}\n";
        fclose(file);
        //generar .dot
        ofstream  fs(name_dot);
        fs << arbol<<endl;
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

//#----------------------------------------------------------------------------
//par inodos:
void repInodos(char _id[], char _name[],char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";

    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }

    if(!existedot){
        string graphsuper;

        //se lee
        FILE *file;
        file = fopen(disco.path,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }

        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se leer super
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        int numeroinodos = auxsuper.s_inode_count;
        int nodoenuso = auxsuper.s_inode_count-auxsuper.s_free_inodes_count;
        string graficaInodos="";
        char bitnodos[numeroinodos];
        fseek(file,auxsuper.s_bm_inode_start,SEEK_SET);
        fread(&bitnodos,sizeof(char),numeroinodos,file);
        //comenzamos a graficar
        graficaInodos+="digraph {\n";
        //iniciar a graficar inodos
        for (int i = 0; i < numeroinodos; ++i) {
            if(bitnodos[i]=='1'){
                INODOS inodolectura;
                fseek(file,auxsuper.s_inode_start+i*sizeof(INODOS),SEEK_SET);
                fread(&inodolectura,sizeof(INODOS),1,file);
                graficaInodos += InodoTabla(i,&inodolectura,0);
                graficaInodos += "\n\n";
            }
        }
        graficaInodos += "}\n";
        fclose(file);
        ofstream fs (name_dot);
        fs << graficaInodos<<endl;
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

//para block:
void repBlock(char _id[], char _name[], char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";

    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }
    if(!existedot){
        string graphsuper;
        //se lee
        FILE *file;
        file = fopen(disco.path,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se lee el super bloque
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        string todoslosblques="";
        int numerobloques = auxsuper.s_block_count;
        char bitbloques[numerobloques];
        int bloqueenuso = auxsuper.s_block_count-auxsuper.s_free_blocks_count;
        //lectura de bloques
        fseek(file,auxsuper.s_bm_block_start,SEEK_SET);
        fread(&bitbloques,sizeof(char),numerobloques,file);
        todoslosblques+="digraph {\n";
        todoslosblques+="rankdir=LR;\n";
        //buscar bloques
        for (int i = 0; i < numerobloques; ++i) {
            if(bitbloques[i]=='1'){//para carpetas
                BLOQUECARPETA carpetaread;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&carpetaread,64,1,file);
                todoslosblques+= BlockFolder_Tabla(i,&carpetaread,0);
            }else if(bitbloques[i]=='2'){//para archivos
                BLOQUEARCHIVO archivoread;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&archivoread,64,1,file);
                todoslosblques+= BlockFile_Tabla(i,&archivoread,0);

            }else if(bitbloques[i]=='3'){//para apuntadores
                APUNTADORES pointerread;
                fseek(file,auxsuper.s_block_start+i*64,SEEK_SET);
                fread(&pointerread,64,1,file);
                todoslosblques+= BlockPointer_Tabla(i,&pointerread,0);

            }
        }
        //finaliznado recorrido
        todoslosblques+="}\n";
        fclose(file);
        ofstream fs(name_dot);
        fs << todoslosblques<<endl;
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());

    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

//journaling:
void repJournaling(char _id[],char _name[],char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";

    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }
    if(!existedot){
        string graphsuper;
        //se lee
        FILE *file;
        file = fopen(disco.path,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se leer el jorunaling
        JOURNALING jour;
        int iniciojour = mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+sizeof(SUPER_BLOQUE)+1;
        fseek(file,iniciojour,SEEK_SET);
        fread(&jour,sizeof(JOURNALING),1,file);

        ofstream fs(name_dot);
        fs << "digraph J {"<<endl;
        fs << "TableJour ["<<endl;
        fs << "shape=plaintext"<<endl;
        fs << "color=forestgreen"<<endl;
        fs << "label=<"<<endl;
        fs << "<table border='1' cellborder='1' >"<<endl;
        fs << "<tr><td>JOURNALING </td></tr>"<<endl;
        fs << "<tr><td>Journal_tipo_operacion</td><td>Creacion</td></tr>"<<endl;//charToStringR(jour.jorunal_tipo_operacion)
        fs << "<tr><td>Journal_tipo</td><td> 1 </td></tr>"<<endl;
        fs << "<tr><td>Journal_nombre</td><td>"+charToStringR(jour.journal_nombre)+"</td></tr>"<<endl;
        fs << "<tr><td>Journal_contenido</td><td>"+charToStringR(jour.journal_contenido)+"</td></tr>"<<endl;
        char *fecha = ctime(&jour.journal_fecha);
        string date = charToStringR(fecha);
        fs << "<tr><td>Journal_fecha</td><td>"+date+"</td></tr>"<<endl;
        fs << "<tr><td>Journal_propietario</td><td>root</td></tr>"<<endl;
        fs << "<tr><td>Journal_Permisos</td><td>777</td></tr>"<<endl;
        fs << "<tr><td>"+charToStringR(mbr.mbr_particion[indice].part_name)+"      "+charToStringR(disco.path)+" </td></tr>"<<endl;
        fs << "</table>"<<endl;
        fs << ">];"<<endl;
        fs << "}"<<endl;
        fclose(file);
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

//super:block
void repSuper(char _id[], char _name[], char _path[],char _dir[]){
    //se lee disco
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";
    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }
    if(!existedot){
        string graphsuper;
        //se lee
        FILE *file;
        file = fopen(pathnew,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }

        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se lee el super bloque
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        ofstream  fs(name_dot);
        fs << "digraph {"<<endl;
        fs << "rankdir=LR;"<<endl;
        fs <<"Super0 ["<<endl;
        fs << "shape=plaintext"<<endl;
        fs << "label=<"<<endl;
        fs << "<table border='0' cellborder='1' color='goldenrod1' cellspacing='0'>"<<endl;
        fs << "<tr><td PORT = \"I0\" >SUPERBLOQUE</td></tr>"<<endl;
        fs << "\t<tr><td cellpadding='4'>"<<endl;
        fs << "<table color='dodgerblue' cellspacing='0'>"<<endl;
        fs << "<tr><td>Nombre</td><td>Valor</td></tr>"<<endl;
        fs << "<tr><td>s_inodes_count</td><td>"+to_string(auxsuper.s_inode_count)+"</td></tr>"<<endl;
        fs << "<tr><td>s_block_count</td><td>"+to_string(auxsuper.s_block_count)+"</td></tr>"<<endl;
        fs << "<tr><td>s_free_blocks_count</td><td>"+to_string(auxsuper.s_free_blocks_count)+"</td></tr>"<<endl;
        fs << "<tr><td>s_free_inodes_count</td><td>"+to_string(auxsuper.s_free_inodes_count)+"</td></tr>"<<endl;
        char *time1 = ctime(&auxsuper.s_mtime);
        string t1 = charToStringR(time1);
        fs << "<tr><td>s_mtime</td><td>"+t1+"</td></tr>"<<endl;
        char *time2 = ctime(&auxsuper.s_umtime);
        string t2 = charToStringR(time2);
        fs << "<tr><td>s_umtime</td><td>"+t2+"</td></tr>"<<endl;
        fs << "<tr><td>s_mnt_count</td><td> "+to_string(auxsuper.s_mnt_count)+" </td></tr>"<<endl;
        fs << "<tr><td>s_magic</td><td >0xEF53</td></tr>"<<endl;
        fs << "<tr><td>s_inodes_size</td><td> "+to_string(auxsuper.s_inode_s)+" </td></tr>"<<endl;
        fs << "<tr><td>s_block_size</td><td> "+to_string(auxsuper.s_block_s)+" </td></tr>"<<endl;
        fs << "<tr><td>s_first_ino</td><td> "+to_string(auxsuper.s_first_ino)+" </td></tr>"<<endl;
        fs << "<tr><td>s_first_blo</td><td> "+to_string(auxsuper.s_first_blo)+" </td></tr>"<<endl;
        fs << "<tr><td>s_bm_inode_start</td><td> "+to_string(auxsuper.s_bm_inode_start)+" </td></tr>"<<endl;
        fs << "<tr><td>s_bm_block_start</td><td> "+to_string(auxsuper.s_bm_block_start)+" </td></tr>"<<endl;
        fs << "<tr><td>s_inode_start</td><td> "+to_string(auxsuper.s_inode_start)+" </td></tr>"<<endl;
        fs << "<tr><td>s_block_start</td><td> "+to_string(auxsuper.s_block_start)+" </td></tr>"<<endl;
        fs << "</table>"<<endl;
        fs << "</td>"<<endl;
        fs << "</tr>"<<endl;
        fs << "<tr><td> "+charToStringR(mbr.mbr_particion[indice].part_name)+"     "+charToStringR(disco.path)+"  </td></tr>"<<endl;
        fs << "</table>"<<endl;
        fs << ">];"<<endl;
        fs << "}"<<endl;
        fclose(file);
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya exiset un .dot con ese nombre"<<endl;
    }
}

bool validacionDirectorioReporte(char _direc[]){
    DIR* directorio;
    if(directorio= opendir(_direc)){
        closedir(directorio);
        return true;
    }else{
        /*closedir(directorio);*/
        return false;
    }
}

bool validacionReporte(char _path[]){
    FILE *file;
    if(file= fopen(_path,"r")){
        fclose(file);
        return true;
    }else{
        //fclose(file);
        return false;
    }
}

void repBitmapInode(char _id[],char _name[],char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";
    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);
    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }

    if(!existedot){
        string graphsuper;
        //se lee
        FILE *file;
        file = fopen(pathnew,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se leer super
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);

        cout<<"lei el superbloque"<<endl;
        //se arma la estructura

        //char numeroinodos = auxsuper.s_inode_count;
        char bitinodos[auxsuper.s_inode_count];
        fseek(file,auxsuper.s_bm_inode_start,SEEK_SET);
        fread(&bitinodos,sizeof(char),1,file);

        ofstream fs (name_dot);
        fs << "digraph H {"<<endl;
        fs << "TableBitInodo ["<<endl;
        fs << "shape=plaintext"<<endl;
        fs << "color=blue"<<endl;
        fs << "label=<"<<endl;
        fs << "<table border='1' cellborder='0'>"<<endl;
        int contadorinodo=0;
        bool flag_salto = true;
        for (int i = 0; i < auxsuper.s_inode_count; ++i) {//se cambia numero de inodos
            string bit = bitinodos[i] >'0'?"1":"0";

            if(contadorinodo==0){
                flag_salto=true;
                fs << "<tr>";
                fs << "<td> "+bit+" </td>";
            }else{
                fs << "<td> "+bit+" </td>";
            }
            ++contadorinodo;
            if(contadorinodo==20){
                contadorinodo=0;
                fs << "</tr>"<<endl;
                flag_salto=false;
            }
        }
        if(flag_salto){
            fs << "</tr>"<<endl;
        }
        fs << "</table>"<<endl;
        fs << ">];"<<endl;
        fs << "}"<<endl;
        fclose(file);
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

void repBitmapBlock(char _id[],char _name[],char _path[],char _dir[]){
    string dir = charToStringR(_dir);
    string name_dot = dir+charToStringR(_name)+".dot";

    bool existedot = validacionDot(name_dot);
    DISCO disco = buscarDisco(_id);
    bool disk_or = validacionPathMount(disco.path);

    char pathnew[100]="";
    if(disk_or){
        strcpy(pathnew,disco.path);
    }
    if(!existedot){
        string graphsuper;

        //se lee
        FILE *file;
        file = fopen(pathnew,"rb+");
        MBR mbr;
        fseek(file,0,SEEK_SET);
        fread(&mbr,sizeof (MBR),1,file);
        int indice=0;
        string nombre_buscar;
        for (int i = 0; i < 4; ++i) {
            if(strcmp(disco.mbr_particion[i].id,_id)==0){
                nombre_buscar = charToStringR(disco.mbr_particion[i].part_name);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if(strcmp(mbr.mbr_particion[i].part_name,nombre_buscar.c_str())==0){
                indice=i;
                break;
            }
        }
        //se leer super
        SUPER_BLOQUE auxsuper;
        fseek(file,mbr.mbr_particion[indice].part_start+sizeof (mbr.mbr_particion[indice])+1,SEEK_SET);
        fread(&auxsuper,sizeof (SUPER_BLOQUE),1,file);
        //se arma la estructura
        //char numerobloques = auxsuper.s_block_count;
        char bitbloque[auxsuper.s_block_count];
        fseek(file,auxsuper.s_bm_block_start,SEEK_SET);
        fread(&bitbloque,sizeof(char),1,file);

        ofstream fs (name_dot);
        fs << "digraph H {"<<endl;
        fs << "TableBitInodo ["<<endl;
        fs << "shape=plaintext"<<endl;
        fs << "color=red"<<endl;
        fs << "label=<"<<endl;
        fs << "<table border='1' cellborder='0'>"<<endl;
        int contadorbloque=0;
        bool flag_salto = true;
        for (int i = 0; i < auxsuper.s_block_count; ++i) {
            string bit = bitbloque[i] >'0'?"1":"0";

            if(contadorbloque==0){
                flag_salto = true;
                fs << "<tr>";
                //fs << "<td> "+bit+" B"+to_string(i+1)+" </td>";
                fs << "<td> "+bit+" </td>";
            }else{
                //fs << "<td> "+bit+" B"+to_string(i+1)+" </td>";
                fs << "<td> "+bit+" </td>";
            }
            ++contadorbloque;
            if(contadorbloque==20){
                contadorbloque=0;
                fs << "</tr>"<<endl;
                flag_salto = false;
            }
        }
        if(flag_salto){
            fs << "</tr>"<<endl;
        }
        fs << "</table>"<<endl;
        fs << ">];"<<endl;
        fs << "}"<<endl;
        fclose(file);
        fs.close();
        string  rutajpg = charToStringM(_path);
        string render= "dot -Tjpg "+name_dot+" -o "+rutajpg;
        system(render.c_str());
    }else{
        cout << "Error -> Ya existe un .dot con ese nombre"<<endl;
    }
}

void analisisRep(char comando[]){
    char lineacomando[100]="";
    char valor_id[10]="";
    char valor_path[100]="";
    char valor_name[20]="";
    int contador=0;
    //tipos de reportes
    bool flag_mbr=false;
    bool flag_disk=false;
    bool flag_block=false;
    bool flag_bm_block=false;
    bool flag_bm_inode=false;
    bool flag_inode=false;
    bool flag_journaling=false;
    bool flag_tree=false;
    bool flag_sb=false;
    bool flag_file=false;

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

        //validacion tipo de reportes
        if(strcmp(lineacomando,"rep")==0){
            cout << "Comando: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            contador++;
        }else if(strcmp(lineacomando,">name=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);

            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = tolower(comando[contador]);
                    strncat(valor_name, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: "<<valor_name<<endl;
        }else if(strcmp(lineacomando,">id=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);
            while (comando[contador] != NULL) {
                if (comando[contador] == ' ' || comando[contador] == '\n') {
                    contador++;
                    break;
                } else {
                    char aux[1] = "";
                    aux[0] = comando[contador];//quito el tolower
                    strncat(valor_id, aux, 1);
                    contador++;
                }
            }
            cout << "Valor: "<<valor_id<<endl;

        }else if(strcmp(lineacomando,">path=")==0){
            cout << "Argumento: "<<lineacomando<<endl;
            memset(lineacomando,0,100);

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

    //--------------------------------validacion argumentos de rep---------------------------------------------
    //validacion directorio
    int cont_barra1=0;
    int cont_1=0;
    char direc[100]="";
    //busca donde está el último / para guardar
    while(valor_path[cont_1]!=NULL){
        if(valor_path[cont_1]=='/'){
            cont_barra1++;
            cont_1++;
        }
        cont_1++;
    }
    //empieza a generar solo el directorio con el valor anteriro de cont_barra1
    int cont_barra2=0;
    int cont_2=0;
    while(valor_path[cont_2]!=NULL){
        if(valor_path[cont_2]=='/'){
            char aux[1]="";
            aux[0] = valor_path[cont_2];
            strncat(direc,aux,1);
            cont_barra2++;
            cont_2++;
        }
        if(cont_barra1==cont_barra2){
            break;
        }else{
            char aux[1]="";
            aux[0] = valor_path[cont_2];
            strncat(direc,aux,1);
            cont_2++;
        }
    }

    char nombre_rep[20]="";
    while(valor_path[cont_2]!=NULL){
        if(valor_path[cont_2]=='.'){
            break;
        }else{
            char aux[1]="";
            aux[0] = valor_path[cont_2];
            strncat(nombre_rep,aux,1);
            cont_2++;
        }
    }
    cout << "NOmbre del reporte: "<<nombre_rep<<endl;
    cout << "NOmbre del DIrectorio: "<<direc<<endl;

    //obtengo el nombre del reporte
    bool flag_directorio = validacionDirectorioReporte(direc);
    bool flag_rep = validacionReporte(valor_path);

    //tipo de reporte de generar

    bool flag_generador=true;
    if(strcmp(valor_name,"mbr")==0){
        flag_mbr = true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"disk")==0){
        flag_disk = true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"block")==0){
        flag_block = true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"bm_block")==0){
        flag_bm_block=true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"bm_inode")==0){
        flag_bm_inode = true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"inode")==0){
        flag_inode=true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"journaling")==0){
        flag_journaling=true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"tree")==0){
        flag_tree=true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"sb")==0){
        flag_sb=true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else if(strcmp(valor_name,"file")==0){
        flag_file = true;
        cout << "Tipo de reporte si existe: "<<valor_name<<endl;
    }else{
        flag_generador=false;
        cout << "Error -> El reporte solicitado es incorrecto "<<valor_name<<endl;
    }
    //------------------------------------------------flag. paara generar reporte---------------------------------------
    if(flag_generador){
        if(flag_directorio){//existe directorio
            if(flag_rep){//existe rep
                cout << "Error -> Ya existe ese reporte con el mismo nombre"<<endl;
            }
            else{//no existe rep
                cout << "Aviso -> Se genera reporte "<<valor_name<<endl;
                if(flag_mbr){
                    repMbr(valor_id,nombre_rep,valor_path,direc);
                }else if(flag_disk){
                    repDisk(valor_id,nombre_rep,valor_path,direc);
                }else if(flag_tree){
                    //se genera rep del arbol
                    repTree(valor_id,nombre_rep,valor_path,direc);
                }else if(flag_inode){//reporte de inodos en uso
                    repInodos(valor_id,nombre_rep,valor_path,direc);

                }else if(flag_block){//para el reporte de bloques
                    repBlock(valor_id,nombre_rep,valor_path,direc);
                }else if(flag_journaling){//reporta para genenara journaling
                    repJournaling(valor_id,nombre_rep,valor_path,direc);
                }else if(flag_sb) {//para superbloque
                    repSuper(valor_id,nombre_rep,valor_path,direc);

                }else if(flag_bm_inode){//bitmap para inodo
                    repBitmapInode(valor_id,nombre_rep,valor_path,direc);

                }else if(flag_bm_block){//bitmal para bloques
                    repBitmapBlock(valor_id,nombre_rep,valor_path,direc);
                }
            }
        }else{// no existe directorio
            //se crea directorio
            cout << "Aviso -> No existe directorio para generar rep, pero se crea"<<endl;
            string path = charToStringR(direc);
            string dir = "mkdir -p \""+path+"\"";
            system(dir.c_str());
            cout << "Aviso -> Se genera reporte "<<valor_name<<endl;
            if(flag_mbr){
                repMbr(valor_id,nombre_rep,valor_path,direc);
            }else if(flag_disk){
                repDisk(valor_id,nombre_rep,valor_path,direc);
            }else if(flag_tree){
                //se genera rep del arbol
                repTree(valor_id,nombre_rep,valor_path,direc);
            }else if(flag_inode){//reporte de inodos en uso
                repInodos(valor_id,nombre_rep,valor_path,direc);

            }else if(flag_block){//para el reporte de bloques
                repBlock(valor_id,nombre_rep,valor_path,direc);
            }else if(flag_journaling){//reporta para genenara journaling
                repJournaling(valor_id,nombre_rep,valor_path,direc);
            }else if(flag_sb) {//para superbloque
                repSuper(valor_id,nombre_rep,valor_path,direc);

            }else if(flag_bm_inode){//bitmap para inodo
                repBitmapInode(valor_id,nombre_rep,valor_path,direc);

            }else if(flag_bm_block){//bitmal para bloques
                repBitmapBlock(valor_id,nombre_rep,valor_path,direc);
            }
        }
    }
}