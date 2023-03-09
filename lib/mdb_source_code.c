#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "mdb.h"

#define MAX_PATH_SIZE 60
#define MAX_DB_NB 20
#define MAX_COL_NB 50

typedef enum {False, True} MDB_bool;

MDB_bool variableVerified = False;
char pathDir[MAX_PATH_SIZE];

int MDB_Init() {
    #ifdef _WIN32
        if (variableVerified == False) {
            strcpy(pathDir, "C:\\myDataBase\\ressources\\");
            variableVerified = True;
        }
    #elif __linux__
        if (variableVerified == False) {
            char user[20];
            strcpy(user, getenv("USER"));
            strcpy(pathDir, "/home/");
            strcat(pathDir, user);
            strcat(pathDir, "/myDataBase/ressources/");
            variableVerified = True;
        }
    #elif __APPLE__
        if (variableVerified == False) {
            char user[20];
            strcpy(user, getenv("USER"));
            strcpy(pathDir, "/Users/");
            strcat(pathDir, user);
            strcat(pathDir, "/myDataBase/ressources/");
            variableVerified = True;
        }
    #endif

    DIR * repMDB = NULL;
    repMDB = opendir(pathDir);
    closedir(repMDB);
    if (repMDB != NULL) {
        return 0;
    }
    #ifdef _WIN32
        mkdir(pathDir);;
    #elif __linux__ || __APPLE__
        mkdir(pathDir, 0755);
    #endif

    return 0;
}

