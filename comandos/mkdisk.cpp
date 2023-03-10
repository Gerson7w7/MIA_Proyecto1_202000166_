//
// Created by gerson on 16/02/23.
//
#include "mkdisk.h"

using namespace std;

/* mkdisk >s=3000 >u=K >path=/home/user/Disco1.dsk
 * >s=tamaño del disco (positivo)
 * >u=unidad de tamaño (K, M)
 * >path=ruta donde se guardará el disco duro
 * >fit=ajust de particiones (BF, FF, WF) */
string pathName(char path[])
{
    int numDiagonales = 0; // numero de diagonales
    int contadorFind = 0;
    int i = 0;
    char name[100] = "";
    // contamos cuantas diagonales hay en el path
    while (path[i] != NULL)
    {
        if (path[i] == '/')
        {
            numDiagonales++;
        }
        i++;
    }
    i = 0; // se reincia contador
    while (path[i] != NULL)
    {
        if (path[i] == '/')
        {
            contadorFind++;
        }
        if (numDiagonales != contadorFind)
        {

            char aux[1] = "";
            aux[0] = path[i];
            strncat(name, aux, 1);
        }
        i++;
    }
    return charToString(name);
}

string nombrePath(char path[])
{
    int numDiagonales = 0; // numero de diagonales
    int contadorFind = 0;
    int contador = 0;
    char nuevoPath[100] = "";
    char valor_name[16] = "";
    // contamos cuantas diagonales hay en el path
    while (path[contador] != NULL)
    {
        if (path[contador] == '/')
        {
            numDiagonales++;
        }
        contador++;
    }
    contador = 0;
    while (path[contador] != NULL)
    { // recorre todo el path como por ejemplo: /home/gerson/Escritorio/disco1.dsk

        if (path[contador] == '/')
        {
            contadorFind++;
        }
        if (numDiagonales == contadorFind)
        {
            if (path[contador] != '/')
            {
                char aux[1] = "";
                aux[0] = path[contador];
                strncat(valor_name, aux, 1);
            }
        }
        contador++;
    }
    return charToString(valor_name);
}

// validar si el nombre tiene la extencion .dsk
bool validacionName(char name[])
{
    int punto;
    for (int i = 0; i < 20; ++i)
    {
        if (name[i] != NULL)
        {
            if (!isalnum(name[i]) && name[i] != '.' && name[i] != '_')
            {
                // no es un nombre válido
                return false;
            }
            if (name[i] == '.')
            {
                punto = i;
                break;
            }
        }
    }

    // validacion de la extension
    if (name[punto + 1] == 'd' && name[punto + 2] == 's' && name[punto + 3] == 'k')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool validacionUnit(char _unit)
{
    if (_unit == 'k')
    {
        return true;
    }
    else if (_unit == 'm')
    {
        return true;
    }
    // unidad no válida
    return false;
}

// función para crear el archivo que simulará el disco.
void crearDiscos(char path[], char name[], char size[], char _unit)
{
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    string sizeSTR = charToString(size);
    string pathSTR = charToString(path);
    string nameSTR = charToString(name);
    string path_disco = pathSTR + nameSTR;
    int _size = stoi(sizeSTR);
    // tamaño del disco en bytes
    if (_unit == 'k')
    {
        _size = _size * 1024;
    }
    else if (_unit == 'm')
    {
        _size = _size * 1024 * 1024;
    }

    // se inicializa el mbr
    // int start = (int)sizeof(MBR);

    mbr->mbr_dsk_signature = rand() % 100;  // se asigna el signature con un random
    mbr->mbr_tamano = _size;                // se asigna en tamaño en bytes
    time_t fecha;
    fecha = time(NULL);
    mbr->mbr_fecha_creacion = fecha; // se asigna el timpo de creacion

    // se inicializa las particiones
    for (int i = 0; i < 4; ++i)
    {
        mbr->mbr_particion[i].part_status = '0';
        mbr->mbr_particion[i].part_type = '-';
        mbr->mbr_particion[i].part_start = 0;
        mbr->mbr_particion[i].part_s = 0;
    }

    // se escribe el mbr en el disco
    FILE *file;                              // se crea el file para escribir en ello
    file = fopen(path_disco.c_str(), "wb+"); // en modo escribir binario

    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < _size; ++i)
    {
        fwrite("0", 1, 1, file);
    }
    fseek(file, 0, SEEK_SET);          // se posiciona al principio del disco
    fwrite(mbr, sizeof(MBR), 1, file); // se escribe el mbr

    fclose(file);
    cout << "sistema~> El disco creado con exito! :D" << endl;
}

