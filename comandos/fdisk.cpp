//
// Created by gerson on 17/02/23.
//
#include "fdisk.h"

using namespace std;

// validamos el fit de la particion
bool validacionFit(char fit[])
{
    if (fit[0] == 'b' && fit[1] == 'f')
    {
        return true;
    }
    else if (fit[0] == 'w' && fit[1] == 'f')
    {
        return true;
    }
    else if (fit[0] == 'f' && fit[1] == 'f')
    {
        return true;
    }
    return false;
}

// aquí verificamos cuantas particiones primarias hay
int contadorPrimaria(MBR *mbr)
{
    int contadorPrimaria = 0;
    for (int i = 0; i < 4; ++i)
    {
        if ('p' == mbr->mbr_particion[i].part_type)
        {
            contadorPrimaria++;
        }
    }
    return contadorPrimaria;
}

// contamos cuantas particiones extendidas
int contadorExtendida(MBR *mbr)
{
    int contadorExtendida = 0;
    for (int i = 0; i < 4; ++i)
    {
        if ('e' == mbr->mbr_particion[i].part_type)
        {
            contadorExtendida++;
        }
    }
    return contadorExtendida;
}

bool existeNamePart(MBR *mbr, char name[])
{
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(mbr->mbr_particion[i].part_name, name) == 0)
        {
            return true;
        }
    }
    return false;
}

int contadorLogicas(MBR *mbr, EBR *ebr, FILE *file)
{
    int contadorEBR = 0;
    int next = 0;
    for (int i = 0; i < 4; ++i)
    {   // buscamos la particion extendida
        if (mbr->mbr_particion[i].part_type == 'e')
        {   // nos posicionamos en la particion extendida
            fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, file); // leemos el ebr de la particion extendida
            next = ebr->part_next;
            // ahora loopeamos para ir avanzando entre particiones lógicas
            while (next != -1)
            {
                fseek(file, next, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                next = ebr->part_next;
                contadorEBR++;
            }
            contadorEBR++;
            break;
        }
    }
    return contadorEBR; // retornamos la cantidad de particiones logicas
}

// validacion nombres logicas
bool existeNameLog(MBR *mbr, EBR *ebr, FILE *file, char _name[])
{
    int next = 0;
    for (int i = 0; i < 4; ++i)
    {   // nos posicionamos en la particion extendida
        if (mbr->mbr_particion[i].part_type == 'e')
        {   // nos posicionamos en el byte del primer ebr
            fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, file);
            next = ebr->part_next;
            // empezamos a loopear para movernos entre ebr's
            while (next != -1)
            {
                if (strcmp(ebr->part_name, _name) == 0)
                {   // se econtro el nombre de la particion
                    return true;
                }
                fseek(file, next, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                next = ebr->part_next;
            }
            if (strcmp(ebr->part_name, _name) == 0)
            {   // se econtro el nombre de la particion
                return true;
            }
            break;
        }
    }
    return false; // NO se econtro el nombre de la particion
}

