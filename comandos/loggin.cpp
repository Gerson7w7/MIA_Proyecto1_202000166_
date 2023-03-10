//
// Created by gerson on 27/02/23.
//

#include "loggin.h"

bool existeUsuario = false, seguir = true;
string USR = "", PWD = "", ID = ""; // usr=usuario pwd=contraseña id=id
UsuarioActual usuario;

// para retornar el ID del disco
string getId()
{
    return usuario.particion;
}

// funcion para saber si hay un usuario activo
bool validacionLogin()
{
    if (usuario.sesionIniciada)
    {
        return true;
    }
    return false;
}

void logout()
{
    if (!usuario.sesionIniciada)
    {
        cout << "Error~> No hay sesion iniciada." << endl;
        return;
    }
    usuario.sesionIniciada = false;
    usuario.UID = "";
    usuario.password = "";
    usuario.usuario = "";
    usuario.grupo = "";
    usuario.particion = "";
    cout << "sistema~> sesion cerrada." << endl;
}

string quitarEspacios(string cadena)
{
    string salir = "";

    for (int i = 0; i < cadena.length(); i++)
    {
        if ((cadena[i] >= 'a' && cadena[i] <= 'z') || (cadena[i] >= 'A' && cadena[i] <= 'Z') || (cadena[i] >= '0' && cadena[i] <= '9') || (cadena[i] == '\n') || (cadena[i] == ','))
        {
            salir += cadena[i];
        }
    }
    return salir;
}

// lee linea a linea el archivo de usuarios y luego las compara
string separarSalto(string cadena, UsuarioActual *objetoEstructura)
{
    string _cadena = cadena;
    string delimitador = "\n";
    size_t pos = 0;
    string token;
    while ((pos = _cadena.find(delimitador)) != std::string::npos)
    {
        token = _cadena.substr(0, pos);
        _cadena.erase(0, pos + delimitador.length());

        if (seguir)
        {
            separarComas(token, objetoEstructura);
        }
    }
    return _cadena;
}

// Iterar y comparar para comparar las credenciales
string separarComas(string cadena, UsuarioActual *objetoEstructura)
{
    string _cadena = cadena;
    string delimitador = ",";
    int contador = 0;
    bool comparar = false;
    string grupo1 = "", user1 = "", password1 = "", uid1 = "";
    // 1,G,root
    // 1,U,root,root,123
    size_t pos = 0;
    string token;
    while ((pos = _cadena.find(delimitador)) != string::npos)
    {
        contador++;
        token = _cadena.substr(0, pos);
        _cadena.erase(0, pos + delimitador.length());

        if (contador == 1)
        {
            uid1 = token;
        }
        else if (contador == 2 && (token == "U" || token == "u"))
        {
            comparar = true;
        }
        else if (contador == 3)
        {
            grupo1 = token;
        }
        else if (contador == 4)
        {
            user1 = quitarEspacios(token);
        }
        else if (contador == 5)
        {
            password1 = quitarEspacios(token);
        }
    }

    if (comparar)
    {
        cout << "iniciando sesion..." << endl;
        password1 = quitarEspacios(_cadena);
        if (user1 == USR)
        {
            if (PWD != password1)
            {
                cout << "Error~> contrasenia incorrecta." << endl;
                return _cadena;
            }
            cout << "sistema~> Bienvenido " << user1 << endl;
            // Usuario,Password,UID,Grupo,Particion;
            bool aux = seguir;
            aux = false;
            objetoEstructura->sesionIniciada = true;
            objetoEstructura->grupo = grupo1;
            objetoEstructura->password = password1;
            objetoEstructura->usuario = user1;
            objetoEstructura->UID = uid1;
            objetoEstructura->particion = ID;
            comparar = false;
            cout << "sistema~> Grupo: " << grupo1 << endl;
            cout << "sistema~> contrasenia: ******" << endl;
            cout << "sistema~> UID: " << uid1 << endl;
            cout << "sistema~> particion id: " << ID << endl;
            bool auxEs = &existeUsuario;
            auxEs = true;
            cout << "═══════════════════════════════════════════" << endl;
        }
    }
    return _cadena;
}

