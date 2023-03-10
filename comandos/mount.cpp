//
// Created by gerson on 22/02/23.
//
#include "mount.h"

DISCO disco[99];

char abdecedario[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
int contador_abc = 0;
int contador_disco = 0;
vector<string> nodos;

// buscamos un disco como struct
DISCO buscarDisco(char _id[], int sys)
{
    DISCO nulo;
    nulo.mbr_tamano = 0;
    // recorro todos los discos montados
    for (int i = 0; i < 99; ++i)
    {
        if (disco[i].mbr_tamano != 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (strcmp(disco[i].mbr_particion[j].id, _id) == 0)
                {
                    strcpy(disco[i].mbr_particion[j].formato,"full");
                    disco[i].mbr_particion[j].sys = sys;
                    return disco[i];
                }
            }
            for (int j = 0; j < 24; ++j)
            {
                if (strcmp(disco[i].ebr_logicas[j].id, _id) == 0)
                {
                    strcpy(disco[i].ebr_logicas[j].formato,"full");
                    disco[i].ebr_logicas[j].sys = sys;
                    return disco[i];
                }
            }
        }
    }
    return nulo;
}

// funcions para saber si existe la particion ya montado buscarDiscoPath
bool buscarDiscoPath(char _particion[], char _path[])
{
    // recorremos los discos montados
    for (int i = 0; i < 99; ++i)
    {
        if (disco[i].mbr_tamano != 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                if ((strcmp(disco[i].mbr_particion[j].part_name, _particion) == 0) && (strcmp(disco[i].path, _particion) == 0) && (strcmp(disco[i].mbr_particion[j].id, "") != 0))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// aqui se añadiran las particiones de los discos cuando hayan cambios
void agregarParticionMount(char _path[], char _name[], int _size, char _type, char _fit[], int _start, int _next)
{
    for (int i = 0; i < 99; ++i)
    { // primero buscar el path del disco y si ya esta montado
        if (strcmp(disco[i].path, _path) == 0)
        {
            if (_type == 'p')
            { // particion primaria
                for (int j = 0; j < 4; ++j)
                {
                    if (disco[i].mbr_particion[j].part_type == '\000')
                    {
                        disco[i].mbr_particion[j].part_type = _type;
                        disco[i].mbr_particion[j].part_status = '1';
                        disco[i].mbr_particion[j].part_size = _size;
                        disco[i].mbr_particion[j].part_start = _start;
                        strcpy(disco[i].mbr_particion[j].part_name, _name);
                        strcpy(disco[i].mbr_particion[j].part_fit, _fit);
                        break;
                    }
                }
            }
            else if (_type == 'e')
            { // particion extendida
                for (int j = 0; j < 4; ++j)
                {
                    if (disco[i].mbr_particion[j].part_type == '\000')
                    {
                        disco[i].mbr_particion[j].part_type = _type;
                        disco[i].mbr_particion[j].part_status = '1';
                        disco[i].mbr_particion[j].part_size = _size;
                        disco[i].mbr_particion[j].part_start = _start;
                        strcpy(disco[i].mbr_particion[j].part_name, _name);
                        strcpy(disco[i].mbr_particion[j].part_fit, _fit);
                        break;
                    }
                }
            }
            else if (_type == 'l')
            { // particion logica
                for (int j = 0; j < 24; ++j)
                {
                    if (disco[i].ebr_logicas[j].part_size != 0)
                    {
                        disco[i].ebr_logicas[j].part_size = _size;
                        disco[i].ebr_logicas[j].part_start = _start;
                        disco[i].ebr_logicas[j].part_next = _next;
                        strcpy(disco[i].ebr_logicas[j].part_fit, _fit);
                        strcpy(disco[i].ebr_logicas[j].part_name, _name);
                        break;
                    }
                }
            }
        }
    }
}

// metodo cuando se elimina una particion y poder borrarla de los discos montados
void elimininarParticionMount(char _path[], char _name[])
{
    // recorremos en los discos montados
    for (int i = 0; i < 99; ++i)
    {
        if (strcmp(disco[i].path, _path) == 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (strcmp(disco[i].mbr_particion[j].part_name, _name) == 0)
                {
                    disco[i].mbr_particion[j].part_type = '\000';
                    disco[i].mbr_particion[j].part_size = 0;
                    disco[i].mbr_particion[j].part_start = 0;
                    memset(disco[i].mbr_particion[j].part_fit, 0, 3);
                    memset(disco[i].mbr_particion[j].part_name, 0, 16);
                    memset(disco[i].mbr_particion[j].id, 0, 10);
                    break;
                }
                if (disco[i].mbr_particion[j].part_type == 'e')
                {
                    // buscamos en las particiones logicas
                    for (int k = 0; k < 24; ++k)
                    {
                        if (strcmp(disco[i].ebr_logicas[k].part_name, _name) == 0)
                        {
                            disco[i].ebr_logicas[k].part_size = 0;
                            disco[i].ebr_logicas[k].part_next = 0;
                            disco[i].ebr_logicas[k].part_start = 0;
                            memset(disco[i].ebr_logicas[k].part_fit, 0, 3);
                            memset(disco[i].ebr_logicas[k].part_name, 0, 16);
                            memset(disco[i].ebr_logicas[k].id, 0, 10);
                            break;
                        }
                    }
                }
            }
        }
    }
}

// funcion para desmontar una particion
void desmontarMount(char _id[])
{
    bool seDesmonto = false;
    for (int i = 0; i < 99; ++i)
    { // buscamos en la lista de montados
        if (disco[i].mbr_tamano != 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (strcmp(disco[i].mbr_particion[j].id, _id) == 0)
                { // buscamos el id a desmontar
                    memset(disco[i].mbr_particion[j].id, 0, 10);
                    cout << "sistema~> se ha desmontado con exito la siguiente particion con id: " << _id << endl;
                    seDesmonto = true;
                    break;
                }

                if (disco[i].mbr_particion[j].part_type == 'e')
                { // si es extendida y nombre no es igual se busca en logicas
                    for (int k = 0; k < 24; ++k)
                    {
                        if (strcmp(disco[i].ebr_logicas[k].id, _id) == 0)
                        {
                            memset(disco[i].ebr_logicas[k].id, 0, 10);
                            cout << "sistema~> se ha desmontado con exito la siguiente particion con id: " << _id << endl;
                            seDesmonto = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // para validar error
    if (!seDesmonto)
    {
        cout << "Error~> No se ha podido desmontar la particion con id: " << _id << ". Por favor verifique el id." << endl;
        return;
    }
}

// obtener el nombre del path
string getName(char path[])
{
    int contadorDia = 0; // contador de diagonales
    int contadorFind = 0;
    int contador = 0;
    char name[100] = "";
    while (path[contador] != NULL)
    { // cuento las diagonales en el path
        if (path[contador] == '/')
        {
            contadorDia++;
        }
        contador++;
    }
    contador = 0;
    while (path[contador] != NULL)
    {
        if (path[contador] == '/')
        {
            contadorFind++;
        }
        if (contadorDia == contadorFind)
        {
            if (path[contador] != '/' and path[contador] != '.')
            {
                char aux[1] = "";
                aux[0] = path[contador];
                strncat(name, aux, 1);
            }
            else if (path[contador] == '.')
            {
                return charToString(name);
            }
        }
        contador++;
    }
    return charToString(name);
}

// funcion para montar una particion
void montajeMount(char _path[], char _name[])
{
    char tipoParticion;
    bool existeParticion = false;
    bool existeDisco = false;
    char id[50] = ""; // id que se genera
    // abrimos el disco (el archivo que representa)
    FILE *file;
    file = fopen(_path, "rb+");
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    EBR *ebr = (EBR *)malloc(sizeof(EBR));
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file);

    // para validar particion
    int cont_Part = 0;
    int nextPos = 0;
    int startPos = 0;

    // buscar en particiones primarias en el mbr escrito en disco
    for (cont_Part = 0; cont_Part < 4; ++cont_Part)
    {   // buscamos en el mbr la particion a montar, con el nombre
        if (strcmp(mbr->mbr_particion[cont_Part].part_name, _name) == 0)
        {   // se busca el nombre de la particion a montar
            if (mbr->mbr_particion[cont_Part].part_type == 'p')
            {   // particion primaria a montar
                tipoParticion = 'p';
                existeParticion = true;
                break;
            }
            else if (mbr->mbr_particion[cont_Part].part_type == 'e')
            {   // particion extendida a montar
                tipoParticion = 'e';
                existeParticion = true;
                break;
            }
        }
        // para buscar en las particiones logicas
        if (mbr->mbr_particion[cont_Part].part_type == 'e')
        {
            int pos = mbr->mbr_particion[cont_Part].part_start + sizeof(EBR) + 1;
            fseek(file, pos, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, file);
            nextPos = ebr->part_next;
            while (nextPos != -1)
            {
                if (strcmp(ebr->part_name, _name) == 0)
                {   //particion logica a montar
                    existeParticion = true;
                    startPos = ebr->part_start;
                    tipoParticion = 'l';
                    break;
                }
                fseek(file, nextPos, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                nextPos = ebr->part_next;
            }
        }
    }

    // para validar path
    int cont_disk = 0;
    string ruta = charToString(_path);
    for (cont_disk = 0; cont_disk < 99; ++cont_disk)
    {   // validamos si el disco ya fue montado
        if (strcmp(disco[cont_disk].path, ruta.c_str()) == 0)
        {
            existeDisco = true;
            break;
        }
    }
    if (existeParticion)
    {   // si existe nombre particion
        if (existeDisco)
        {   // ya se ha ingresado una particion del mismo disco
            if (tipoParticion == 'p')
            {
                // Esto es para obtener el nombre
                char nameChar[16] = "";
                string nombreSTR = getName(_path);
                strcpy(nameChar, nombreSTR.c_str());
                for (int i = 0; i < 4; ++i)
                {
                    if (strcmp(disco[cont_disk].mbr_particion[i].part_name, _name) == 0)
                    {
                        // id
                        id[0] = '6';
                        id[1] = '6';
                        disco[cont_disk].numeral++;
                        string numSTR = to_string(disco[cont_disk].numeral);
                        char const *num_char = numSTR.c_str();
                        strncat(id, num_char, 1); // aqui le agrego el numero de disco
                        // ponemos el nombre del disco
                        int contadorP = 0;
                        while (nameChar[contadorP] != NULL)
                        {
                            char aux[1] = "";
                            aux[0] = nameChar[contadorP];
                            strncat(id, aux, 1);
                            contadorP++;
                        }
                        // aqui le meto de una el ID
                        strcpy(disco[cont_disk].mbr_particion[cont_Part].id, id);
                        cout << "sistema~>  el id generado para la particion montada es:  " << disco[cont_disk].mbr_particion[cont_Part].id << endl;
                        memset(id, 0, 50);
                        cout << "sistema~> la particion primaria ha sido montada con exito. Particion: " << _name << endl;
                        break;
                    }
                }
                fclose(file);
            }
            else if (tipoParticion == 'e')
            {
                // Esto es para obtener el nombre del path
                char nameChar[16] = "";
                string nameSTR = getName(_path);
                strcpy(nameChar, nameSTR.c_str());
                // copiamos las propiedades del disco en las particiones montadas
                char letra = disco[cont_disk].letra;
                disco[cont_disk].mbr_particion[cont_Part].part_status = mbr->mbr_particion[cont_Part].part_status;
                disco[cont_disk].mbr_particion[cont_Part].part_type = mbr->mbr_particion[cont_Part].part_type;
                disco[cont_disk].mbr_particion[cont_Part].part_size = mbr->mbr_particion[cont_Part].part_s;
                disco[cont_disk].mbr_particion[cont_Part].part_start = mbr->mbr_particion[cont_Part].part_start;
                strcpy(disco[cont_disk].mbr_particion[cont_Part].part_name, mbr->mbr_particion[cont_Part].part_name);
                strcpy(disco[cont_disk].mbr_particion[cont_Part].part_fit, mbr->mbr_particion[cont_Part].part_fit);
                // generando id
                id[0] = '6';
                id[1] = '6';
                disco[cont_disk].numeral++;
                string numSTR = to_string(disco[cont_disk].numeral);
                char const *num_char = numSTR.c_str();
                strncat(id, num_char, 1);
                // concatenamos el nombre del disco
                int contadorP = 0;
                while (nameChar[contadorP] != NULL)
                {
                    char aux[1] = "";
                    aux[0] = nameChar[contadorP];
                    strncat(id, aux, 1);
                    contadorP++;
                }
                strcpy(disco[cont_disk].mbr_particion[cont_Part].id, id);
                cout << "sistema~>  el id generado para la particion montada es:  " << disco[cont_disk].mbr_particion[cont_Part].id << endl;
                memset(id, 0, 50);

                EBR *_ebr = (EBR *)malloc(sizeof(EBR));
                // se guardan las logicas
                fseek(file, mbr->mbr_particion[cont_Part].part_start + sizeof(EBR) + 1, SEEK_SET);
                fread(_ebr, sizeof(EBR), 1, file);
                int pos = 0;
                int cont_logicas = 0;
                while (_ebr->part_next != -1)
                {
                    pos = _ebr->part_next;
                    disco[cont_disk].ebr_logicas[cont_logicas].part_next = _ebr->part_next;
                    disco[cont_disk].ebr_logicas[cont_logicas].part_start = _ebr->part_start;
                    disco[cont_disk].ebr_logicas[cont_logicas].part_size = _ebr->part_s;
                    strcpy(disco[cont_disk].ebr_logicas[cont_logicas].part_fit, _ebr->part_fit);
                    strcpy(disco[cont_disk].ebr_logicas[cont_logicas].part_name, _ebr->part_name);
                    fseek(file, pos, SEEK_SET);
                    fread(_ebr, sizeof(EBR), 1, file);
                    cont_logicas++;
                }
                cout << "sistema~> la particion extendida ha sido montada con exito. Particion: " << _name << endl;
            }
            else if (tipoParticion == 'l')
            {

                // Esto es para obtener el nombre
                char nameChar[16] = "";
                string nameSTR = getName(_path);
                strcpy(nameChar, nameSTR.c_str());
                char letra = disco[cont_disk].letra;
                for (int i = 0; i < 4; ++i)
                {   // como es logica buscamos solo en la particion extendida
                    if (disco[cont_disk].mbr_particion[i].part_type == 'e')
                    {
                        for (int j = 0; j < 24; ++j)
                        {
                            if (strcmp(disco[cont_disk].ebr_logicas[j].part_name, _name) == 0)
                            {
                                id[0] = '6';
                                id[1] = '6';
                                disco[cont_disk].numeral++;
                                string numSTR = to_string(disco[cont_disk].numeral);
                                char const *num_char = numSTR.c_str();
                                strncat(id, num_char, 1);
                                // concatenamos el nombre del disco
                                int contadorP = 0;
                                while (nameChar[contadorP] != NULL)
                                {
                                    char aux[1] = "";
                                    aux[0] = nameChar[contadorP];
                                    strncat(id, aux, 1);
                                    contadorP++;
                                }

                                strcpy(disco[cont_disk].ebr_logicas[j].id, id);
                                cout << "sistema~>  el id generado para la particion montada es:  " << disco[cont_disk].ebr_logicas[j].id << endl;
                                memset(id, 0, 50);
                                cout << "sistema~> la particion logica ha sido montada con exito. Particion: " << _name << endl;
                                break;
                            }
                        }
                    }
                }
                fclose(file);
            }
        }
        else
        { // disco es nuevo, primera vez que se ingresa en las particiones montadas
            if (tipoParticion == 'p')
            {   // Esto es para obtener el nombre
                char nameChar[16] = "";
                string nameSTR = getName(_path);
                strcpy(nameChar, nameSTR.c_str());
                // pasamos los datos al arreglo de los discos
                disco[contador_disco].letra = abdecedario[contador_abc];
                disco[contador_disco].mbr_disk_signature = mbr->mbr_dsk_signature;
                disco[contador_disco].mbr_tamano = mbr->mbr_tamano;
                disco[contador_disco].mbr_fecha_creacion = mbr->mbr_fecha_creacion;
                // obtenemos el path en string
                string ruta = charToString(_path);
                strcpy(disco[contador_disco].path, ruta.c_str());
                for (int i = 0; i < 4; ++i)
                {
                    if (mbr->mbr_particion[i].part_s != 0)
                    {   // solo se agregan las que tengan tamaño
                        if (strcmp(mbr->mbr_particion[i].part_name, _name) == 0)
                        {   // agregamos el disco a las particiones montadas
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                            // generando id
                            id[0] = '6';
                            id[1] = '6';
                            // concatenamos el resto del id
                            disco[contador_disco].numeral++;
                            string numSTR = to_string(disco[contador_disco].numeral);
                            char const *num_char = numSTR.c_str();
                            strncat(id, num_char, 1);
                            // concatenamos el nombre del disco
                            int contadorP = 0;
                            while (nameChar[contadorP] != NULL)
                            {
                                char aux[1] = "";
                                aux[0] = nameChar[contadorP];
                                strncat(id, aux, 1);
                                contadorP++;
                            }
                            strcpy(disco[contador_disco].mbr_particion[i].id, id);
                            cout << "sistema~>  el id generado para la particion montada es: " << disco[contador_disco].mbr_particion[i].id << endl;
                            memset(id, 0, 50);
                            cout << "sistema~> la particion primaria ha sido montada con exito. Particion: " << _name << endl;
                        }
                        else
                        {   // particiones extendidas/logicas
                            if (mbr->mbr_particion[i].part_type == 'e')
                            {   // para buscar las logicas
                                disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                                disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                                disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                                disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                                strcpy(disco[contador_disco].mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                                strcpy(disco[contador_disco].mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                                // valido si existe logicas
                                EBR *_ebr = (EBR *)malloc(sizeof(EBR));
                                fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                                fread(_ebr, sizeof(EBR), 1, file);
                                int next = _ebr->part_next;
                                int incre = 0;
                                if (_ebr->part_next != 0)
                                { // solo si existe ebr escritas
                                    while (next != -1)
                                    {
                                        disco[contador_disco].ebr_logicas[incre].part_size = _ebr->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = _ebr->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = _ebr->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit, _ebr->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name, _ebr->part_name);
                                        fseek(file, next, SEEK_SET);
                                        fread(_ebr, sizeof(EBR), 1, file);
                                        next = _ebr->part_next;
                                        incre++;
                                    }
                                }
                            }
                            else
                            {   // particiones primarias
                                disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                                disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                                disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                                disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                                strcpy(disco[contador_disco].mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                                strcpy(disco[contador_disco].mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                            }
                        }
                    }
                }
                contador_abc++;   // aqui se aumenta para que cambie a,b,c ,d e
                contador_disco++; // aqui suma el Disco , 1,2,3...
                fclose(file);
            }
            else if (tipoParticion == 'e')
            {   // para particiones extendidas
                char letra = disco[cont_disk].letra;
                // se obtiene el nombre
                char valor_nameE[16] = "";
                string NombreD = getName(_path);
                strcpy(valor_nameE, NombreD.c_str());
                // agregamos el disco al arreglo de particiones montadas
                disco[contador_disco].mbr_particion[cont_Part].part_status = mbr->mbr_particion[cont_Part].part_status;
                disco[contador_disco].mbr_particion[cont_Part].part_type = mbr->mbr_particion[cont_Part].part_type;
                disco[contador_disco].mbr_particion[cont_Part].part_size = mbr->mbr_particion[cont_Part].part_s;
                disco[contador_disco].mbr_particion[cont_Part].part_start = mbr->mbr_particion[cont_Part].part_start;
                strcpy(disco[contador_disco].mbr_particion[cont_Part].part_name, mbr->mbr_particion[cont_Part].part_name);
                strcpy(disco[contador_disco].mbr_particion[cont_Part].part_fit, mbr->mbr_particion[cont_Part].part_fit);
                // generando id
                id[0] = '6';
                id[1] = '6';
                // contatenamos el resto del id
                disco[contador_disco].numeral++;
                string numSTR = to_string(disco[contador_disco].numeral);
                char const *num_char = numSTR.c_str();
                strncat(id, num_char, 1);
                // concatenamos el nombre del disco
                int contadorP = 0;
                while (valor_nameE[contadorP] != NULL)
                {
                    char aux[1] = "";
                    aux[0] = valor_nameE[contadorP];
                    strncat(id, aux, 1);
                    contadorP++;
                }

                strcpy(disco[contador_disco].mbr_particion[cont_Part].id, id);
                cout << "sistema~>  el id generado para la particion montada es: " << disco[contador_disco].mbr_particion[cont_Part].id << endl;
                memset(id, 0, 50);
                EBR *ebr = (EBR *)malloc(sizeof(EBR));
                // se guardan las logicas
                fseek(file, mbr->mbr_particion[cont_Part].part_start + sizeof(EBR) + 1, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                int pos = 0;
                int cont_logicas = 0;
                while (ebr->part_next != -1)
                {
                    pos = ebr->part_next;
                    // guardo ebr
                    disco[contador_disco].ebr_logicas[cont_logicas].part_next = ebr->part_next;
                    disco[contador_disco].ebr_logicas[cont_logicas].part_start = ebr->part_start;
                    disco[contador_disco].ebr_logicas[cont_logicas].part_size = ebr->part_s;
                    strcpy(disco[contador_disco].ebr_logicas[cont_logicas].part_fit, ebr->part_fit);
                    strcpy(disco[contador_disco].ebr_logicas[cont_logicas].part_name, ebr->part_name);
                    fseek(file, pos, SEEK_SET);
                    fread(ebr, sizeof(EBR), 1, file);
                    cont_logicas++;
                }
                contador_abc++;
                contador_disco++;
                cout << "sistema~> la particion extendida ha sido montada con exito. Particion: " << _name << endl;
            }
            else if (tipoParticion == 'l')
            {
                // para particiones logicas
                char nameChar[16] = "";
                string nameSTR = getName(_path);
                strcpy(nameChar, nameSTR.c_str());

                // se copia el disco
                disco[contador_disco].letra = abdecedario[contador_abc];
                disco[contador_disco].mbr_disk_signature = mbr->mbr_dsk_signature;
                disco[contador_disco].mbr_tamano = mbr->mbr_tamano;
                disco[contador_disco].mbr_fecha_creacion = mbr->mbr_fecha_creacion;
                string ruta = charToString(_path);
                strcpy(disco[contador_disco].path, ruta.c_str());
                for (int i = 0; i < 4; ++i)
                {
                    if (mbr->mbr_particion[i].part_s != 0)
                    {   // buscamos solo en las particiones extendidas ya que es una partcion logica la que montaremos
                        if (mbr->mbr_particion[i].part_type == 'e')
                        {
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                            // valido si existe logicas
                            EBR *_ebr = (EBR *)malloc(sizeof(EBR));
                            fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                            fread(_ebr, sizeof(EBR), 1, file);
                            int next = _ebr->part_next;
                            int incre = 0;
                            if (_ebr->part_next != 0)
                            {   // loopeamos hasta encontrar el ebr
                                while (next != -1)
                                {   // buscamos la particion por medio del nombre
                                    if (strcmp(_ebr->part_name, _name) == 0)
                                    {
                                        disco[contador_disco].ebr_logicas[incre].part_size = _ebr->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = _ebr->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = _ebr->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit, _ebr->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name, _ebr->part_name);
                                        // generar id
                                        id[0] = '6';
                                        id[1] = '6';
                                        // concatenamos el resto del id
                                        disco[contador_disco].numeral++;
                                        string numSTR = to_string(disco[contador_disco].numeral);
                                        char const *num_char = numSTR.c_str();
                                        strncat(id, num_char, 1);
                                        // concatenamos el nombre del disco
                                        int contadorP = 0;
                                        while (nameChar[contadorP] != NULL)
                                        {
                                            char aux[1] = "";
                                            aux[0] = nameChar[contadorP];
                                            strncat(id, aux, 1);
                                            contadorP++;
                                        }
                                        strcpy(disco[contador_disco].ebr_logicas[incre].id, id);
                                        cout << "sistema~> el id generado para la particion montada es: " << disco[contador_disco].ebr_logicas[incre].id << endl;
                                        memset(id, 0, 50);
                                        cout << "sistema~> la particion logica ha sido montada con exito. Particion: " << _name << endl;
                                        // leer siguiente
                                        fseek(file, _ebr->part_next, SEEK_SET);
                                        fread(_ebr, sizeof(EBR), 1, file);
                                        next = _ebr->part_next;
                                        incre++;
                                    }
                                    else
                                    {
                                        disco[contador_disco].ebr_logicas[incre].part_size = _ebr->part_s;
                                        disco[contador_disco].ebr_logicas[incre].part_next = _ebr->part_next;
                                        disco[contador_disco].ebr_logicas[incre].part_start = _ebr->part_start;
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_fit, _ebr->part_fit);
                                        strcpy(disco[contador_disco].ebr_logicas[incre].part_name, _ebr->part_name);
                                        fseek(file, next, SEEK_SET);
                                        fread(_ebr, sizeof(EBR), 1, file);
                                        next = _ebr->part_next;
                                        incre++;
                                    }
                                }
                            }
                        }
                        else
                        {
                            disco[contador_disco].mbr_particion[i].part_start = mbr->mbr_particion[i].part_start;
                            disco[contador_disco].mbr_particion[i].part_size = mbr->mbr_particion[i].part_s;
                            disco[contador_disco].mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                            disco[contador_disco].mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                            strcpy(disco[contador_disco].mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                            strcpy(disco[contador_disco].mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                        }
                    }
                }
                contador_disco++;
                contador_abc++;
                fclose(file);
            }
        }
    }
    else
    {
        cout << "Error~> No se ha encontrado ninguna particion. Compruebe el nombre." << endl;
        return;
    }
}

// funcion para ver las particiones montadas
void showMount()
{
    cout << "╔═══════════════════════════════════════════════════╗" << endl;
    cout << "║           LISTA DE PARTICIONES MONTADAS           ║" << endl;
    cout << "╠═════════════════╦════════════╦════════════════════╣" << endl;
    cout << "║     NOMBRE      ║     ID     ║         TIPO       ║" << endl;
    cout << "╚═════════════════╩════════════╩════════════════════╝" << endl;
    for (int i = 0; i < 99; ++i)
    {   // recorremos el arreglo de las particioens montadas
        if (disco[i].letra != '\000')
        {
            for (int j = 0; j < 4; ++j)
            {
                if (disco[i].mbr_particion[j].part_type == 'p')
                {   // si es primaria y esta activa procedemos a imprimir en pantalla
                    if (disco[i].mbr_particion[j].part_status == '1' && disco[i].mbr_particion[j].id[0] != '\000')
                    {
                        cout << "║ " << disco[i].mbr_particion[j].part_name << "      ║  " << disco[i].mbr_particion[j].id << " ║  " << disco[i].mbr_particion[j].part_type << "                 ║" << endl;
                    }
                }
                if (disco[i].mbr_particion[j].part_type == 'e')
                {   // si es extendida y esta activa procedemos a imprimir en pantalla
                    if (disco[i].mbr_particion[j].part_status == '1')
                    {
                        for (int k = 0; k < 24; ++k)
                        {
                            if (disco[i].ebr_logicas[k].part_size != 0 && disco[i].ebr_logicas[k].id[0] != '\000')
                            {
                                cout << "║ " << disco[i].ebr_logicas[k].part_name << "      ║  " << disco[i].ebr_logicas[k].id << " ║  "
                                     << "l"
                                     << "                 ║" << endl;
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "" << endl;
    cout << "" << endl;
}

// funciones para hacer los reportes graficos
bool validacionDotM(string _path)
{
    FILE *file;
    if (file = fopen(_path.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    return false;
}

void repMbr(char _id[], char _namerep[], char _path[], char _dir[])
{
    bool flag_rep = false;
    string dir = charToString(_dir);
    string name_dot = dir + charToString(_namerep) + ".dot";
    string name_svg = dir + charToString(_namerep) + ".svg";
    bool existedot = validacionDotM(name_dot); // VALIDO EL DOT y no la imagen ya que la imagen no es .png

    if (existedot)
    {
        cout << "Error~> Ya existe un reporte con ese nombre. Intente con otro nuevo. Nombre: " << name_dot << endl;
        return;
    }
    for (int i = 0; i < 99; ++i)
    {
        if (disco[i].mbr_tamano != 0)
        {   // solo buscamos en los discos que tengan una tamaño, eso quiere decri que estan montados
            for (int j = 0; j < 4; ++j)
            {
                if (strcmp(disco[i].mbr_particion[j].id, _id) == 0)
                {
                    // se realiza reporte mbr
                    flag_rep = true;
                    char *tt = ctime(&disco[i].mbr_fecha_creacion);
                    string time = charToString(tt);
                    ofstream fs(name_dot);
                    fs << "digraph G {" << endl;
                    fs << "rankdir=LR;" << endl;
                    fs << "mbr[" << endl;
                    fs << "shape=plaintext " << endl;
                    fs << "label=<" << endl;
                    fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>" << endl;
                    fs << "<tr><td>MBR</td></tr>" << endl;
                    fs << "<tr><td cellpadding='4'>" << endl;
                    fs << "<table color='orange' cellspacing='0'>" << endl;
                    fs << "<tr><td> MBR  </td></tr>" << endl;
                    fs << "<tr><td> Mbr_tamaño: " + to_string(disco[i].mbr_tamano) + "</td></tr>" << endl;
                    fs << "<tr><td> Mbr_fecha_creacion: " + time + "</td></tr>" << endl;
                    fs << "<tr><td> Mbr_signature: " + to_string(disco[i].mbr_disk_signature) + "</td></tr>" << endl;
                    fs << "</table>" << endl;
                    fs << "</td>" << endl;
                    fs << "</tr>" << endl;
                    fs << "\n"
                       << endl;
                    // para los 4 basicos
                    for (int k = 0; k < 4; ++k)
                    {
                        if (disco[i].mbr_particion[k].part_size != 0)
                        {
                            string s;
                            s += disco[i].mbr_particion[k].part_status;
                            string t;
                            t += disco[i].mbr_particion[k].part_type;
                            fs << "<tr><td cellpadding='4'>" << endl;
                            fs << "<table color='orange' cellspacing='0'>" << endl;
                            fs << " <tr><td> PARTICION  </td></tr>" << endl;
                            fs << "<tr><td> Nombre: " + charToString(disco[i].mbr_particion[k].part_name) + "</td></tr>" << endl;
                            fs << "<tr><td> Status: " + s + "</td></tr>" << endl;
                            fs << "<tr><td> Tipo: " + t + "</td></tr>" << endl;
                            fs << "<tr><td> Fit: " + charToString(disco[i].mbr_particion[k].part_fit) + "</td></tr>" << endl;
                            fs << "<tr><td> Start: " + to_string(disco[i].mbr_particion[k].part_start) + "</td></tr>" << endl;
                            fs << "<tr><td> Size: " + to_string(disco[i].mbr_particion[k].part_size) + "</td></tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                            fs << "</tr>" << endl;
                            fs << "\n"
                               << endl;
                        }
                    }
                    fs << "</table>" << endl;
                    fs << ">];" << endl;
                    // buscamos las particiones logicas
                    for (int k = 0; k < 24; ++k)
                    {
                        if (disco[i].ebr_logicas[k].part_size != 0)
                        {
                            nodos.push_back("ebr" + to_string(k));
                            fs << "ebr" + to_string(k) + " [" << endl;
                            fs << "shape=plaintext" << endl;
                            fs << "label=<" << endl;
                            fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>" << endl;
                            fs << "<tr><td>ebr</td></tr>" << endl;
                            fs << "<tr><td cellpadding='4'>" << endl;
                            fs << "<table color='orange' cellspacing='0'>" << endl;
                            fs << "<tr><td> Nombre: " + charToString(disco[i].ebr_logicas[k].part_name) + " </td></tr>" << endl;
                            fs << "<tr><td> Fit: " + charToString(disco[i].ebr_logicas[k].part_fit) + " </td></tr>" << endl;
                            fs << "<tr><td> Start: " + to_string(disco[i].ebr_logicas[k].part_start) + " </td></tr>" << endl;
                            fs << "<tr><td> Size: " + to_string(disco[i].ebr_logicas[k].part_size) + " </td></tr>" << endl;
                            fs << "<tr><td> Next: " + to_string(disco[i].ebr_logicas[k].part_next) + " </td></tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                            fs << "</tr>" << endl;
                            fs << "</table>" << endl;
                            fs << ">];\n"
                               << endl;
                            fs << "\n"
                               << endl;
                        }
                    }
                    int tam_vec = nodos.size();
                    if (tam_vec >= 1)
                    {
                        for (int k = 0; k < tam_vec - 1; ++k)
                        {
                            fs << nodos[k] + " -> " + nodos[k + 1] + "\n"
                               << endl;
                        }
                    }
                    fs << "}\n"
                       << endl;
                    fs.close();
                    string rutajpg = charToString(_path);
                    string render = "dot -Tjpg " + name_dot + " -o " + rutajpg;
                    system(render.c_str());
                    break;
                }
                if (disco[i].mbr_particion[j].part_type == 'e')
                { // si es extendida y nombre no es igual se busca en logicas
                    for (int k = 0; k < 24; ++k)
                    {
                        if (strcmp(disco[i].ebr_logicas[k].id, _id) == 0)
                        {
                            flag_rep = true;
                            ofstream fs(name_dot);
                            char *tt = ctime(&disco[i].mbr_fecha_creacion);
                            string time = charToString(tt);
                            fs << "digraph G {" << endl;
                            fs << "rankdir=LR;" << endl;
                            fs << "mbr[" << endl;
                            fs << "shape=plaintext " << endl;
                            fs << "label=<" << endl;
                            fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>" << endl;
                            fs << "<tr><td>MBR</td></tr>" << endl;
                            fs << "<tr><td cellpadding='4'>" << endl;
                            fs << "<table color='orange' cellspacing='0'>" << endl;
                            fs << "<tr><td> MBR  </td></tr>" << endl;
                            fs << "<tr><td> Mbr_tamaño: " + to_string(disco[i].mbr_tamano) + "</td></tr>" << endl;
                            fs << "<tr><td> Mbr_fecha_creacion: " + time + "</td></tr>" << endl;
                            fs << "<tr><td> Mbr_signature: " + to_string(disco[i].mbr_disk_signature) + "</td></tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                            fs << "</tr>" << endl;
                            fs << "\n"
                               << endl;
                            for (int k = 0; k < 4; ++k)
                            {
                                if (disco[i].mbr_particion[k].part_size != 0)
                                {
                                    string s;
                                    s += disco[i].mbr_particion[k].part_status;
                                    string t;
                                    t += disco[i].mbr_particion[k].part_type;
                                    fs << "<tr><td cellpadding='4'>" << endl;
                                    fs << "<table color='orange' cellspacing='0'>" << endl;
                                    fs << " <tr><td> PARTICION  </td></tr>" << endl;
                                    fs << "<tr><td> Nombre: " + charToString(disco[i].mbr_particion[k].part_name) + "</td></tr>" << endl;
                                    fs << "<tr><td> Status: " + s + "</td></tr>" << endl;
                                    fs << "<tr><td> Tipo: " + t + "</td></tr>" << endl;
                                    fs << "<tr><td> Fit: " + charToString(disco[i].mbr_particion[k].part_fit) + "</td></tr>" << endl;
                                    fs << "<tr><td> Start: " + to_string(disco[i].mbr_particion[k].part_start) + "</td></tr>" << endl;
                                    fs << "<tr><td> Size: " + to_string(disco[i].mbr_particion[k].part_size) + "</td></tr>" << endl;
                                    fs << "</table>" << endl;
                                    fs << "</td>" << endl;
                                    fs << "</tr>" << endl;
                                    fs << "\n"
                                       << endl;
                                }
                            }
                            // finaliza para el mbr
                            fs << "</table>" << endl;
                            fs << ">];" << endl;
                            for (int k = 0; k < 24; ++k)
                            {
                                if (disco[i].ebr_logicas[k].part_size != 0)
                                {
                                    nodos.push_back("ebr" + to_string(k));
                                    fs << "ebr" + to_string(k) + " [" << endl;
                                    fs << "shape=plaintext" << endl;
                                    fs << "label=<" << endl;
                                    fs << "<table border='0' cellborder='1' color='blue' cellspacing='0'>" << endl;
                                    fs << "<tr><td>ebr</td></tr>" << endl;
                                    fs << "<tr><td cellpadding='4'>" << endl;
                                    fs << "<table color='orange' cellspacing='0'>" << endl;
                                    fs << "<tr><td> Nombre: " + charToString(disco[i].ebr_logicas[k].part_name) + " </td></tr>" << endl;
                                    fs << "<tr><td> Fit: " + charToString(disco[i].ebr_logicas[k].part_fit) + " </td></tr>" << endl;
                                    fs << "<tr><td> Start: " + to_string(disco[i].ebr_logicas[k].part_start) + " </td></tr>" << endl;
                                    fs << "<tr><td> Size: " + to_string(disco[i].ebr_logicas[k].part_size) + " </td></tr>" << endl;
                                    fs << "<tr><td> Next: " + to_string(disco[i].ebr_logicas[k].part_next) + " </td></tr>" << endl;
                                    fs << "</table>" << endl;
                                    fs << "</td>" << endl;
                                    fs << "</tr>" << endl;
                                    fs << "</table>" << endl;
                                    fs << ">];\n"
                                       << endl;
                                    fs << "\n"
                                       << endl;
                                }
                            }
                            int tam_vec = nodos.size();
                            if (tam_vec >= 1)
                            {
                                for (int k = 0; k < tam_vec - 1; ++k)
                                {
                                    fs << nodos[k] + " -> " + nodos[k + 1] + "\n"
                                       << endl;
                                }
                            }
                            fs << "}\n"
                               << endl;
                            fs.close();
                            string rutajpg = charToString(_path);
                            string render = "dot -Tjpg " + name_dot + " -o " + rutajpg;
                            system(render.c_str());
                            break;
                        }
                    }
                }
            }
        }
    }
    // para validar error
    if (!flag_rep)
    {
        cout << "Error~> No se ha econtrado la particion con id: " << _id << " por lo que no se ha podido generar el reporte." << endl;
        return;
    }
}

void repDisk(char _id[], char _namerep[], char _path[], char _dir[])
{
    bool flag_rep = false;
    string dir = charToString(_dir);
    string name_dot = dir + charToString(_namerep) + ".dot";
    int suma = 0;
    float espaLibre = 100.0;
    bool existeDot = validacionDotM(name_dot);
    if (existeDot)
    {
        cout << "Error~> Ya existe un reporte con ese nombre. Intente con otro nuevo. Nombre: " << name_dot << endl;
        return;
    }
    for (int i = 0; i < 99; ++i)
    {
        if (disco[i].mbr_tamano != 0)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (strcmp(disco[i].mbr_particion[j].id, _id) == 0)
                {
                    // generando .dot
                    ofstream fs(name_dot);
                    fs << "digraph G{" << endl;
                    fs << "tbl [" << endl;
                    fs << "shape=plaintext" << endl;
                    fs << "label=<" << endl,
                            fs << "<table border='2' cellborder='0' color='blue' cellspacing='1'>" << endl;
                    fs << "<tr>" << endl;
                    fs << "<td colspan='1' rowspan='1'>" << endl;
                    fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>" << endl;
                    fs << "<tr><td>MBR</td></tr>" << endl;
                    fs << "</table>" << endl;
                    fs << "</td>" << endl;
                    for (int k = 0; k < 4; ++k)
                    {
                        if (disco[i].mbr_particion[k].part_type == 'p')
                        {
                            int sizeP, sizeOP = 0;
                            sizeP = disco[i].mbr_particion[k].part_size;
                            sizeOP = (sizeP * 100) / disco[i].mbr_tamano;
                            string tam = to_string(sizeOP);
                            suma += sizeP;
                            float sizeF, sizeOF = 0; //
                            sizeF = disco[i].mbr_particion[k].part_size * 1.00;
                            sizeOF = (sizeF * 100) / disco[i].mbr_tamano;
                            string tam2 = to_string(sizeOF);
                            cout << "***************************************" << endl;
                            cout << "El tmaño es: " << disco[i].mbr_tamano << endl;
                            cout << "antes del string: " << sizeOF << endl;
                            cout << "El % es: " << tam << endl;
                            espaLibre -= sizeOF;
                            fs << "<td colspan='1' rowspan='1'>" << endl;
                            fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>" << endl;
                            fs << "<tr><td>Primaria <br/>Nombre:" + charToString(disco[i].mbr_particion[k].part_name) + "<br/> Formato: " + charToString(disco[i].mbr_particion[k].formato) + " <br/>" + tam2 + " % del disco</td></tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                        }
                        else if (disco[i].mbr_particion[k].part_size == 0)
                        {
                            fs << "<td colspan='1' rowspan='1'>" << endl;
                            fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>" << endl;
                            fs << "<tr><td>Libre</td></tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                        }
                        else if (disco[i].mbr_particion[k].part_type == 'e')
                        {
                            fs << "<td colspan='1' rowspan='1'>" << endl;
                            fs << "<table color='red' border='1' cellborder='1' cellpadding='10' cellspacing='0'>" << endl;
                            fs << "<tr>" << endl;
                            int sizeP, sizeOP = 0;
                            sizeP = disco[i].mbr_particion[k].part_size;
                            sizeOP = (sizeP * 100) / disco[i].mbr_tamano;
                            string tam = to_string(sizeOP);
                            suma += sizeP;

                            for (int l = 0; l < 24; ++l)
                            {
                                if (disco[i].ebr_logicas[l].part_size != 0)
                                {
                                    double size = disco[i].ebr_logicas[l].part_size;
                                    double sizedisk = disco[i].mbr_tamano;
                                    double op = (size * 100) / sizedisk;
                                    stringstream tmp;
                                    tmp << setprecision(3) << fixed << op;
                                    string res = tmp.str();
                                    cout << res << endl;
                                    espaLibre -= op;
                                    fs << "<td>EBR</td><td>" + charToString(disco[i].ebr_logicas[l].part_name) + "<br/> " + res + " % del disco</td>" << endl;
                                }
                            }
                            fs << "<td>Libre</td>" << endl;
                            fs << "</tr>" << endl;
                            fs << "</table>" << endl;
                            fs << "</td>" << endl;
                        }
                    }
                    fs << "<td colspan='1' rowspan='1'>" << endl;
                    fs << "<table color='orange' border='1' cellborder='1' cellpadding='10' cellspacing='0'>" << endl;
                    stringstream tmp2;
                    tmp2 << setprecision(4) << fixed << espaLibre;
                    string res2 = tmp2.str();
                    fs << "<tr><td> Porcentaje Libre del disco: " + res2 + " % </td></tr>" << endl;
                    fs << "</table>" << endl;
                    fs << "</td>" << endl;

                    fs << "</tr>" << endl;
                    fs << "</table>" << endl;
                    fs << ">];" << endl;
                    fs << "}" << endl;
                    fs.close();
                    // puse lo siguiente:
                    string rutajpg = charToString(_path);
                    string render = "dot -Tjpg " + name_dot + " -o " + rutajpg;
                    system(render.c_str());
                }
            }
        }
    }
}

void analisisMount(char comando[])
{
    int contador = 0;
    char lineacomando[100] = "";
    char _path[100] = "";
    char _name[100] = "";
    bool gotPath = false;
    bool gotName = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ' || comando[contador] == '\n')
        {
            contador++;
            memset(lineacomando, 0, 100);
        }
        else
        {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        // validacion de argumentos de mount
        if (strcmp(lineacomando, "mount") == 0)
        {
            cout << "sistema~> Analizando disco y particion a montar..." << endl;
            memset(lineacomando, 0, 100);
            contador++;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 100);
            gotPath = true;
            while (comando[contador] != NULL)
            {
                if (comando[contador] == '"')
                {   // cuando viene con comillas el path
                    contador++;
                    while (comando[contador] != NULL)
                    {
                        if (comando[contador] == '"')
                        {
                            contador++;
                            break;
                        }
                        else
                        {
                            char aux[1] = "";
                            aux[0] = comando[contador];
                            strncat(_path, aux, 1);
                            contador++;
                        }
                    }
                }
                else
                {
                    if (comando[contador] == ' ' || comando[contador] == '\n')
                    {
                        contador++;
                        break;
                    }
                    else
                    {
                        char aux[1] = "";
                        aux[0] = comando[contador];
                        strncat(_path, aux, 1);
                        contador++;
                    }
                }
            }
            cout << "sistema~> se proporciono el siguiente path para montar una particion: " << _path << endl;
        }
        else if (strcmp(lineacomando, ">name=") == 0)
        {
            memset(lineacomando, 0, 100);
            gotName = true;
            while (comando[contador] != NULL)
            {
                if (comando[contador] == '"')
                {   // cuando viene con comillas el path
                    contador++;
                    while (comando[contador] != NULL)
                    {
                        if (comando[contador] == '"')
                        {
                            contador++;
                            break;
                        }
                        else
                        {
                            char aux[1] = "";
                            aux[0] = comando[contador];
                            strncat(_name, aux, 1);
                            contador++;
                        }
                    }
                }
                else
                {
                    if (comando[contador] == ' ' || comando[contador] == '\n')
                    {
                        contador++;
                        break;
                    }
                    else
                    {
                        char aux[1] = "";
                        aux[0] = comando[contador];
                        strncat(_name, aux, 1);
                        contador++;
                    }
                }
            }
            cout << "sistema~> el nombre de la particion a montar es el siguiente: " << _name << endl;
        }
    }

    if (gotName && gotPath)
    { // es para montar
        bool existeDisk = existeArchivo(_path); // valida que si exista el archivo
        if (!existeDisk)
        {
            cout << "Error~> No no se ha encontrado el disco en la ruta especificada. Por favor revifique el path. path: " << _path << endl;
            return;
        }
        bool estaMontado = buscarDiscoPath(_name, _path);
        if (estaMontado)
        {
            cout << "Error~> La particion ya se ha montado." << endl;
            return;
        }
        montajeMount(_path, _name);
    }
    else
    { // solo para listar
        showMount();
    }
}