void crearParticion(char _size[], char _unit, char _path[], char _type, char _fit[], char _name[])
{
    // funcion para crear una particion en el disco especificado
    FILE *file;
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    file = fopen(_path, "rb+"); // sirve para leelo no para crearlo
    // leemos el mbr
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file);
    // contamos cuantas particiones hay actualmente
    int contador_P = contadorPrimaria(mbr);
    int contador_E = contadorExtendida(mbr);
    int particionesTotales = contador_P + contador_E;
    // validacion el tipo de particon
    if (_type == 'p')
    {   // particion primaria
        if (contador_P >= 4)
        {
            cout << "Error~> Particiones primarias maximas alcanzadas." << endl;
            return;
        }
        else if (particionesTotales == 4)
        {
            cout << "Error~> Particiones maximas alcanzadas." << endl;
            return;
        }
        cout << "sistema~> creando particion..." << endl;
        bool esNombreRepetido = existeNamePart(mbr, _name);
        if (esNombreRepetido)
        {
            cout << "Error~> El nombre de la particion no puede repetirse. Intente con otro nombre." << endl;
            return;
        }
        // asignamos particion al espacio que este disponible
        for (int i = 0; i < 4; ++i)
        {
            if (mbr->mbr_particion[i].part_type == '-')
            {   // en este caso esta disponible
                int sizeTotal = mbr->mbr_tamano; // espacio total
                int sizeUsado = mbr->mbr_particion[0].part_s + mbr->mbr_particion[1].part_s + mbr->mbr_particion[2].part_s + mbr->mbr_particion[3].part_s; // espacio usado
                int sizeDisponible = sizeTotal - (sizeUsado + sizeof(MBR)); // espacio disponible
                string sizeSTR = charToString(_size);
                int sizeParticion = 0;
                // para el tamaño
                if (_unit == 'b')
                {
                    sizeParticion = stoi(sizeSTR);
                }
                else if (_unit == 'k')
                {
                    sizeParticion = stoi(sizeSTR) * 1024;
                }
                else if (_unit == 'm')
                {
                    sizeParticion = stoi(sizeSTR) * 1024 * 1024;
                }
                // si no hay espacio error
                if (sizeDisponible < sizeParticion)
                {
                    cout << "Error~> Espacio insuficiente para crear particion." << endl;
                    return;
                }
                // existe espacio en disco para esa particion
                mbr->mbr_particion[i].part_status = '1';
                mbr->mbr_particion[i].part_type = 'p';
                mbr->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;
                mbr->mbr_particion[i].part_s = sizeParticion;
                strcpy(mbr->mbr_particion[i].part_fit, _fit);
                strcpy(mbr->mbr_particion[i].part_name, _name);
                // para agregar en el mount
                agregarParticionMount(_path, _name, sizeParticion, 'p', _fit, mbr->mbr_particion[i].part_start, 0);

                // se actualiza el mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbr, sizeof(MBR), 1, file);
                // se escribe la particion
                fseek(file, mbr->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbr->mbr_particion[i], sizeof(mbr->mbr_particion[i]), 1, file);
                fclose(file);
                cout << "sistema~> Particion creada con exito :D" << endl;
                break;
            }
        }
    }
    else if (_type == 'e')
    {   // particiones extendidas
        if (contador_E >= 1)
        {
            cout << "Error~> Particiones extendidas maximas alcanzadas." << endl;
            return;
        }
        else if (particionesTotales == 4)
        {
            cout << "Error~> Particiones maximas alcanzadas." << endl;
            return;
        }
        cout << "sistema~> creando particion..." << endl;
        bool esNombreRepetido = existeNamePart(mbr, _name);
        if (esNombreRepetido)
        {
            cout << "Error~> El nombre de la particion no puede repetirse. Intente con otro nombre." << endl;
            return;
        }
        // asignamos particion al espacio que este disponible
        for (int i = 0; i < 4; ++i)
        {
            if (mbr->mbr_particion[i].part_type == '-')
            {
                int sizeTotal = mbr->mbr_tamano;
                int sizeUsado = mbr->mbr_particion[0].part_s + mbr->mbr_particion[1].part_s + mbr->mbr_particion[2].part_s + mbr->mbr_particion[3].part_s;
                int sizeDisponible = sizeTotal - (sizeUsado + sizeof(MBR));
                string sizeSTR = charToString(_size);
                int sizeParticion = 0;
                // para el tamaño
                if (_unit == 'b')
                {
                    sizeParticion = stoi(sizeSTR);
                }
                else if (_unit == 'k')
                {
                    sizeParticion = stoi(sizeSTR) * 1024;
                }
                else if (_unit == 'm')
                {
                    sizeParticion = stoi(sizeSTR) * 1024 * 1024;
                }
                if (sizeDisponible < sizeParticion)
                {
                    cout << "Error~> Espacio insuficiente para crear particion." << endl;
                    return;
                }

                // hay espacio disponible y se empiza a escribir la particion
                mbr->mbr_particion[i].part_status = '1';
                mbr->mbr_particion[i].part_type = 'e';
                mbr->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;
                mbr->mbr_particion[i].part_s = sizeParticion;
                strcpy(mbr->mbr_particion[i].part_fit, _fit);
                strcpy(mbr->mbr_particion[i].part_name, _name);
                agregarParticionMount(_path, _name, sizeParticion, 'e', _fit, mbr->mbr_particion[i].part_start, 0);
                // se crea el primer ebr
                EBR *ebr = (EBR *)malloc(sizeof(EBR));
                ebr->part_start = 0;
                ebr->part_s = 0;
                ebr->part_next = -1;
                memset(ebr->part_name, 0, 16);
                memset(ebr->part_fit, 0, 3);
                // se actualiza el mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbr, sizeof(MBR), 1, file);
                // se escribe la particion
                fseek(file, mbr->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbr->mbr_particion[i], sizeof(mbr->mbr_particion[i]), 1, file);
                // se escribe el ebr
                fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                fwrite(ebr, sizeof(EBR), 1, file);
                fclose(file);
                cout << "sistema~> Particion extendida creada con exito :D" << endl;
                break;
            }
        }
    }
    else if (_type == 'l')
    {   // particion logica
        EBR *ebr = (EBR *)malloc(sizeof(EBR));
        if (contador_E < 1)
        {
            cout << "Error~> No es posible crear una particion lógica sin una particion extendida existente." << endl;
            return;
        }
        int contadorLog = contadorLogicas(mbr, ebr, file);
        if (contadorLog >= 24)
        {
            cout << "Error~> Numero maximo de particiones logicas alcanzado." << endl;
            return;
        }
        bool existeNameLogica = existeNameLog(mbr, ebr, file, _name);
        // se validara si existe nombre repetidos
        if (existeNameLogica)
        {
            cout << "Error~> El nombre de la partion ya existe. Intente con otro." << endl;
            return;
        }
        // empezamos a crear la particion
        for (int i = 0; i < 4; ++i)
        {
            if (mbr->mbr_particion[i].part_type == 'e')
            {   // nos posicionamos en la particion extendida
                fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                if (ebr->part_next == -1)
                {   // primera asigacion de ebr
                    string sizeSTR = charToString(_size);
                    // tamaño de la particion
                    int size = 0;
                    if (_unit == 'b')
                    {
                        size = stoi(sizeSTR);
                    }
                    else if (_unit == 'k')
                    {
                        size = stoi(sizeSTR) * 1024;
                    }
                    else if (_unit == 'm')
                    {
                        size = stoi(sizeSTR) * 1024 * 1024;
                    }
                    // llenamos la info del nuevo ebr
                    ebr->part_start = mbr->mbr_particion[i].part_start + sizeof(EBR) + 1;
                    ebr->part_s = size;
                    strcpy(ebr->part_fit, _fit);
                    strcpy(ebr->part_name, _name);
                    ebr->part_next = ebr->part_start + ebr->part_s + 1;
                    agregarParticionMount(_path, _name, size, 'l', _fit, ebr->part_start, ebr->part_next);
                    // actualizo el ebr
                    fseek(file, ebr->part_start, SEEK_SET);
                    fwrite(ebr, sizeof(EBR), 1, file);
                    // se crea un nuevo ebr para su uso posterior
                    fseek(file, ebr->part_next, SEEK_SET);
                    EBR *ebrnew = (EBR *)malloc(sizeof(EBR));
                    ebrnew->part_start = 0;
                    ebrnew->part_s = 0;
                    ebrnew->part_next = -1;
                    memset(ebr->part_fit, 0, 3);
                    memset(ebr->part_name, 0, 16);
                    fwrite(ebrnew, sizeof(EBR), 1, file);
                    fclose(file);
                    cout << "sistema~> Particion logica creada con exito :D" << endl;
                }
                else
                {   // si no es la primer particion logica
                    int pos = ebr->part_next;
                    int posAnterior = 0;
                    // loopeamos hasta encontrar el ultimo ebr
                    while (pos != -1)
                    {
                        posAnterior = pos;
                        fseek(file, pos, SEEK_SET);
                        fread(ebr, sizeof(EBR), 1, file);
                        pos = ebr->part_next;
                    }
                    // cuando estemos en el ultimo ebr empezamos a actualizar los datos
                    string sizeSTR = charToString(_size);
                    int size = 0;
                    if (_unit == 'b')
                    {
                        size = stoi(sizeSTR);
                    }
                    else if (_unit == 'k')
                    {
                        size = stoi(sizeSTR) * 1024;
                    }
                    else if (_unit == 'm')
                    {
                        size = stoi(sizeSTR) * 1024 * 1024;
                    }
                    // llenamos los datos del ebr
                    ebr->part_start = posAnterior;
                    ebr->part_s = size;
                    strcpy(ebr->part_fit, _fit);
                    strcpy(ebr->part_name, _name);

                    if (contadorLog <= 23)
                    {
                        ebr->part_next = ebr->part_s + ebr->part_start + 1;
                        agregarParticionMount(_path, _name, size, 'l', _fit, ebr->part_start, ebr->part_next);
                        // escribimos el ebr
                        fseek(file, ebr->part_start, SEEK_SET);
                        fwrite(ebr, sizeof(EBR), 1, file);
                        cout << "sistema~> Particion logica creada con exito :D" << endl;
                    }
                    else
                    {   // verificamos si aun cabe una ultima particion logica
                        ebr->part_next = -1;
                        agregarParticionMount(_path, _name, size, 'l', _fit, ebr->part_start, ebr->part_next);
                        fseek(file, ebr->part_start, SEEK_SET);
                        fwrite(ebr, sizeof(EBR), 1, file);
                        cout << "sistema~> Particion logica creada con exito :D" << endl;
                    }
                    // ahora escribimos un nuevo ebr para usar posteriormente si aun cabe otra particion
                    if (contadorLog <= 23)
                    {
                        EBR *ebrnew = (EBR *)malloc(sizeof(EBR));
                        ebrnew->part_start = 0;
                        ebrnew->part_s = 0;
                        ebrnew->part_next = -1;
                        memset(ebrnew->part_fit, 0, 3);
                        memset(ebrnew->part_name, 0, 16);
                        fseek(file, ebr->part_next, SEEK_SET);
                        fwrite(ebrnew, sizeof(EBR), 1, file);
                    }
                    fclose(file);
                }
                break;
            }
        }
    }
}