string readpointer(APUNTADORES bloqueapuntador, FILE *archivo, SUPER_BLOQUE superblock)
{
    string lectura = "";
    // Apartir de aca ya hemos extraido la informacion del blockeapuntador
    // procedemos a leer los indices de los 16 bloques directos
    for (int j = 0; j < 16; j++)
    {
        int indicearchivo = bloqueapuntador.b_apuntadores[j];
        if (indicearchivo != -1)
        {
            BLOQUEARCHIVO blockfile;
            fseek(archivo, superblock.s_block_start + indicearchivo * sizeof(BLOQUEARCHIVO), SEEK_SET);
            fread(&blockfile, sizeof(BLOQUEARCHIVO), 1, archivo);
            // Ya hemos obtenido el bloque de archivo
            // procedemos a concatenar la informacion
            for (int g = 0; g < 64; g++)
            {
                lectura += blockfile.b_content[g];
            }
        }
    }
    return lectura;
}

// lee punteros treples si hubieran
string readtriplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock)
{
    /*PASOS
        1. Ir al bloque 14 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros triple que contiene 16 indirectos
        4. Leer los 16 indlogin -usr->root -pass->123 -id->281 Directos uno a uno que contienen otro indirecto
        5. Obtener el indirecto
        6. Leer el texto de cada directo
        7. concatenar el texto
    */
    int indicepunterotriple = 14;
    string lecturapunterotriple = "";
    int indiceBloque = inodo.i_block[indicepunterotriple];
    if (indiceBloque != -1)
    {
        APUNTADORES bloqueapuntadortriple;
        fseek(archivo, superblock.s_block_start + indiceBloque * sizeof(APUNTADORES), SEEK_SET);
        fread(&bloqueapuntadortriple, sizeof(APUNTADORES), 1, archivo);
        // Apartir de aca ya hemos extraido la informacion del bloquepunterotriple
        // procedemos a leer los indices de los 16 bloques indirectos
        for (int j = 0; j < 16; j++)
        {
            int indiceapuntadorindirecto = bloqueapuntadortriple.b_apuntadores[j];
            if (indiceapuntadorindirecto != -1)
            {
                APUNTADORES bloqueapuntadorIndirecto;
                fseek(archivo, superblock.s_block_start + indiceapuntadorindirecto * sizeof(APUNTADORES), SEEK_SET);
                fread(&bloqueapuntadorIndirecto, sizeof(APUNTADORES), 1, archivo);
                // Ya obtuvimos el apuntador Indirecto
                // Procedemos a leer sus siguientes 16 apuntadores directos
                int indiceapuntadordirecto = bloqueapuntadorIndirecto.b_apuntadores[j];
                if (indiceapuntadordirecto != -1)
                {
                    for (int m = 0; m < 16; m++)
                    {
                        APUNTADORES bloqueapuntadordirecto;
                        fseek(archivo, superblock.s_block_start + indiceapuntadordirecto * sizeof(APUNTADORES), SEEK_SET);
                        fread(&bloqueapuntadordirecto, sizeof(APUNTADORES), 1, archivo);
                        lecturapunterotriple += readpointer(bloqueapuntadordirecto, archivo, superblock);
                    }
                }
            }
        }
    }
    return lecturapunterotriple;
}

