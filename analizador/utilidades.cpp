//
// Created by gerson on 17/02/23.
//
#include "utilidades.h"

// para convertir array de char a string
string charToString(char *a)
{
    string s = a;
    return s;
}

// para validar el path del disco (si existe o no el archivo)
bool existeArchivo(char path[])
{   // si existe retornamos true, sino false
    FILE *file;
    if (file = fopen(path, "r"))
    {
        fclose(file);
        return true;
    }
    return false;
}

// para validar el tamaño del disco que sea mayor a 0
bool validacionSize(char valor[])
{
    string sizeSTR = charToString(valor);
    int size = stoi(sizeSTR);
    if (size > 0)
    {
        return true;
    }
    cout << "Error. El size del disco debería ser mayor a 0." << endl;
    return false;
}

// funcion para validar que el path sea correcto (solo la ruta sin el archivo)
bool validacionPath(string path)
{
    DIR *directorio; // puntero de un directorio
    if (directorio = opendir(path.c_str()))
    {
        // aquí el directorio si existe siuuu
        closedir(directorio);
        return true;
    }
    else
    {
        // aquí no :c, así que lo creamos xd
        closedir(directorio);
        cout << "El directorio no existe:( " << endl;
        return false;
    }
}