//
// Created by gerson on 28/02/23.
//

#include "mkfile.h"

// funcion que retorna el indice de bloque que este disponible
int returnBlockAvailable2(SUPER_BLOQUE *super, FILE *file)
{
    int numerobloques = super->s_block_count;
    char bitbloques[numerobloques];
    int indicebloque = -1;
    fseek(file, super->s_bm_block_start, SEEK_SET);
    fread(&bitbloques, 64, 1, file);
    for (int i = 0; i < numerobloques; ++i)
    {
        if (bitbloques[i] == '0')
        {
            indicebloque = i;
            break;
        }
    }
    return indicebloque;
}

// funcion para obtener nombre de la ruta
string filename2(string ruta)
{
    string name = ruta;
    string delimitador = "/";
    size_t pos = 0;
    string token;
    while ((pos = name.find(delimitador)) != std::string::npos)
    {
        token = name.substr(0, pos);
        name.erase(0, pos + delimitador.length());
    }
    return name;
}

void linktofile(int indiceinodocarpeta, int indicebloquearchivo, string namefile, FILE *file, SUPER_BLOQUE *super)
{
    INODOS carpeta;
    fseek(file, super->s_inode_start + indiceinodocarpeta * sizeof(INODOS), SEEK_SET);
    fread(&carpeta, sizeof(INODOS), 1, file);

    bool hay_espacios_indirectos = false;
    int valorinodo = -1;
    for (int i = 0; i < 12 && valorinodo == -1; i++)
    {
        if (carpeta.i_block[i] != -1)
        {
            BLOQUECARPETA carpetalectura;
            fseek(file, super->s_block_start + (carpeta.i_block[i]) * 64, SEEK_SET);
            fread(&carpetalectura, 64, 1, file);
            for (int j = 0; j < 4 && valorinodo == -1; j++)
            {
                if (carpetalectura.b_content[j].b_inodo == -1)
                {
                    if (file)
                    {
                        strcpy(carpetalectura.b_content[j].b_name, namefile.c_str());
                        carpetalectura.b_content[j].b_inodo = (int)indicebloquearchivo;

                        valorinodo = indicebloquearchivo;
                        BLOQUECARPETA nuevo = carpetalectura;
                        for (int k = 0; k < 4; k++)
                        {
                            nuevo.b_content[k] = carpetalectura.b_content[k];
                        }
                        fseek(file, super->s_block_start + (carpeta.i_block[i]) * 64, SEEK_SET);
                        fwrite(&nuevo, 64, 1, file);
                    }
                    else
                    {
                        cout << "Error~> Pudo perderse en el buffer" << endl;
                    }
                }
                if (valorinodo > 0)
                {
                    fseek(file, super->s_block_start + (carpeta.i_block[i]) * 64, SEEK_SET);
                    fwrite(&carpetalectura, 64, 1, file);
                    break;
                }
            }
        }
        else
        {
            hay_espacios_indirectos = true;
        }
    }

    int posicion = 0;
    if (hay_espacios_indirectos && valorinodo == -1)
    {
        for (int i = 0; i < 12 && valorinodo == -1; i++)
        {
            if (carpeta.i_block[i] == -1)
            {
                int nuevobloque = returnBlockAvailable(super, file);
                posicion = i;
                carpeta.i_block[i] = nuevobloque;
                BLOQUECARPETA carpetalectura;
                CONTENT contenido;

                strcpy(contenido.b_name, "-");
                contenido.b_inodo = -1;
                for (int j = 0; j < 4; ++j)
                {
                    carpetalectura.b_content[j] = contenido;
                }
                valorinodo = nuevobloque;
                strcpy(carpetalectura.b_content[0].b_name, namefile.c_str());
                carpetalectura.b_content[0].b_inodo = indicebloquearchivo;

                fseek(file, super->s_block_start + (nuevobloque)*64, SEEK_SET);
                fwrite(&carpetalectura, 64, 1, file);
                char bitbloque = '1';
                fseek(file, super->s_bm_block_start + (nuevobloque) * sizeof(char), SEEK_SET);
                fwrite(&bitbloque, sizeof(char), 1, file);
            }
        }
        if (valorinodo > 0)
        {
            carpeta.i_block[posicion] = valorinodo;
            fseek(file, super->s_inode_start + (indiceinodocarpeta) * sizeof(INODOS), SEEK_SET);
            fwrite(&carpeta, sizeof(INODOS), 1, file);
        }
    }
}

void rewriteBmBlock(SUPER_BLOQUE *super, FILE *file, int indice, char value)
{
    char bitbloque = value;
    fseek(file, super->s_bm_block_start + indice * sizeof(char), SEEK_SET);
    fread(&bitbloque, sizeof(char), 1, file);
}

void rewriteBmInode(SUPER_BLOQUE *super, FILE *file, int indice, char value)
{
    char bitinodo = value;
    fseek(file, super->s_bm_inode_start + indice * sizeof(char), SEEK_SET);
    fwrite(&bitinodo, sizeof(char), 1, file);
}