// lee punteros dobles si hubieran
string readdoublepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock)
{
    /*PASOS
        1. Ir al bloque 13 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros doble que contiene 16 indirectos
        4. Leer los 16 indirectos uno a uno que contienen un directo
        5. Obtener el texto de cada directo
        6. concatenar el texto
    */
    int indicepunterodoble = 13;
    string lecturapunterodouble = "";
    int indiceBloque = inodo.i_block[indicepunterodoble];
    if (indiceBloque != -1)
    {
        APUNTADORES bloquepunterodoble;
        fseek(archivo, superblock.s_block_start + indiceBloque * sizeof(APUNTADORES), SEEK_SET);
        fread(&bloquepunterodoble, sizeof(APUNTADORES), 1, archivo);
        // Apartir de aca ya hemos extraido la informacion del bloquepunterodoble
        // procedemos a leer los indices de los 16 bloques indirectos
        for (int j = 0; j < 16; j++)
        {
            int indiceapuntadordirecto = bloquepunterodoble.b_apuntadores[j];
            if (indiceapuntadordirecto != -1)
            {
                APUNTADORES bloqueapuntadordirecto;
                fseek(archivo, superblock.s_block_start + indiceapuntadordirecto * sizeof(APUNTADORES), SEEK_SET);
                fread(&bloqueapuntadordirecto, sizeof(APUNTADORES), 1, archivo);
                // Ya obtuvimos el apuntador directo que obtiene informacion
                // Procedemos a leer
                lecturapunterodouble += readpointer(bloqueapuntadordirecto, archivo, superblock);
            }
        }
    }
    return lecturapunterodouble;
}

// lee punteros simples
string readsimplepointer(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock)
{
    int indicepunterosimple = 12;
    string lecturapunterosimple = "";
    /*PASOS
        1. Ir al bloque 12 del inodo
        2. Verificar que este ocupado
        3. Obtener el bloque de punteros debido a que es simple los apuntadores del apuntador son directos
        4. Ir leyendo los 16 bloques que contienen texto
        5. concatenar el texto
    */
    int indiceBloque = inodo.i_block[indicepunterosimple]; // indice del bloque indirecto simple
    if (indiceBloque != -1)
    {                                    // El indice del puntero simple debe estar ocupado
        APUNTADORES bloquepunterosimple; // Bloque de apuntador a buscar
        fseek(archivo, superblock.s_block_start + indiceBloque * sizeof(APUNTADORES), SEEK_SET);
        fread(&bloquepunterosimple, sizeof(APUNTADORES), 1, archivo);
        // Iniciamos la lectura del bloque de apuntadores
        lecturapunterosimple += readpointer(bloquepunterosimple, archivo, superblock);
    }
    return lecturapunterosimple;
}

// función para leer un archivo
string leerArchivoCompleto(INODOS inodo, FILE *archivo, SUPER_BLOQUE superblock)
{
    string archivotexto = "";
    // leemos apuntadores directos
    for (int i = 0; i < 12; i++)
    {
        int indiceBloque = inodo.i_block[i];
        if (indiceBloque != -1)
        {
            BLOQUEARCHIVO blockfile;
            fseek(archivo, superblock.s_block_start + indiceBloque * sizeof(BLOQUEARCHIVO), SEEK_SET);
            fread(&blockfile, sizeof(BLOQUEARCHIVO), 1, archivo);
            for (int j = 0; j < 64; j++)
            {
                if (blockfile.b_content[j] != '\0')
                {
                    archivotexto += blockfile.b_content[j];
                }
            }
        }
    }
    cout << "lo que retornare: " << archivotexto << endl;
    //LEER APUNTADORES INDIRECTOS
    //archivotexto += readsimplepointer(inodo,archivo,superblock);
    //archivotexto += readdoublepointer(inodo,archivo,superblock);
    //archivotexto += readtriplepointer(inodo,archivo,superblock);
    return archivotexto;
}