void analisisMkdisk(char comando[])
{
    cout << "sistema~> Creando disco..." << endl;
    cout << "sistema~> Verificando parametros... " << endl;
    int contador = 0;
    char lineacomando[200] = "";

    char _size[20] = "";
    char _unit = 'm';
    char _pathCompleto[100] = "";
    char _onlyPath[100] = "";
    char _name[16] = ""; // Este valor se obtiene del Path

    // argumentos opcionales
    bool gotSize = false;
    bool gotUnidad = false;
    bool gotPath = false;
    bool gotName = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
        {
            contador++;
            memset(lineacomando, 0, 200);
        }
        else
        {
            char aux[1] = "";
            aux[0] = tolower(comando[contador]);
            strncat(lineacomando, aux, 1);
            contador++;
        }

        // validacion claves y valores
        if (strcmp(lineacomando, "mkdisk") == 0)
        {
            memset(lineacomando, 0, 200);
            contador++;
        }
        else if (strcmp(lineacomando, ">size=") == 0)
        {
            memset(lineacomando, 0, 200);

            while (comando[contador] != NULL)
            {
                if (comando[contador] == ' ' || comando[contador] == '\n')
                {
                    contador++;
                    break;
                }
                else
                {   // aquí obtenemos el size del disco
                    char aux[1] = "";
                    aux[0] = comando[contador];
                    strncat(_size, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> El disco tendrá un tamanio de: " << _size << endl;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 200);
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
                            strncat(_pathCompleto, aux, 1);
                            contador++;
                        }
                    }
                }
                else
                {   // si el path no lleva comillas
                    if (comando[contador] == ' ' || comando[contador] == '\n')
                    {
                        contador++;
                        break;
                    }
                    else
                    {
                        char aux[1] = "";
                        aux[0] = comando[contador];
                        strncat(_pathCompleto, aux, 1);
                        contador++;
                    }
                }
            }
            // aqui se obtiene el nombre del disco
            string nombreConst = nombrePath(_pathCompleto);
            strcpy(_name, nombreConst.c_str());
            cout << "sistema~> Nombre del disco a crear: " << _name << endl;

            // path sin cnombre:
            string _onlyPathConst = pathName(_pathCompleto);
            strcpy(_onlyPath, _onlyPathConst.c_str());
            cout << "sistema~> Directorio a crear el disco: " << _onlyPath << endl;
        }
        else if (strcmp(lineacomando, ">unit=") == 0)
        {
            gotUnidad = true;
            memset(lineacomando, 0, 200);
            _unit = tolower(comando[contador]);
            contador++;
            cout << "sistema~> unidad de bytes del disco: " << _unit << endl;
        }
    }

    string pathSTR = charToString(_onlyPath); // valido si existe el directorio
    gotPath = validacionPath(pathSTR);        // continuasion de lo de arriba
    gotSize = validacionSize(_size);
    gotName = validacionName(_name);
    gotUnidad = validacionUnit(_unit);
    // para agregar la barra del directorio sin el nombre
    char barra[1];
    strcpy(barra, "/");
    strncat(_onlyPath, barra, 1);
    // primero verificamos si existe el directorio
    if (!gotPath)
    {   // si no existe el directorio lo creamos
        cout << "sistema~> Directorio que ingreso no existe, creando directorio..." << endl;
        cout << "sistema~> Se creo directorio con exito :D" << endl;
        // Como no existe el directorio se procede a crear
        string dir = "mkdir -p \"" + pathSTR + "\"";
        system(dir.c_str());
    }
    // ahora verificamos los parametros obligatorios
    if (!gotUnidad)
    {
        cout << "Error~> Unidad no valido." << endl;
        return;
    }
    else if (!gotSize)
    {
        cout << "Error~> Tamanio no valido o no encontrado" << endl;
        return;
    }
    else if (!gotName)
    {
        cout << "Error~> La extension del disco no es es esperado, verifique se trate de un .dsk" << endl;
        return;
    }
    // comenzamos a crear el disco
    crearDiscos(_pathCompleto, _name, _size, _unit);
}
