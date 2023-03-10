//
// Created by gerson on 24/02/23.
//
#include "mkfs.h"

// ext2 porque no tiene journaling
void crearEXT2(DISCO disco, char _id[], char path[])
{
    // abrimos el archivo
    FILE *file;
    file = fopen(path, "rb+");
    // Se leer el mbr
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file); // se leer el mbr
    // busco la particion en disco fisico
    int indice = 0;
    string nombre_buscar;
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(disco.mbr_particion[i].id, _id) == 0)
        { // encuetro la particion
            nombre_buscar = charToString(disco.mbr_particion[i].part_name);
            break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(mbr->mbr_particion[i].part_name, nombre_buscar.c_str()) == 0)
        {
            indice = i;
            break;
        }
    }
    // se calcula la n, donde n es el numero de inodos
    int n = (mbr->mbr_particion[indice].part_s - sizeof(SUPER_BLOQUE)) / (4 + sizeof(INODOS) + 3 * sizeof(BLOQUECARPETA));
    n = (int)floor(n);

    // validacion para saber si la particion es capaz de soportar el ext
    if (mbr->mbr_particion[indice].part_s < 1000)
    { // debe tener la menos 1kb
        cout << "Error~> El tamano de la partición es demasiado pequenio para crear el sistema ext2" << endl;
        return;
    }
    // se inicia formato
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    for (int i = mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1; i < mbr->mbr_particion[indice].part_s; ++i)
    {
        fwrite("0", 1, 1, file);
        fseek(file, i, SEEK_SET);
    }

    // se inicializa super bloque
    SUPER_BLOQUE super;
    super.s_filesystem_type = 3;
    super.s_inode_count = n;
    super.s_block_count = 3 * n;
    super.s_free_blocks_count = 3 * n; // restar despues
    super.s_free_inodes_count = n;     // restar despues
    time_t time_mount;
    time_mount = time(NULL);
    super.s_mtime = time_mount;
    super.s_mnt_count++;
    super.s_magic = 0xEF53;
    super.s_inode_s = sizeof(INODOS);
    super.s_block_s = sizeof(BLOQUECARPETA);
    super.s_first_ino = 2;
    super.s_first_blo = 2;
    super.s_bm_inode_start = mbr->mbr_particion[indice].part_start + sizeof(SUPER_BLOQUE) + 1;
    super.s_bm_block_start = super.s_bm_inode_start + n + 1;
    super.s_inode_start = super.s_bm_block_start + (3 * n) + 1;
    super.s_block_start = super.s_inode_start + (n * sizeof(INODOS)) + 1;

    // escribir superbloque
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fwrite(&super, sizeof(SUPER_BLOQUE), 1, file);

    // creacion bitmap de inodos y bloques
    char bitmap_inodos[n];
    char bitmap_bloques[3 * n];
    int start_nodos = mbr->mbr_particion[indice].part_start + sizeof(SUPER_BLOQUE) + 1;
    for (int i = 0; i < n; ++i)
    {
        bitmap_inodos[i] = '0';
    }
    bitmap_inodos[0] = '1'; // carpeta raiz

    // se escribe el bitmap de inodos
    fseek(file, start_nodos, SEEK_SET);
    fwrite(&bitmap_inodos, sizeof(char), n, file);

    int start_bloques = start_nodos + n + 1;
    for (int i = 0; i < 3 * n; ++i)
    {
        bitmap_bloques[i] = '0';
    }
    bitmap_bloques[0] = '1'; // 1 = carpeta, 2 = archivo
    // se escribe el bitmap de bloques
    fseek(file, start_bloques, SEEK_SET);
    fwrite(&bitmap_bloques, sizeof(char), 3 * n, file);

    // aux para leer el super bloque
    SUPER_BLOQUE auxsuper;
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fread(&auxsuper, sizeof(SUPER_BLOQUE), 1, file);

    // se crea el inodo raiz
    INODOS raiz;
    raiz.i_uid = 1;
    raiz.i_gid = 1;
    raiz.i_s = n * sizeof(INODOS);
    time_t fecha_c;
    fecha_c = time(NULL);
    raiz.i_ctime = fecha_c;
    raiz.i_atime = fecha_c;
    raiz.i_mtime = fecha_c;
    // se crean los 15 espacios de apuntadores
    for (int i = 0; i < 15; ++i)
    {
        raiz.i_block[i] = -1;
    }
    raiz.i_block[0] = 0;
    raiz.i_type = '0'; // es carpeta
    // bloque carpeta raiz
    BLOQUECARPETA carpetaraiz;
    CONTENT contentraiz;
    strcpy(contentraiz.b_name, ".");        // carpeta actual
    contentraiz.b_inodo = 0;                // apuntador al inodo que pertenece
    carpetaraiz.b_content[0] = contentraiz; // guardamos en la carpeta raiz
    strcpy(contentraiz.b_name, "..");
    contentraiz.b_inodo = 0;
    carpetaraiz.b_content[1] = contentraiz;
    // se escribe el inodo
    fseek(file, auxsuper.s_inode_start, SEEK_SET);
    fwrite(&raiz, sizeof(INODOS), 1, file);
    auxsuper.s_free_inodes_count--; // se disminuye
    // escribe carpeta raiz
    fseek(file, auxsuper.s_block_start, SEEK_SET);
    fwrite(&carpetaraiz, sizeof(BLOQUECARPETA), 1, file);
    auxsuper.s_free_blocks_count--; // se disminuye

    // rescribo el SUPERBLOQUE
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fwrite(&auxsuper, sizeof(SUPER_BLOQUE), 1, file);
    fclose(file);
    cout << "sistema~> particion formateada como EXT2 correctamente. :D" << endl;
}