void recorrido(DISCO disco, char _id[], char path[])
{
    FILE *file;
    file = fopen(path, "rb+");
    // Usuario,Password,UID,Grupo,Particion;
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file); // se leer el mbr
    // busco la particion en disco fisico
    int indice = 0;
    bool gotIndice = false;
    string nombre_buscar;
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(disco.mbr_particion[i].id, _id) == 0)
        {
            nombre_buscar = charToString(disco.mbr_particion[i].part_name);
            break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(mbr->mbr_particion[i].part_name, nombre_buscar.c_str()) == 0)
        {
            indice = i;
            gotIndice = true;
            break;
        }
    }
    if (gotIndice)
    {
        SUPER_BLOQUE auxsuperbloque;
        fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
        fread(&auxsuperbloque, sizeof(SUPER_BLOQUE), 1, file);

        if (auxsuperbloque.s_magic == 0)
        {
            cout << "Error~> No se ha encontrado un sistema de archivos definido. Por favor primero formatee la partcion con ext2 o ext3.";
            fclose(file);
            return;
        }
        INODOS inodoUsuario;
        fseek(file, auxsuperbloque.s_inode_start + sizeof(INODOS) + 1, SEEK_SET);
        fread(&inodoUsuario, sizeof(INODOS), 1, file);
        string filetext = leerArchivoCompleto(inodoUsuario, file, auxsuperbloque);
        cout << "\nArchivo de usuarios: " << endl;
        cout << "inicio de lectura-------- \n" << filetext;
        cout << "fin de lectura-------" << endl;
        cout << endl;
        separarSalto(filetext, &usuario); // antes de validar el el suario se manda a salto
        if (!existeUsuario)
        {
            cout << "Error~> El usuario no existe";
            return;
        }
    }
}

void validarLogin(char _id[])
{
    DISCO disco = buscarDisco(_id, 3);
    if (disco.mbr_tamano == 0)
    {
        cout << "Error~> No se ha encontrado la particion." << endl;
        return;
    }
    bool existeDisco = existeArchivo(disco.path);
    if (!existeDisco)
    {
        cout << "Error~> No se ha encontrado la ruta del disco." << endl;
        return;
    }
    cout << "sistema~> validando credenciales de usuario..." << endl;
    if (!(USR == "root" && PWD == "123"))
    {
        cout << "Error~> Credenciales incorrectas." << endl;
        return;
    }
    cout << "sistema~> Credenciales correctas" << endl;
    usuario.sesionIniciada = true;
    usuario.grupo = "";
    usuario.password = "123";
    usuario.usuario = "root";
    usuario.UID = "1";
    usuario.particion = _id;
    cout << "sistema~> Bienvenido root" << endl;
    //recorrido(disco,_id,disco.path);
}

void analisisLogin(char comando[])
{
    int contador = 0;
    char lineacomando[200] = "";
    char _usr[100] = "";
    char _pass[100] = "";
    char _id[50] = "";
    bool gotUsr = false;
    bool gotPass = false;
    bool gotId = false;

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
        if (strcmp(lineacomando, "login") == 0)
        {
            cout << "sistema~> analizando login." << endl;
            memset(lineacomando, 0, 200);
            contador++;
        }
        else if (strcmp(lineacomando, ">user=") == 0)
        {
            memset(lineacomando, 0, 200);
            gotUsr = true;
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
                    strncat(_usr, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> usuario ingresado: " << _usr << endl;
        }
        else if (strcmp(lineacomando, ">pass=") == 0)
        {
            memset(lineacomando, 0, 200);
            gotPass = true;
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
                    strncat(_pass, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> contrasenia ingresada: ******" << endl;
        }
        else if (strcmp(lineacomando, ">id=") == 0)
        {
            memset(lineacomando, 0, 200);
            gotId = true;
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
                    strncat(_id, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> id ingresado: " << _id << endl;
        }
    }

    if (!gotId)
    {
        cout << "Error~> Se necesita especificar el id de la particion montada." << endl;
        return;
    }
    else if (!gotUsr)
    {
        cout << "Error~> Se necesita especificar el usuario." << endl;
        return;
    }
    else if (!gotPass)
    {
        cout << "Error~> Se necesita especificar la contrasenia." << endl;
        return;
    }

    if (usuario.sesionIniciada)
    {
        cout << "Error~> Existe una sesion abierta, si desea ingresar con otra cuenta debe de cerrar la actual." << endl;
        return;
    }
    usuario.sesionIniciada = false;
    usuario.UID = "";
    usuario.password = "";
    usuario.usuario = "";
    usuario.grupo = "";
    usuario.particion = "";
    USR = charToString(_usr);
    PWD = charToString(_pass);
    ID = charToString(_id);
    validarLogin(_id);
}
