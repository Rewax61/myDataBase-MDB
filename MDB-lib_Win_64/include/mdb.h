#ifndef _MDB_H_
#define _MDB_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Definition of error variables */
#define INIT_NOT_DONE 1
#define DB_ALREADY_EXISTS 2
#define TABLE_ALREADY_EXISTS 3
#define ERR_CREATING_COL 4
#define DB_NOT_EXISTS 5
#define TABLE_NOT_EXISTS 6
#define COL_NOT_EXISTS 7
#define ERR_MEMORY 8
#define ERR_WRITING_FILE 9
#define ERR_READING_FILE 10
#define INVALID_VALUE 11

/* Database creating functions */
int MDB_Init();
int MDB_CreateDatabase(char const *name);
int MDB_CreateTable(char const *database, char const *name, char **columns, int const numberOfColumns);

/* Adding function */
int MDB_AddToCol(char const *database, char const *table, char const *column_name, char const *value);

/* Geting functions */
int MDB_GetNumOfElementsInCol(char const *database, char const *table, char const *column_name);
int MDB_GetItemInCol(char const *database, char const *table, char const *column_name, int const element_id, char *__buffer_dest);

/* Removing functions */
int remove_directory(char const *name);
int MDB_RemoveDataBase(char const *database);
int MDB_RemoveTable(char const *database, char const *table);

#endif