int createFile(FILE *file, SUPER_BLOQUE *super, string texto)
{
    char noBloquesautilizar = '1';                        // SELECTOR DE BLOQUES
    int cantidadcaracteresdeltexto = (int)texto.length(); // PARA SABER CUANTOS CARACTERES VAMOS A ALMACENAR
    int indexcurrent = -1;
    int inododisponible = -1;
    if (noBloquesautilizar == '1')
    {
        inododisponible = returnBlockAvailable2(super, file);
        if (inododisponible != -1)
        {
            INODOS filenew;
            // se crea el inodo
            filenew.i_s = 0;
            filenew.i_type = '1'; // el tipo es de archivo
            filenew.i_gid = 1;
            filenew.i_uid = 1;
            for (int i = 0; i < 15; ++i)
            {
                filenew.i_block[i] = -1;
            }
            time_t timeactual;
            timeactual = time(NULL);
            filenew.i_mtime = timeactual;
            filenew.i_ctime = timeactual;
            filenew.i_atime = timeactual;
            rewriteBmInode(super, file, inododisponible, '1');

            fseek(file, super->s_inode_start + inododisponible * sizeof(INODOS), SEEK_SET);
            fwrite(&filenew, sizeof(INODOS), 1, file);
            super->s_free_inodes_count--;
            cout << "sistema~> Archivo creado con exito :D" << endl;
        }
        else
        {
            cout << "Error~> No hay espacio en el disco para crear archivo" << endl;
        }
    }
    return inododisponible;
}

bool createRecursivom(FILE *file, SUPER_BLOQUE *auxsuper, list<string> *Lista, int *indiceactualdeinodo)
{
    INODOS inodoactual;
    int indice = (*indiceactualdeinodo);
    fseek(file, auxsuper->s_inode_start + indice * sizeof(INODOS), SEEK_SET);
    fread(&inodoactual, sizeof(INODOS), 1, file);
    // se crea carpetas luego inodos
    string nombreCarpeta = "";
    int contador = 0;
    for (string item : *Lista)
    {
        if (contador == 0)
        {
            nombreCarpeta = item; // obtengo el nombre de la carpeta
            break;
        }
    }
    if (nombreCarpeta == "#")
    {
        return false;
    }
    int nuevacarpeta = searchFolder(&inodoactual, file, auxsuper, nombreCarpeta);
    bool recursividad = pm;
    if (nuevacarpeta == -1)
    {
        recursividad = true;
        ++controlm;
    }

    // para la recursividad
    if (recursividad || (controlm - 2 == (int)Lista->size()) || (controlm + 2 == (int)Lista->size()))
    {
        Lista->pop_front();
        if (nuevacarpeta == -1)
        {
            (*indiceactualdeinodo) = nuevacarpeta;
            return createRecursivom(file, auxsuper, Lista, indiceactualdeinodo);
        }
        else
        {
            int espacio_carpeta = buscarEspacio(&inodoactual, file, auxsuper, indiceactualdeinodo, nombreCarpeta);
            if (espacio_carpeta != -1)
            {
                (*indiceactualdeinodo) = espacio_carpeta;
            }
            else
            {
                cout << "Error~> Memoria insuficiente." << endl;
                return true;
            }
        }
        if (Lista->size() == 0)
        {
            return true;
        }
        else
        {
            return createRecursivom(file, auxsuper, Lista, indiceactualdeinodo);
        }
    }
    else
    {
        cout << "Error~> El directorio necesita >r" << endl;
        omitirarchivo = true;
        return true;
    }
}

int comprobarEspacio(string path)
{
    int retorno = 0;
    for (int i = 0; i < (int)path.length(); i++)
    {
        if (path[i] != ' ')
        {
            retorno = 0;
        }
        else
        {
            retorno = 1;
            break;
        }
    }
    return retorno;
}

// para devolver el directorio sin el archivo
string filePath(string ruta)
{
    string s = ruta;
    string delimitador = "/";
    string retorno = "";
    string token = "";
    int cont = 0;
    size_t pos = 0;
    while ((pos = s.find(delimitador)) != std::string::npos)
    {
        token = s.substr(0, pos);
        if (cont == 0)
        {
            int tipo = comprobarEspacio(token);
            if (tipo == 1)
            {
                retorno += "'" + token + "'";
            }
            else
            {
                retorno += token;
            }
        }
        else
        {
            int tipo = comprobarEspacio(token);
            if (tipo == 1)
            {
                retorno += "/" + token + "/";
            }
            else
            {
                retorno += "/" + token;
            }
        }
        cont++;
        s.erase(0, pos + delimitador.length());
    }
    return retorno;
}