// ext3 porque tiene journaling
void crearEXT3(DISCO disco, char _id[], char path[])
{
    // abrimos el archivo
    FILE *file;
    file = fopen(path, "rb+");
    // Se leer el mbr
    MBR *mbr = (MBR *)malloc(sizeof(MBR));
    fseek(file, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, file); // se leer el mbr
    // busco la particion en disco fisico
    int indice = 0;
    string nombre_buscar;
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(disco.mbr_particion[i].id, _id) == 0)
        { // encuetro la particion
            nombre_buscar = charToString(disco.mbr_particion[i].part_name);
            break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (strcmp(mbr->mbr_particion[i].part_name, nombre_buscar.c_str()) == 0)
        {
            indice = i;
            break;
        }
    }
    // se calcula la n, donde n es el numero de inodos
    int n = (mbr->mbr_particion[indice].part_s - sizeof(SUPER_BLOQUE)) / (4 + sizeof(JOURNALING) + sizeof(INODOS) + 3 * sizeof(BLOQUECARPETA));
    n = (int)floor(n);

    // validacion para saber si la particion es capaz de soportar el ext
    if (mbr->mbr_particion[indice].part_s < 1000)
    { // debe tener la menos 1kb
        cout << "Error~> El tamano de la partición es demasiado pequenio para crear el sistema ext3" << endl;
        return;
    }
    // se inicia formato
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    for (int i = mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1; i < mbr->mbr_particion[indice].part_s; ++i)
    {
        fwrite("0", 1, 1, file);
        fseek(file, i, SEEK_SET);
    }

    // se inicializa super bloque
    SUPER_BLOQUE super;
    super.s_filesystem_type = 3;
    super.s_inode_count = n;
    super.s_block_count = 3 * n;
    super.s_free_blocks_count = 3 * n; // restar dspues
    super.s_free_inodes_count = n;     // restar despues
    time_t time_mount;
    time_mount = time(NULL);
    super.s_mtime = time_mount;
    super.s_mnt_count++;
    super.s_magic = 0xEF53;
    super.s_inode_s = sizeof(INODOS);
    super.s_block_s = sizeof(BLOQUECARPETA);
    super.s_first_ino = 2;
    super.s_first_blo = 2;
    super.s_bm_inode_start = mbr->mbr_particion[indice].part_start + sizeof(SUPER_BLOQUE) + (n * sizeof(JOURNALING)) + 1;
    super.s_bm_block_start = super.s_bm_inode_start + n + 1;
    super.s_inode_start = super.s_bm_block_start + (3 * n) + 1;
    super.s_block_start = super.s_inode_start + (n * sizeof(INODOS)) + 1;

    // escribir superbloque
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fwrite(&super, sizeof(SUPER_BLOQUE), 1, file);

    // creacion bitmap de inodos y bloques
    char bitmap_inodos[n];
    char bitmap_bloques[3 * n];
    int start_nodos = mbr->mbr_particion[indice].part_start + sizeof(SUPER_BLOQUE) + (n * sizeof(JOURNALING)) + 1; // 1
    for (int i = 0; i < n; ++i)
    {
        bitmap_inodos[i] = '0';
    }
    bitmap_inodos[0] = '1'; // carpeta raiz

    // se escribe el bitmap de inodos
    fseek(file, start_nodos, SEEK_SET);
    fwrite(&bitmap_inodos, sizeof(char), n, file);

    int start_bloques = start_nodos + n + 1;
    for (int i = 0; i < 3 * n; ++i)
    {
        bitmap_bloques[i] = '0';
    }
    bitmap_bloques[0] = '1'; // 1 = carpeta, 2 = archivo
    // se escribe el bitmap de bloques
    fseek(file, start_bloques, SEEK_SET);
    fwrite(&bitmap_bloques, sizeof(char), 3 * n, file);

    // escribo el journaling
    int start_journaling = mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + sizeof(SUPER_BLOQUE) + 1; // 1
    JOURNALING jour;
    jour.jorunal_tipo_operacion[0] = '-';
    jour.journal_contenido[0] = '-';
    jour.journal_nombre[0] = '-';
    jour.journal_persimo = '-';
    jour.journal_propietario[0] = '-';
    jour.journal_tipo = '-';
    // se escribe el journaling
    fseek(file, start_journaling, SEEK_SET);
    fwrite(&jour, sizeof(JOURNALING), 1, file);

    // aux para leer el super bloque
    SUPER_BLOQUE auxsuper;
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fread(&auxsuper, sizeof(SUPER_BLOQUE), 1, file);

    // se crea el inodo raiz
    INODOS raiz;
    raiz.i_uid = 1;
    raiz.i_gid = 1;
    raiz.i_s = n * sizeof(INODOS);
    time_t fecha_c;
    fecha_c = time(NULL);
    raiz.i_ctime = fecha_c;
    raiz.i_atime = fecha_c;
    raiz.i_mtime = fecha_c;
    // se crean los 15 espacios de apuntadores
    for (int i = 0; i < 15; ++i)
    {
        raiz.i_block[i] = -1;
    }
    raiz.i_block[0] = 0;
    raiz.i_type = '0'; // es carpeta
    // bloque carpeta raiz
    BLOQUECARPETA carpetaraiz;
    CONTENT contentraiz;
    strcpy(contentraiz.b_name, ".");        // carpeta actual
    contentraiz.b_inodo = 0;                // apuntador al inodo que pertenece
    carpetaraiz.b_content[0] = contentraiz; // guardamos en la carpeta raiz
    strcpy(contentraiz.b_name, "..");
    contentraiz.b_inodo = 0;
    carpetaraiz.b_content[1] = contentraiz;
    // se escribe el inodo
    fseek(file, auxsuper.s_inode_start, SEEK_SET);
    fwrite(&raiz, sizeof(INODOS), 1, file);
    auxsuper.s_free_inodes_count--; // se disminuye
    // escribe carpeta raiz
    fseek(file, auxsuper.s_block_start, SEEK_SET);
    fwrite(&carpetaraiz, sizeof(BLOQUECARPETA), 1, file);
    auxsuper.s_free_blocks_count--; // se disminuye

    // rescribo el SUPERBLOQUE
    fseek(file, mbr->mbr_particion[indice].part_start + sizeof(mbr->mbr_particion[indice]) + 1, SEEK_SET);
    fwrite(&auxsuper, sizeof(SUPER_BLOQUE), 1, file);
    fclose(file);
    cout << "sistema~> particion formateada como EXT3 correctamente. :D" << endl;
}