// funcion para eleminar una particion
void deleteParticion(char _name[], char _path[], char _delete[])
{
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    FILE *file;
    bool existeParticion = false;
    file = fopen(_path, "rb+");
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file);
    // verficamos que el tipo de delete existe
    if (strcmp(_delete, "full") != 0)
    {
        cout << "Error~> No se reconoce el tipo de delete. Si desea eliminar una particion intente con >delete=full." << endl;
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(mbr->mbr_particion[i].part_name, _name) == 0)
        {   // si encontramos la particion a eliminar
            existeParticion = true;
            if (mbr->mbr_particion[i].part_type == 'p')
            {   // eliminamos una particion primaria
                char res;
                cout << "sistema~> ¿Esta seguro de querer eliminar la particion " << _name << "? esta accion no se podra deshacer (Y/n) : ";
                cin >> res;
                if (tolower(res) == 'y')
                {
                    // limpiamos la particion y la dejamos vacia
                    for (int j = mbr->mbr_particion[i].part_start; j < mbr->mbr_particion[i].part_start + sizeof(MBR); ++j)
                    {
                        fseek(file, j, SEEK_SET);
                        fwrite("0", 1, 1, file);
                    }
                    // se actualizae el mbr
                    mbr->mbr_particion[i].part_s = 0;
                    mbr->mbr_particion[i].part_start = 0;
                    mbr->mbr_particion[i].part_type = '-';
                    mbr->mbr_particion[i].part_status = '0';
                    memset(mbr->mbr_particion[i].part_fit, 0, 3);
                    memset(mbr->mbr_particion[i].part_name, 0, 16);
                    elimininarParticionMount(_path, _name);

                    // se escribe el mbr de nuevo
                    fseek(file, 0, SEEK_SET);
                    fwrite(mbr, sizeof(MBR), 1, file);

                    cout << "sistema~> se ha eliminado la particion exitosamente." << endl;
                }
                fclose(file);
                break;
            }
            else if (mbr->mbr_particion[i].part_type == 'e')
            {
                // si nos topamos con una particion extendida tenemos que borrar todas su particiones logicas
                char res;
                cout << "sistema~> ¿Esta seguro de querer eliminar la particion " << _name << "? esta accion no se podra deshacer (Y/n) : ";
                cin >> res;
                if (tolower(res) == 'y')
                {
                    // primero se eliminan las logicas
                    EBR *ebr = (EBR *)malloc(sizeof(EBR));
                    fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                    fread(ebr, sizeof(EBR), 1, file);
                    int pos = 0;
                    // loopeamos hasta eliminar el ultimo ebr
                    while (ebr->part_next != -1)
                    {
                        pos = ebr->part_next;
                        for (int j = ebr->part_start; j < ebr->part_start + sizeof(EBR); ++j)
                        {
                            // borramos toda la info de la particion logica
                            fseek(file, j, SEEK_SET);
                            fwrite("0", 1, 1, file);
                        }
                        elimininarParticionMount(_path, ebr->part_name);
                        fseek(file, pos, SEEK_SET);
                        fread(ebr, sizeof(EBR), 1, file);
                    }
                    for (int j = ebr->part_start; j < ebr->part_start + sizeof(EBR); ++j)
                    {
                        // borramos toda la info de la particion logica
                        fseek(file, j, SEEK_SET);
                        fwrite("0", 1, 1, file);
                    }

                    // ahora eliminamos la particion extendida del mbr
                    for (int j = mbr->mbr_particion[i].part_start; j < mbr->mbr_particion[i].part_start + sizeof(MBR); ++j)
                    {
                        fseek(file, j, SEEK_SET);
                        fwrite("0", 1, 1, file);
                    }
                    // se actualiza el mbr
                    mbr->mbr_particion[i].part_s = 0;
                    mbr->mbr_particion[i].part_start = 0;
                    mbr->mbr_particion[i].part_type = '-';
                    mbr->mbr_particion[i].part_status = '0';
                    memset(mbr->mbr_particion[i].part_fit, 0, 3);
                    memset(mbr->mbr_particion[i].part_name, 0, 16);
                    elimininarParticionMount(_path, _name);

                    // se escribe el mbr de nuevo
                    fseek(file, 0, SEEK_SET);
                    fwrite(mbr, sizeof(MBR), 1, file);
                    fclose(file);

                    cout << "sistema~> se ha eliminado la particion exitosamente." << endl;
                    break;
                }
            }
        }
        // si existe una particion extendida, procedemos a buscar en las particiones logicas, la particion a eliminar
        if (mbr->mbr_particion[i].part_type == 'e')
        {
            EBR *ebr = (EBR *)malloc(sizeof(EBR));
            int inicio = mbr->mbr_particion[i].part_start + sizeof(EBR) + 1;
            fseek(file, inicio, SEEK_SET);
            fread(ebr, sizeof(EBR), 1, file);
            int next = ebr->part_next;
            // loopeamos hasta encontrar la particion
            while (next != -1)
            {
                if (strcmp(ebr->part_name, _name) == 0)
                {
                    existeParticion = true;
                    char res;
                    cout << "sistema~> ¿Esta seguro de querer eliminar la particion " << _name << "? esta accion no se podra deshacer (Y/n) : ";
                    cin >> res;
                    if (tolower(res) == 'y')
                    {
                        int start = ebr->part_start;
                        ebr->part_start = 0;
                        ebr->part_s = 0;
                        memset(ebr->part_fit, 0, 3);
                        memset(ebr->part_name, 0, 16);
                        elimininarParticionMount(_path, _name); // falta esto para el mount

                        fseek(file, start, SEEK_SET);
                        fwrite(ebr, sizeof(EBR), 1, file);
                        cout << "sistema~> se ha eliminado la particion exitosamente. " << endl;
                    }
                    fclose(file);
                    break;
                }
            }
        }
    }
    if (!existeParticion)
    {
        cout << "Error~> La particion que se desea eliminar no existe. Verificque el nombre." << endl;
        return;
    }
}

