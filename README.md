# **MDB**

<img src="./logo.ico" alt="logo" width="80"> <!--Logo-->  

*Developed by **Rewax***

## Description
The MDB library (myDataBase) is a library created to facilitate the management of databases in C language. It allows you to easily create, modify, remove a database. The main advantage it offers is that you don't need to take care of the management of the files containing the data, MDB does that for you.

## Installation
Download the library according to your system:  
* [Window (64 bits)](https://rewax61.alwaysdata.net/MDB-lib_Win_64.zip)
* [Linux (64 bits)](https://rewax61.alwaysdata.net/MDB-lib_Linux_64.zip)

## List of available functions

`MDB_Init(void)` : Initialize the library (required before using another MDB function).
* Return value :  
    - 0 if all went well

`MDB_CreateDatabase(char * name)` : Create the database with its name as parameter.
* Return value :
    - 0 if all went well
    - -1 if there is an error. The errno variable is set to the value of the error:
        - INIT_NOT_DONE if the initialization was not performed (with MDB_INIT())
        - DB_ALREADY_EXISTS if the database already exists
        - ERR_MEMORY if there is a memory error

`MDB_CreateTable(char * database, char * name, char ** columns, int numberOfColumns)` : Create a table by specifying the database name, the table name and the number of columns.
* Return value :
    - 0 if all went well
    - -1 if there is an error. The errno variable is set to the value of the error:
        - INIT_NOT_DONE if the initialization was not performed (with MDB_INIT())
        - TABLE_ALREADY_EXISTS if the table already exists
        - ERR_CREATING_COL if there is an error when creating the column file
        - ERR_MEMORY if there is a memory error

`MDB_AddToCol(char * database, char * table, char * column, char * value)` : Add an element to a column by specifying the database, table and column.
* Return value :
    - 0 if all went well
    - -1 if there is an error. The errno variable is set to the value of the error:
        - INIT_NOT_DONE if the initialization was not performed (with MDB_INIT())
        - DB_NOT_EXISTS if the database does not exist
        - TABLE_NOT_EXISTS if the table does not exist
        - COL_NOT_EXISTS if the column does not exist
        - ERR_WRITING_FILE if there is an error when writing a file
        - INVALID_VALUE if the values passed in parameter are not correct
        - ERR_MEMORY if there is a memory error

`MDB_GetNumOfElementsInCol(char *database, char *table, char *column_name)` : Get the number of items in a column by specifying the database, table and column.
* Return value :
    - 0 if all went well
    - -1 if there is an error. The errno variable is set to the value of the error:
        - INIT_NOT_DONE if the initialization was not performed (with MDB_INIT())
        - DB_NOT_EXISTS if the database does not exist
        - TABLE_NOT_EXISTS if the table does not exist
        - COL_NOT_EXISTS if the column does not exist
        - ERR_READING_FILE if there is an error when reading a file
        - ERR_MEMORY if there is a memory error

`MDB_GetItemInCol(char *database, char *table, char *column_name, int element_id, char *__buffer_dest)` : Get an element from a column by specifying the database, table, column, its index and giving a destination buffer.
* Return value :
    - 0 if all went well
    - -1 if there is an error. The errno variable is set to the value of the error:
        - INIT_NOT_DONE if the initialization was not performed (with MDB_INIT())
        - DB_NOT_EXISTS if the database does not exist
        - TABLE_NOT_EXISTS if the table does not exist
        - COL_NOT_EXISTS if the column does not exist
        - ERR_READING_FILE if there is an error when reading a file
        - ERR_MEMORY if there is a memory error