void formatoMkfs(char _id[], char _type[], int sys)
{
    DISCO disco = buscarDisco(_id, sys);
    if (disco.mbr_tamano == 0)
    {
        cout << "Error~> No se ha econtrado la particion con el id ingresado. Por favor, compruebe el id." << endl;
        return;
    }
    // tipo de formateo
    if (strcmp(_type, "full") != 0)
    {
        cout << "Error~> Tipo de formateo desconocido. Intete con >full." << endl;
        return;
    }
    // miramos si existe el disco
    bool existeDisco = existeArchivo(disco.path);
    if (!existeDisco)
    {
        cout << "Error~> No se ha encontrado la ruta del disco." << endl;
        return;
    }
    // siempre es full, sino se especifica
    cout << "sistema~> creando sistema de archivos..." << endl;
    if (sys == 2)
    {
        crearEXT2(disco, _id, disco.path);
    }
    else
    {
        crearEXT3(disco, _id, disco.path);
    }
}

void analisisMkfs(char comando[])
{
    int contador = 0;
    char lineacomando[200] = "";

    char _id[10] = "";
    char _type[10] = ""; // full
    char _fs[4] = {'2', 'f', 's'};

    bool gotType = false;
    bool gotId = false;

    while (comando[contador] != NULL)
    {
        if (comando[contador] == ' ' || comando[contador] == '\n')
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

        // para validar los argumentos
        if (strcmp(lineacomando, "mkfs") == 0)
        {
            cout << "sistema~> analizando particion para crear sistema de archivos..."<< endl;
            memset(lineacomando, 0, 200);
            contador++;
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
                    aux[0] = comando[contador]; // se quita el tolower ya que ID
                    strncat(_id, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> la particion a analizar tine el siguiente id: " << _id << endl;
        }
        else if (strcmp(lineacomando, ">type=") == 0)
        {
            memset(lineacomando, 0, 200);
            gotType = true;
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
                    strncat(_type, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~> tipo de formateo a aplicar: " << _type << endl;
        }
        else if (strcmp(lineacomando, ">fs=") == 0)
        {
            memset(lineacomando, 0, 200);
            memset(_fs, 0, 4);
            // flag_fs=true;
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
                    strncat(_fs, aux, 1);
                    contador++;
                }
            }
            cout << "sistema~>  sistema de archivos a implementar: " << _fs << endl;
        }
    }
    if (!gotType)
    {   // si no tiene type se lo ponemos nosotros ya que es opcional
        strcpy(_type, "full");
        cout << "sistema~> Tipo de formateo a aplicar: full" << endl;
    }
    if (!gotId)
    {
        cout << "Error~> No se puede encontrar la particion sin un id." << endl;
        return;
    }

    // ahora veremos si es ext2 o ext3
    if (strcmp(_fs, "2fs") == 0)
    {
        cout << "sistema~> formateando y creando EXT2..." << endl;
        formatoMkfs(_id, _type, 2);
    }
    else if (strcmp(_fs, "3fs") == 0)
    {
        cout << "sistema~> formateando y creando EXT3..." << endl;
        formatoMkfs(_id, _type, 3);
    }
}