int MDB_CreateDatabase(char const *name) {
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Definition of the database path */
    size_t size_pathBase = strlen(pathDir) + strlen(name) + 1;
    char * pathBase = calloc(size_pathBase, sizeof(char));
    if (pathBase == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    pathBase[0] = '\0';
    strcpy(pathBase, pathDir);
    strcat(pathBase, name);

    /* Creation of the database */
    DIR * repDB = NULL;
    repDB = opendir(pathBase);
    closedir(repDB);
    if (repDB != NULL) {
        errno = DB_ALREADY_EXISTS;
        return -1;
    }
    
    #ifdef _WIN32
        mkdir(pathBase);
    #elif __linux__ || __APPLE__
        mkdir(pathBase, 0755);
    #endif
    
    free(pathBase);

    return 0;
}

int MDB_CreateTable(char const *database, char const *name, char **columns, int const numberOfColumns) {
    int i;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Definition of the table path */
    char *pathTable = malloc((strlen(DB_path) + strlen(name) + 2) * sizeof(char));
    if (pathTable == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    pathTable[0] = '\0';
    strcat(pathTable, DB_path);
    #ifdef _WIN32
        strcat(pathTable, "\\");
    #elif __linux__ || __APPLE__
        strcat(pathTable, "/");
    #endif
    strcat(pathTable, name);

    /* Creation of the table folder */
    DIR * repTable = opendir(pathTable);
    closedir(repTable);
    if (repTable != NULL) {
        errno = TABLE_ALREADY_EXISTS;
        return -1;
    }

    #ifdef _WIN32
        mkdir(pathTable);
    #elif __linux__ || __APPLE__
        mkdir(pathTable, 0755);
    #endif

    /* Creation of the columns files */
    for (i = 0; i < numberOfColumns; i++) {
        char *pathCol = malloc((strlen(pathTable) + strlen(columns[i]) + 1 + 5 + 1) * sizeof(char));
        if (pathCol == NULL) {
            errno = ERR_MEMORY;
            return -1;
        }
        pathCol[0] = '\0';
        strcpy(pathCol, pathTable);
        #ifdef _WIN32
            strcat(pathCol, "\\");
        #elif __linux__ || __APPLE__
            strcat(pathCol, "/");
        #endif
        strcat(pathCol, columns[i]);
        strcat(pathCol, ".data");

        FILE * fCol = fopen(pathCol, "wb");
        fclose(fCol);
        if (fCol == NULL) {
            printf("END\n");
            errno = ERR_CREATING_COL;
            return -1;
        }
        free(pathCol);
    }

    /* Creation of the info file */
    int pathInfo_len = strlen(pathTable) + 11;
    char *pathInfo = calloc(pathInfo_len, sizeof(char));
    pathInfo[0] = '\0';
    strcpy(pathInfo, pathTable);
    #ifdef _WIN32
        strcat(pathInfo, "\\info.info");
    #elif __linux__ || __APPLE__
        strcat(pathInfo, "/info.info");
    #endif
    FILE * info_file = fopen(pathInfo, "wb");
    if (info_file == NULL) {
        fclose(info_file);
        errno = ERR_WRITING_FILE;
        return -1;
    }
    fwrite(&numberOfColumns, sizeof(int), 1, info_file);
    for (i = 0; i < numberOfColumns; i++) {
        int col_len = strlen(columns[i]);
        fwrite(&col_len, sizeof(int), 1, info_file);
        fwrite(columns[i], sizeof(char), col_len, info_file);

        int n_items = 0;
        fwrite(&n_items, sizeof(int), 1, info_file);
    }
    fclose(info_file);
    free(pathInfo);
    free(DB_path);
    free(pathTable);
    
    return 0;
}

int MDB_AddToCol(char const *database, char const *table, char const *column_name, char const *value) {
    int i;
    int res;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Values verification */
    for (i = 0; i < strlen(value); i++) {
        if (value[i] < 32 || value[i] > 255) {
            errno = INVALID_VALUE;
            return -1;
        }
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        free(DB_path);
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Table verification */
    DIR * table_dir = NULL;
    char *table_path = malloc((strlen(DB_path) + strlen(table) + 2) * sizeof(char));
    if (table_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    table_path[0] = '\0';
    strcpy(table_path, DB_path);
    #ifdef _WIN32
        strcat(table_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(table_path, "/");
    #endif
    strcat(table_path, table);

    table_dir = opendir(table_path);
    closedir(table_dir);
    if (table_dir == NULL) {    
        free(table_path);
        errno = TABLE_NOT_EXISTS;
        return -1;
    }

    /* Column verification */
    char *col_path = malloc((strlen(table_path) + strlen(column_name) + 1 + 5 + 1) * sizeof(char));
    if (col_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    col_path[0] = '\0';
    strcpy(col_path, table_path);
    #ifdef _WIN32
        strcat(col_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(col_path, "/");
    #endif
    strcat(col_path, column_name);
    strcat(col_path, ".data");
    FILE * col_file = fopen(col_path, "ab");
    if (col_file == NULL) {
        free(col_path);
        errno = COL_NOT_EXISTS;
        fclose(col_file);
        return -1;
    }
    
    /* Writing into colummn */
    int val_len = strlen(value);
    fwrite(&val_len, sizeof(int), 1, col_file);

    fwrite(value, sizeof(char), val_len, col_file);

    fclose(col_file);

    /* Begin writing into info file */
    char *info_path = malloc((strlen(table_path) + 10 + 1) * sizeof(char));
    if (info_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    info_path[0] = '\0';
    strcpy(info_path, table_path);
    #ifdef _WIN32
        strcat(info_path, "\\info.info");
    #elif __linux__ || __APPLE__
        strcat(info_path, "/info.info");
    #endif
    //printf("\n%s\n\n", info_path);
    FILE * info_file = fopen(info_path, "rb");
    if (info_file == NULL) {
        free(info_path);
        fclose(info_file);
        errno = ERR_WRITING_FILE;
        return -1;
    }
    int n_tot_cols;
    fread(&n_tot_cols, sizeof(int), 1, info_file);
    char **tab_cols = malloc(sizeof(char) * n_tot_cols);
    if (tab_cols == NULL) {
        free(tab_cols);
        fclose(info_file);
        errno = ERR_WRITING_FILE;
        return -1;
    }
    int *n_item_in_col = malloc(sizeof(int) * n_tot_cols);
    if (n_item_in_col == NULL) {
        free(n_item_in_col);
        fclose(info_file);
        errno = ERR_WRITING_FILE;
        return -1;
    }

    for (i = 0; i < n_tot_cols; i++) {
        int item_len;
        fread(&item_len, sizeof(int), 1, info_file); /* Reading of the column name's size */
        tab_cols[i] = malloc((item_len + 1) * sizeof(char));
        if (tab_cols[i] == NULL) {
            free(tab_cols);
            fclose(info_file);
            errno = ERR_WRITING_FILE;
            return -1;
        }
        fread(tab_cols[i], sizeof(char), item_len, info_file); /* Reading of the column name */
        tab_cols[i][item_len] = '\0';
        fread(&(n_item_in_col[i]), sizeof(int), 1, info_file); /* Reading of number of items in column */
    }
    
    for (i = 0; i < n_tot_cols; i++) {
        if (strcmp(tab_cols[i], column_name) == 0) {
            n_item_in_col[i] += 1; /* Addition of one to number of items in column */
        }
    }
    fclose(info_file);

    FILE * info_file_writing = fopen(info_path, "wb");
    if (info_file_writing == NULL) {
        fclose(info_file_writing);
        errno = ERR_WRITING_FILE;
        return -1;
    }
    fwrite(&n_tot_cols, sizeof(int), 1, info_file_writing);
    for (i = 0; i < n_tot_cols; i++) {
        int len_item = strlen(tab_cols[i]);
        fwrite(&len_item, sizeof(int), 1, info_file_writing);
        fwrite(tab_cols[i], sizeof(char), len_item, info_file_writing);
        fwrite(&(n_item_in_col[i]), sizeof(int), 1, info_file_writing);
    }

    free(tab_cols);
    free(n_item_in_col);

    fclose(info_file_writing);
    /* End writing into info file */

    /* FILE * fic = fopen(col_path, "rb");
    int len;
    fread(&len, sizeof(int), 1, fic);

    char *tab = malloc( (len + 1) * sizeof(char));
    if (tab == NULL) {
        free(tab);
        fclose(fic);
        fprintf(stderr, "Erreur malloc\n");
        return -1;
    }
    fread(tab, sizeof(char), len, fic);
    tab[len] = '\0';
    printf("Longueur : %d | tab : %s\n", len, tab);

    free(tab);
    fclose(fic); */

    free(DB_path);
    free(table_path);
    free(col_path);
    free(info_path);

    return 0;
}

int MDB_GetNumOfElementsInCol(char const *database, char const *table, char const *column_name) {
    int i;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        free(DB_path);
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Table verification */
    DIR * table_dir = NULL;
    char *table_path = malloc((strlen(DB_path) + strlen(table) + 2) * sizeof(char));
    if (table_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    table_path[0] = '\0';
    strcpy(table_path, DB_path);
    #ifdef _WIN32
        strcat(table_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(table_path, "/");
    #endif
    strcat(table_path, table);

    table_dir = opendir(table_path);
    closedir(table_dir);
    if (table_dir == NULL) {
        free(table_path);
        errno = TABLE_NOT_EXISTS;
        return -1;
    }

    /* Reading info file */
    char *info_path = malloc((strlen(table_path) + 10 + 1) * sizeof(char));
    if (info_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    info_path[0] = '\0';
    strcat(info_path, table_path);
    #ifdef _WIN32
        strcat(info_path, "\\info.info");
    #elif __linux__ || __APPLE__
        strcat(info_path, "/info.info");
    #endif
    FILE * info_file = fopen(info_path, "rb");
    if (info_file == NULL) {
        free(info_path);
        fclose(info_file);
        errno = ERR_READING_FILE;
        return -1;
    }

    int n_tot_cols;
    fread(&n_tot_cols, sizeof(int), 1, info_file);
    char **tab_cols = malloc(sizeof(char) * n_tot_cols);
    if (tab_cols == NULL) {
        free(tab_cols);
        fclose(info_file);
        errno = ERR_MEMORY;
        return -1;
    }

    int *n_item_in_col = malloc(n_tot_cols * sizeof(int));
    if (n_item_in_col == NULL) {
        free(n_item_in_col);
        fclose(info_file);
        errno = ERR_MEMORY;
        return -1;
    }

    for (i = 0; i < n_tot_cols; i++) {
        int item_len;
        fread(&item_len, sizeof(int), 1, info_file); /* Reading of the column name's size */
        tab_cols[i] = malloc((item_len + 1) * sizeof(char));
        if (tab_cols[i] == NULL) {
            free(tab_cols);
            fclose(info_file);
            errno = ERR_MEMORY;
            return -1;
        }
        fread(tab_cols[i], sizeof(char), item_len, info_file); /* Reading of the column name */
        tab_cols[i][item_len] = '\0';
        fread(&(n_item_in_col[i]), sizeof(int), 1, info_file); /* Reading of number of items in column */
    }
    fclose(info_file);

    /* Verifications */
    int n_colItems = 0;
    MDB_bool col_exists = False;
    for (i = 0; i < n_tot_cols; i++) {
        if (strcmp(tab_cols[i], column_name) == 0) {
            col_exists = True;
            n_colItems = n_item_in_col[i];
        }
    }
    if (col_exists == False) {
        errno = COL_NOT_EXISTS;
        return -1;
    }
    if (n_colItems == 0) {
        errno = ERR_READING_FILE;
        return -1;
    }


    free(DB_path);
    free(table_path);
    free(tab_cols);
    free(n_item_in_col);

    return n_colItems;
}

int MDB_GetItemInCol(char const *database, char const *table, char const *column_name, int const element_id, char *__buffer_dest) {
    int i;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        free(DB_path);
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Table verification */
    DIR * table_dir = NULL;
    char *table_path = malloc((strlen(DB_path) + strlen(table) + 2) * sizeof(char));
    if (table_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    table_path[0] = '\0';
    strcpy(table_path, DB_path);
    #ifdef _WIN32
        strcat(table_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(table_path, "/");
    #endif
    strcat(table_path, table);

    table_dir = opendir(table_path);
    closedir(table_dir);
    if (table_dir == NULL) {
        free(table_path);
        errno = TABLE_NOT_EXISTS;
        return -1;
    }

    /* Reading info file */
    char *info_path = malloc((strlen(table_path) + 10 + 1) * sizeof(char));
    if (info_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    info_path[0] = '\0';
    strcat(info_path, table_path);
    #ifdef _WIN32
        strcat(info_path, "\\info.info");
    #elif __linux__ || __APPLE__
        strcat(info_path, "/info.info");
    #endif
    FILE * info_file = fopen(info_path, "rb");
    if (info_file == NULL) {
        free(info_path);
        fclose(info_file);
        errno = ERR_READING_FILE;
        return -1;
    }

    int n_tot_cols;
    fread(&n_tot_cols, sizeof(int), 1, info_file);
    char **tab_cols = malloc(sizeof(char) * n_tot_cols);
    if (tab_cols == NULL) {
        free(tab_cols);
        fclose(info_file);
        errno = ERR_MEMORY;
        return -1;
    }

    int *n_item_in_col = malloc(n_tot_cols * sizeof(int));
    if (n_item_in_col == NULL) {
        free(n_item_in_col);
        fclose(info_file);
        errno = ERR_MEMORY;
        return -1;
    }

    /* Getting the columns' names */
    for (i = 0; i < n_tot_cols; i++) {
        int item_len;
        fread(&item_len, sizeof(int), 1, info_file); /* Reading of the column name's size */
        tab_cols[i] = malloc((item_len + 1) * sizeof(char));
        if (tab_cols[i] == NULL) {
            free(tab_cols);
            fclose(info_file);
            errno = ERR_MEMORY;
            return -1;
        }
        fread(tab_cols[i], sizeof(char), item_len, info_file); /* Reading of the column name */
        tab_cols[i][item_len] = '\0';
        fread(&(n_item_in_col[i]), sizeof(int), 1, info_file); /* Reading of number of items in column */
    }
    fclose(info_file);

    /* Verifications */
    int n_colItems = 0;
    MDB_bool col_exists = False;
    for (i = 0; i < n_tot_cols; i++) {
        if (strcmp(tab_cols[i], column_name) == 0) {
            col_exists = True;
            n_colItems = n_item_in_col[i];
        }
    }
    if (col_exists == False) {
        errno = COL_NOT_EXISTS;
        return -1;
    }
    if (n_colItems == 0) {
        errno = ERR_READING_FILE;
        return -1;
    }

    /* Reading column file */
    char *col_path = malloc((strlen(table_path) + strlen(column_name) + 1 + 5 + 1) * sizeof(char));
    if (col_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    col_path[0] = '\0';
    strcpy(col_path, table_path);
    #ifdef _WIN32
        strcat(col_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(col_path, "/");
    #endif
    strcat(col_path, column_name);
    strcat(col_path, ".data");
    FILE * col_file = fopen(col_path, "rb");
    if (col_file == NULL) {
        free(col_path);
        fclose(col_file);
        errno = COL_NOT_EXISTS;
        return -1;
    }

    for (i = 0; i < n_colItems; i++) {
        int size_item;
        fread(&size_item, sizeof(int), 1, col_file);
        char *item = malloc(size_item + 1 * sizeof(char));
        if (item == NULL) {
            errno = ERR_MEMORY;
            return -1;
        }
        fread(item, sizeof(char), size_item, col_file);
        item[size_item] = '\0';

        if (i+1 == element_id) {
            strcpy(__buffer_dest, item);
            fclose(col_file);
            free(DB_path);
            free(table_path);
            free(info_path);
            free(tab_cols);
            free(n_item_in_col);
            free(col_path);
            return 0;
        }
    }

    /* This part is only executed if there was an error before */
    fclose(col_file);
    free(DB_path);
    free(table_path);
    free(info_path);
    free(tab_cols);
    free(n_item_in_col);
    free(col_path);
    
    errno = ERR_READING_FILE;
    return -1;
}

int MDB_RemoveDataBase(char const *database) {
    int i;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        free(DB_path);
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Database removing */
    char *command = malloc((strlen(DB_path) + 6 + 1) * sizeof(char));
    command[0] = '\0';
    strcpy(command, "rm -r ");
    strcat(command, DB_path);
    system(command);

    free(DB_path);
    free(command);
    return 0;
}

int MDB_RemoveTable(char const *database, char const *table) {
    int i;
    if (variableVerified == False) {
        errno = INIT_NOT_DONE;
        return -1;
    }

    /* Database verification */
    DIR * db_dir = NULL;
    char *DB_path = malloc((strlen(pathDir) + strlen(database) + 1) * sizeof(char));
    if (DB_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    DB_path[0] = '\0';
    strcpy(DB_path, pathDir);
    strcat(DB_path, database);

    db_dir = opendir(DB_path);
    closedir(db_dir);
    if (db_dir == NULL) {
        free(DB_path);
        errno = DB_NOT_EXISTS;
        return -1;
    }

    /* Table verification */
    DIR * table_dir = NULL;
    char *table_path = malloc((strlen(DB_path) + strlen(table) + 2) * sizeof(char));
    if (table_path == NULL) {
        errno = ERR_MEMORY;
        return -1;
    }
    table_path[0] = '\0';
    strcpy(table_path, DB_path);
    #ifdef _WIN32
        strcat(table_path, "\\");
    #elif __linux__ || __APPLE__
        strcat(table_path, "/");
    #endif
    strcat(table_path, table);

    table_dir = opendir(table_path);
    closedir(table_dir);
    if (table_dir == NULL) {
        free(table_path);
        errno = TABLE_NOT_EXISTS;
        return -1;
    }

    /* Table removing */
    char *command = malloc((strlen(table_path) + 6 + 1) * sizeof(char));
    command[0] = '\0';
    strcpy(command, "rm -r ");
    strcat(command, table_path);
    system(command);

    free(DB_path);
    free(table_path);
    return 0;
}