void ejecutarMkfile(DISCO disco, char _id[], char _directorio[], char _path[])
{
    omitirarchivo = false;
    if (disco.mbr_tamano == 0)
    {
        cout << "Error~> No se encuentra la particion montada." << endl;
        return;
    }
    string nombre_buscar;
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(disco.mbr_particion[i].id, _id) == 0)
        {
            nombre_buscar = charToString(disco.mbr_particion[i].part_name);
            break;
        }
    }
    FILE *file;
    file = fopen(_path, "rb+");
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file);
    int indice;
    for (int i = 0; i < 4; ++i)
    {   // se extrae el indice
        if (strcmp(mbr->mbr_particion[i].part_name, nombre_buscar.c_str()) == 0)
        {
            indice = i;
            break;
        }
    }
    SUPER_BLOQUE auxsuper;
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fread(&auxsuper, sizeof(SUPER_BLOQUE), 1, file);
    if (auxsuper.s_magic == 0)
    {
        cout << "Error~> La particion no esta formateado." << endl;
        fclose(file);
        return;
    }
    // actualizamos el jorunaling
    JOURNALING auxjournaling;
    int inicio_jour = mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + sizeof(SUPER_BLOQUE) + 1;
    fseek(file, inicio_jour, SEEK_SET);
    fread(&auxjournaling, sizeof(JOURNALING), 1, file);
    auxjournaling.journal_tipo = '1'; // es directorio
    strcpy(auxjournaling.journal_propietario, "root");
    auxjournaling.journal_persimo = 1;
    strcpy(auxjournaling.journal_nombre, "archivo");
    strcpy(auxjournaling.journal_contenido, "archivo");
    strcpy(auxjournaling.jorunal_tipo_operacion, "crear");
    // generar el tiempo
    time_t timecurrent;
    timecurrent = time(NULL);
    auxjournaling.journal_fecha = timecurrent;
    // se escribe
    fseek(file, inicio_jour, SEEK_SET);
    fwrite(&auxjournaling, sizeof(JOURNALING), 1, file);
    // se separa las carpetas
    string path = filePath(_directorio);
    string s = path;
    string delimitador = "/";
    string namefile = filename2(_directorio);
    int cont = 0;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimitador)) != std::string::npos)
    {
        token = s.substr(0, pos);
        s.erase(0, pos + delimitador.length());
        if (cont != 0)
        {
            ListaCarpetasm.push_back(token);
        }
        cont++;
    }
    // sigue
    ListaCarpetasm.push_back(s);
    if (ListaCarpetasm.size() <= 0)
    {
        cout << "Error~> Directorio vacio." << endl;
        ListaCarpetasm.clear();
        return;
    }
    ListaCarpetasm.push_back("#"); // ultimo item para control
    int indiceinodo = 0;           // empezara en la carpeta raiz
    bool retorn = createRecursivom(file, &auxsuper, &ListaCarpetasm, &indiceinodo);
    if (!omitirarchivo && !retorn && ListaCarpetasm.size() > 0)
    {
        string texto = "";
        int indicearchivo = createFile(file, &auxsuper, "00000000");
        if (indicearchivo > 0)
        {
            linktofile(indiceinodo, indicearchivo, namefile, file, &auxsuper);
            indiceinodo = 0;
        }
        else
        {
            cout << "Error~> No se puede enlazar archivo" << endl;
        }
    }
    else
    {
        cout << "Error~> No se pudo crear el archivo" << endl;
    }
    ListaCarpetasm.clear();
}

void analisisMkfile(char comando[])
{
    int contador = 0;
    char lineacomando[100] = "";
    char _id[10] = "";
    char _path[100] = "";
    char _r[10] = "";
    bool gotR = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ')
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

        // validacion argumentos
        if (strcmp(lineacomando, "mkfile") == 0)
        {
            cout << "sistema~> analizando mkfile "<< endl;
            memset(lineacomando, 0, 100);
            contador++;
        }
        else if (strcmp(lineacomando, ">path=") == 0)
        {
            memset(lineacomando, 0, 100);
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
            cout << "sistema~> ruta ingresada para crear archivo: " << _path << endl;
        }
        else if (strcmp(lineacomando, ">r") == 0)
        {
            memset(lineacomando, 0, 100);
            gotR = true;
            cout << "sistema~> se crearan las carpetas necesarias para crear el archivo." << endl;
        }
    }

    // para mandar los valores
    bool sesionIniciada = validacionLogin();
    if (!sesionIniciada)
    {
        cout << "Error~> se necesita inicar sesion para ejecutar este comando." << endl;
        return;
    }
    if (gotR)
    {
        R = true;
        cout << "sistema~> creando folders necesarios..." << endl;
        string idAux = getId();
        strcpy(_id, idAux.c_str()); // obtengo el id de la particion montada
        DISCO disco = buscarDisco(_id, 3);
        ejecutarMkfile(disco, _id, _path, disco.path);
    }
    else
    {
        string idAux = getId();
        strcpy(_id, idAux.c_str()); // obtengo el id de la particion montada
        DISCO disco = buscarDisco(_id, 3);
        ejecutarMkfile(disco, _id, _path, disco.path);
    }
}