// funcion para agregar espacio a una particion
void addParticion(char _add[], char _unit, char _path[], char _name[])
{
    string addSTR = charToString(_add);
    int add = 0;
    // validacion unit
    if (_unit == 'b')
    {
        add = stoi(addSTR);
    }
    else if (_unit == 'k')
    {
        add = stoi(addSTR) * 1024;
    }
    else if (_unit == 'm')
    {
        add = stoi(addSTR) * 1024 * 1024;
    }

    if (add > 0)
    {   // agregamos espacio
        // leemos el mbr
        EBR ebrs[25];
        MBR *mbr = (MBR *)malloc(sizeof(MBR));
        MBR *mbraux = (MBR *)malloc(sizeof(MBR));
        PARTICION *particionAux = (PARTICION *)malloc(sizeof(PARTICION));
        particionAux->part_s = 0;
        particionAux->part_start = 0;
        particionAux->part_status = '0';
        for (int i = 0; i < 4; ++i) {
            mbraux->mbr_particion[i] = *particionAux;
        }

        EBR *ebraux = (EBR *)malloc(sizeof(EBR));
        EBR *ebr = (EBR *)malloc(sizeof(EBR));
        ebraux->part_next = -1;
        ebraux->part_start = 0;
        ebraux->part_s = 0;
        for (int i = 0; i < 16; ++i) {
            ebraux->part_name[i] = '\0';
        }

        FILE *file;
        file = fopen(_path, "rb+");
        fseek(file, 0, SEEK_SET);
        fread(mbr, sizeof(MBR), 1, file); // obtenemos toda la informacion de las particiones en el mbr
        int espacioUsado = mbr->mbr_particion[0].part_s + mbr->mbr_particion[1].part_s + mbr->mbr_particion[2].part_s + mbr->mbr_particion[3].part_s + sizeof(MBR);
        int espacioTotal = mbr->mbr_tamano;
        int espacioDisponible = espacioTotal - espacioUsado;
        int contadorEbr = 0;

        bool existeParticion = existeNamePart(mbr, _name);
        if (!existeParticion)
        {
            cout << "Error~> La particion " << _name << " no existe, verifique el nombre." << endl;
            return;
        }
        // buscamos y analizamos la particion
        cout << "sistema~> buscando particion... " << endl;
        if (espacioDisponible < add)
        {
            cout << "Error~> Espacio insuficiente para agregar el espacio ingresado. Intente con un tamanio menor." << endl;
            return;
        }
        cout << "sistema~> analizando particion... " << endl;
        // se obtienen todos los datos del disco
        for (int i = 0; i < 4; ++i)
        {   // solo en la extendida hay mas datos
            if (mbr->mbr_particion[i].part_type == 'e')
            {
                fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                int next = ebr->part_start;
                if (next != -1)
                {
                    while (next != -1)
                    {
                        fseek(file, next, SEEK_SET);
                        fread(ebr, sizeof(EBR), 1, file);
                        // se guardan las logicas
                        ebrs[contadorEbr].part_next = ebr->part_next;
                        ebrs[contadorEbr].part_start = 0;
                        ebrs[contadorEbr].part_s = ebr->part_s;
                        strcpy(ebrs[contadorEbr].part_name, ebr->part_name);
                        strcpy(ebrs[contadorEbr].part_fit, ebr->part_fit);
                        contadorEbr++;
                        next = ebr->part_next;
                    }
                    // se guardan las logicas
                    ebrs[contadorEbr].part_next = ebr->part_next;
                    ebrs[contadorEbr].part_start = ebr->part_start;
                    ebrs[contadorEbr].part_s = ebr->part_s;
                    strcpy(ebrs[contadorEbr].part_name, ebr->part_name);
                    strcpy(ebrs[contadorEbr].part_fit, ebr->part_fit);
                }
            }
        }
        // limpio disco dejando todo_ en cero
        fseek(file, 0, SEEK_SET);
        for (int i = 0; i < mbr->mbr_tamano; ++i)
        {
            fwrite("0", 1, 1, file);
        }
        // se modifica especificamente el tamaño de la particion que se soicita
        for (int i = 0; i < 4; ++i)
        {
            if (strcmp(mbr->mbr_particion[i].part_name, _name) == 0)
            { // se busca a que particion se desea agregar
                mbr->mbr_particion[i].part_s += add;
                break;
            }
        }
        // se guarda mbr
        mbraux->mbr_tamano = mbr->mbr_tamano;
        mbraux->mbr_dsk_signature = mbr->mbr_dsk_signature;
        mbraux->mbr_fecha_creacion = mbr->mbr_fecha_creacion;
        // modificaciones al mbr y ebr
        for (int i = 0; i < 4; ++i)
        {
            int sizeNuevo = mbr->mbr_particion[i].part_s;
            int sizeUsado = mbraux->mbr_particion[0].part_s + mbraux->mbr_particion[1].part_s + mbraux->mbr_particion[2].part_s + mbraux->mbr_particion[3].part_s;
            if (mbr->mbr_particion[i].part_type == 'p')
            {
                mbraux->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;
                // paso de datos
                mbraux->mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                mbraux->mbr_particion[i].part_s = sizeNuevo;
                mbraux->mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                strcpy(mbraux->mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                strcpy(mbraux->mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                // se actualiza mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbraux, sizeof(MBR), 1, file);
                // se escribe la particion modificada
                fseek(file, mbraux->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbraux->mbr_particion[i], sizeof(mbraux->mbr_particion[i]), 1, file);
            }
            else if (mbr->mbr_particion[i].part_type == 'e')
            {
                mbraux->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;
                // paso de datos
                mbraux->mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                mbraux->mbr_particion[i].part_s = sizeNuevo;
                mbraux->mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                strcpy(mbraux->mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                strcpy(mbraux->mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                // se actualiza mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbraux, sizeof(MBR), 1, file);
                // se reescribe la particion modificada
                fseek(file, mbraux->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbraux->mbr_particion[i], sizeof(mbraux->mbr_particion[i]), 1, file);

                // se reescribe los ebr
                int contador = 0;
                int siguiente = 0;
                int inicio = mbraux->mbr_particion[i].part_start + sizeof(EBR) + 1;
                int contadorLog = 0;
                while (ebrs[contador].part_next != -1)
                {
                    contadorLog++;
                    ebraux->part_s = ebrs[contador].part_s;
                    ebraux->part_start = inicio;
                    siguiente = ebraux->part_s + ebraux->part_start + 1;
                    ebraux->part_next = ebraux->part_s + ebraux->part_start + 1;
                    strcpy(ebraux->part_fit, ebrs[contador].part_fit);
                    strcpy(ebraux->part_name, ebrs[contador].part_name);
                    fseek(file, inicio, SEEK_SET);
                    fwrite(ebraux, sizeof(EBR), 1, file);
                    inicio = siguiente;
                    contador++;
                    if (contadorLog <= 23)
                    {
                        EBR *ebrc = (EBR *)malloc(sizeof(EBR));
                        ebrc->part_start = 0;
                        ebrc->part_s = 0;
                        ebrc->part_next = -1;
                        memset(ebrc->part_fit, 0, 3);
                        memset(ebrc->part_name, 0, 16);
                        fseek(file, inicio, SEEK_SET);
                        fwrite(ebrc, sizeof(EBR), 1, file);
                    }
                }
                // cuanod no hay particiones logicas en el mbr
                if (contadorLog == 0)
                {
                    EBR *ebrc = (EBR *)malloc(sizeof(EBR));
                    ebrc->part_start = 0;
                    ebrc->part_s = 0;
                    ebrc->part_next = -1;
                    memset(ebrc->part_fit, 0, 3);
                    memset(ebrc->part_name, 0, 16);
                    fseek(file, inicio, SEEK_SET);
                    fwrite(ebrc, sizeof(EBR), 1, file);
                }
            }
        }
        cout << "sistema~> Se ha incrementado el espacio con exito :D" << endl;
        fclose(file);
    }
    else if (add < 0)
    {   // quitando espacio
        // leemos el mbr
        EBR ebrs[25];
        MBR *mbr = (MBR *)malloc(sizeof(MBR));
        MBR *mbraux = (MBR *)malloc(sizeof(MBR));
        PARTICION *particionAux = (PARTICION *)malloc(sizeof(PARTICION));
        particionAux->part_s = 0;
        particionAux->part_start = 0;
        particionAux->part_status = '0';
        for (int i = 0; i < 4; ++i) {
            mbraux->mbr_particion[i] = *particionAux;
        }

        EBR *ebraux = (EBR *)malloc(sizeof(EBR));
        EBR *ebr = (EBR *)malloc(sizeof(EBR));
        ebraux->part_next = -1;
        ebraux->part_start = 0;
        ebraux->part_s = 0;
        for (int i = 0; i < 16; ++i) {
            ebraux->part_name[i] = '\0';
        }

        FILE *file;
        file = fopen(_path, "rb+");
        fseek(file, 0, SEEK_SET);
        fread(mbr, sizeof(MBR), 1, file);
        int sizeActual = 0;
        int nuevoSize = 0;
        int contador = 0;

        bool existeParticion = existeNamePart(mbr, _name);
        if (!existeParticion)
        {
            cout << "Error~> No se ha encontrado la particion. Compruebe el nombre." << endl;
            return;
        }
        cout << "sistema~> analizando particion..." << endl;
        // buscamos cuanto pesa actualmente y cuanto se desea reducir
        for (int i = 0; i < 4; ++i)
        {
            if (strcmp(mbr->mbr_particion[i].part_name, _name) == 0)
            {
                sizeActual = mbr->mbr_particion[i].part_s;
                nuevoSize = sizeActual + add;
            }
        }
        // verificamos que el tamaño total de la particion no sea negativa
        if (nuevoSize < 0)
        {
            cout << "Error~> La particion no puede tener un tamanio negativo, por favor intente con otro valor." << endl;
            return;
        }
        // se obtienen todos los datos del disco
        for (int i = 0; i < 4; ++i)
        {   // solo en la extendida hay mas datos
            if (mbr->mbr_particion[i].part_type == 'e')
            {
                fseek(file, mbr->mbr_particion[i].part_start + sizeof(EBR) + 1, SEEK_SET);
                fread(ebr, sizeof(EBR), 1, file);
                int next = ebr->part_start;
                // no se ha escrito nada el nexto es -1 al principio
                if (next != -1)
                {
                    while (next != -1)
                    {
                        fseek(file, next, SEEK_SET);
                        fread(ebr, sizeof(EBR), 1, file);
                        // se guardan las logicas
                        ebrs[contador].part_next = ebr->part_next;
                        ebrs[contador].part_start = 0;
                        ebrs[contador].part_s = ebr->part_s;
                        strcpy(ebrs[contador].part_name, ebr->part_name);
                        strcpy(ebrs[contador].part_fit, ebr->part_fit);
                        contador++;
                        next = ebr->part_next;
                    }
                    // se guardan las logicas
                    ebrs[contador].part_next = ebr->part_next;
                    ebrs[contador].part_start = ebr->part_start;
                    ebrs[contador].part_s = ebr->part_s;
                    strcpy(ebrs[contador].part_name, ebr->part_name);
                    strcpy(ebrs[contador].part_fit, ebr->part_fit);
                }
                break;
            }
        }
        // limpiando disco
        fseek(file, 0, SEEK_SET);
        for (int i = 0; i < mbr->mbr_tamano; ++i)
        {
            fwrite("0", 1, 1, file);
        }
        // se modifica el tamaño de la particion
        for (int i = 0; i < 4; ++i)
        {
            if (strcmp(mbr->mbr_particion[i].part_name, _name) == 0)
            {
                mbr->mbr_particion[i].part_s += add;
                break;
            }
        }
        // modificamos el mbr
        mbraux->mbr_tamano = mbr->mbr_tamano;
        mbraux->mbr_dsk_signature = mbr->mbr_dsk_signature;
        mbraux->mbr_fecha_creacion = mbr->mbr_fecha_creacion;
        // modificaciones al mbr y ebr
        for (int i = 0; i < 4; ++i)
        {
            int nuevoSize = mbr->mbr_particion[i].part_s;
            if (mbr->mbr_particion[i].part_type == 'p')
            {
                int sizeUsado = mbraux->mbr_particion[0].part_s + mbraux->mbr_particion[1].part_s + mbraux->mbr_particion[2].part_s + mbraux->mbr_particion[3].part_s;
                mbraux->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;

                // actualizamos datos
                mbraux->mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                mbraux->mbr_particion[i].part_s = nuevoSize;
                mbraux->mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                strcpy(mbraux->mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                strcpy(mbraux->mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                // se actualiza mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbraux, sizeof(MBR), 1, file);
                // se escribe la particion modificada
                fseek(file, mbraux->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbraux->mbr_particion[i], sizeof(mbraux->mbr_particion[i]), 1, file);
            }
            else if (mbr->mbr_particion[i].part_type == 'e')
            {
                int sizeUsado = mbraux->mbr_particion[0].part_s + mbraux->mbr_particion[1].part_s + mbraux->mbr_particion[2].part_s + mbraux->mbr_particion[3].part_s;
                mbraux->mbr_particion[i].part_start = sizeUsado + sizeof(MBR) + 1;
                // actualizamos datos
                mbraux->mbr_particion[i].part_status = mbr->mbr_particion[i].part_status;
                mbraux->mbr_particion[i].part_s = nuevoSize;
                mbraux->mbr_particion[i].part_type = mbr->mbr_particion[i].part_type;
                strcpy(mbraux->mbr_particion[i].part_fit, mbr->mbr_particion[i].part_fit);
                strcpy(mbraux->mbr_particion[i].part_name, mbr->mbr_particion[i].part_name);
                // se actualiza mbr
                fseek(file, 0, SEEK_SET);
                fwrite(mbraux, sizeof(MBR), 1, file);
                // se reescribe la particion modificada
                fseek(file, mbraux->mbr_particion[i].part_start, SEEK_SET);
                fwrite(&mbraux->mbr_particion[i], sizeof(mbraux->mbr_particion[i]), 1, file);
                // se reescribe los ebr
                int contadorEbrs = 0;
                int siguiente = 0;
                int inicio = mbraux->mbr_particion[i].part_start + sizeof(EBR) + 1;
                int contadorLogs = 0;
                while (ebrs[contadorEbrs].part_next != -1)
                {
                    ebraux->part_s = ebrs[contadorEbrs].part_s;
                    ebraux->part_start = inicio;
                    siguiente = ebraux->part_s + ebraux->part_start + 1;
                    ebraux->part_next = ebraux->part_s + ebraux->part_start + 1;
                    strcpy(ebraux->part_fit, ebrs[contadorEbrs].part_fit);
                    strcpy(ebraux->part_name, ebrs[contadorEbrs].part_name);
                    fseek(file, inicio, SEEK_SET);
                    fwrite(ebraux, sizeof(EBR), 1, file);
                    contadorLogs++;
                    inicio = siguiente;
                    contadorEbrs++;
                    if (contadorLogs <= 23)
                    {
                        EBR *ebrc = (EBR *)malloc(sizeof(EBR));
                        ebrc->part_start = 0;
                        ebrc->part_s = 0;
                        ebrc->part_next = -1;
                        memset(ebrc->part_fit, 0, 3);
                        memset(ebrc->part_name, 0, 16);
                        fseek(file, inicio, SEEK_SET);
                        fwrite(ebrc, sizeof(EBR), 1, file);
                    }
                }
                // si no hay particiones logicas en la particion
                if (contadorLogs == 0)
                {
                    EBR *ebrc = (EBR *)malloc(sizeof(EBR));
                    ebrc->part_start = 0;
                    ebrc->part_s = 0;
                    ebrc->part_next = -1;
                    memset(ebrc->part_fit, 0, 3);
                    memset(ebrc->part_name, 0, 16);
                    fseek(file, inicio, SEEK_SET);
                    fwrite(ebrc, sizeof(EBR), 1, file);
                }
            }
        }
        cout << "sistema~> Se ha reducido el espacio en la particion con exito." << _name << endl;
        fclose(file);
    }
}

void analisisFdisk(char comando[])
{
    int contador = 0;
    char lineacomando[200] = "";
    // parametros
    char sizeChar[20] = "";
    char unitChar = 'k';
    char pathChar[100] = "";
    char typeChar = 'p';
    char fitChar[3] = {'w', 'f'};
    char deleteChar[20] = "";
    char nameChar[16] = "";
    char addChar[20] = "";

    // argumentos opcionales
    bool gotSize = false;
    bool gotUnit = false;
    bool gotType = false;
    bool gotFit = false;
    bool gotDelete = false;
    bool gotAdd = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
        {   // aquí separamos los comandos
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
        // validaciones de claves
        if (strcmp(lineacomando, "fdisk") == 0)
        {
            cout << "sistema~> analizando disco para crear/remover particion..." << endl;
            memset(lineacomando, 0, 200);
        }
        else if (strcmp(lineacomando, ">size=") == 0)
        {
            memset(lineacomando, 0, 200);
            gotSize = true;
            // para obener valor
            while (comando[contador] != NULL)
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
                    strncat(sizeChar, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> La particion tendra un tamanio de " << sizeChar << endl;
        }
        else if (strcmp(lineacomando, ">unit=") == 0)
        {
            gotUnit = true;
            memset(lineacomando, 0, 200);
            unitChar = tolower(comando[contador]);
            contador++;
            cout << "sistema~> El espacio se medira en " << unitChar << "bytes" << endl;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 200);
            // para obtener el valor
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
                            strncat(pathChar, aux, 1);
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
                        strncat(pathChar, aux, 1);
                        contador++;
                    }
                }
            }
            cout << "sistema~> la ruta proporcionada es la siguiente: " << pathChar << endl;
        }
        else if (strcmp(lineacomando, ">type=") == 0)
        {
            gotType = true;
            memset(lineacomando, 0, 200);
            typeChar = tolower(comando[contador]);
            contador++;
            cout << "sistema~> el tipo de particion sera: " << typeChar << endl;
        }
        else if (strcmp(lineacomando, ">fit=") == 0)
        {
            gotFit = true;
            memset(lineacomando, 0, 200);
            memset(fitChar, 0, 3);
            // para obtener valor fit
            while (comando[contador] != NULL)
            {
                if (comando[contador] == ' ' || comando[contador] == '\n')
                {
                    contador++;
                    break;
                }
                else
                {
                    char aux[1] = "";
                    aux[0] = tolower(comando[contador]);
                    strncat(fitChar, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> el tipo de fit para la particion sera: " << fitChar << endl;
        }
        else if (strcmp(lineacomando, ">delete=") == 0)
        {
            gotDelete = true;
            memset(lineacomando, 0, 200);
            // para obtenre valor delete
            while (comando[contador] != NULL)
            {
                if (comando[contador] == ' ' || comando[contador] == '\n')
                {
                    contador++;
                    break;
                }
                else
                {
                    char aux[1] = "";
                    aux[0] = tolower(comando[contador]);
                    strncat(deleteChar, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> analizando particion para poder eliminar... " << endl;
        }
        else if (strcmp(lineacomando, ">name=") == 0)
        {
            memset(lineacomando, 0, 200);
            // para obtener valor name
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
                            strncat(nameChar, aux, 1);
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
                        strncat(nameChar, aux, 1);
                        contador++;
                    }
                }
            }
            cout << "sistema~> la particion a analizar es la siguiente: " << nameChar << endl;
        }
        else if (strcmp(lineacomando, ">add=") == 0)
        {
            gotAdd = true;
            memset(lineacomando, 0, 200);
            // para obener valor
            while (comando[contador] != NULL)
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
                    strncat(addChar, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> analizando particion para agregar/quitar espacio... " << endl;
        }
    }

    // validacion para crear particiones
    bool existeDisk = existeArchivo(pathChar);
    if (!existeDisk)
    {
        cout << "Error~> No se ha encontrado el disco. Verifique la ruta y vuelva a intentarlo." << endl;
        return;
    }
    if (gotAdd)
    {   // opcion para añadir espacio en particion
        // se encontro el disco
        if (unitChar != 'b' && unitChar != 'k' && unitChar != 'm')
        {
            cout << "Error~> El tipo de unidad es invalido." << endl;
            return;
        }
        addParticion(addChar, unitChar, pathChar, nameChar);
    }
    else if (gotDelete)
    {   // opcion de eliminar particion
        deleteParticion(nameChar, pathChar, deleteChar);
    }
    else
    {   // es para particion
        if (gotSize)
        {   // verificamos que se trate de un número
            gotSize = validacionSize(sizeChar);
        }
        gotFit = validacionFit(fitChar);
        // verificamos si existe el size
        if (!gotSize)
        {
            cout << "Error~> El tamanio dado no es valido." << endl;
            return;
        }
        if (unitChar != 'b' && unitChar != 'k' && unitChar != 'm')
        {
            cout << "Error~> El tipo de unidad es invalido." << endl;
            return;
        }
        if (typeChar != 'p' && typeChar != 'l' && typeChar != 'e')
        {
            cout << "Error~> Tipo de particion invalido. opciones disponibles: p (primaria), l (logica), e (extendida)" << endl;
            return;
        }
        if (!gotFit)
        {
            cout << "Error~> Tipo de fit invalido. opciones disponibles: ff (first fit), wf (worst fit), bf (best fit)" << endl;
            return;
        }
        // una vez ya hayamos verificado los argumentos obligatorios procedemos a crear la particion
        crearParticion(sizeChar, unitChar, pathChar, typeChar, fitChar, nameChar);
    }
}