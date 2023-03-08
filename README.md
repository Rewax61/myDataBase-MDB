# **MDB**

<img src="./logo.ico" alt="logo" width="80"> <!--Logo-->  

*Développé par **Rewax***

## Description
La bibliothèque MDB (myDataBase) est une bibliothèque créée pour faciliter la gestion des base de données en language c. Elle vous permet de faciliment créer, modifier, supprimer une base de donées.  
Le principal avantage qu'elle offre est que vous n'avez pas besoin de vous occuper de la gestion des fichiers contenants les données, MDB fait cela pour vous.

## Installation
Téléchargez la bibliothèque selon votre système :  
* [Window (64 bits)](https://rewax61.alwaysdata.net/MDB-lib_Win_64.zip)
* [Linux (64 bits)](https://rewax61.alwaysdata.net/MDB-lib_Linux_64.zip)

## Liste des instructions disponibles

`MDB_Init(void)` : Initialiser la bibliothèque (obligatoire avant d'utiliser une autre fonction de MDB).  
Valeur de retour :  
* 0 si tout s'est bien passé

`MDB_CreateDatabase(char * name)` : Créer la base de données avec son nom en paramètre.
* Valeur de retour :
    - 0 si tout s'est bien passé
    - -1 s'il y a une erreur. La variable errno est définie à la valeur de l'erreur :
        - INIT_NOT_DONE si l'inittialisation n'a pas été effectuée (avec MDB_INIT())
        - DB_ALREADY_EXISTS si la base de donnes existe déjà
        - ERR_MEMORY s'il y a une erreur au niveau de la mémoire

`MDB_CreateTable(char * database, char * name, char ** columns, int numberOfColumns)` : Créer une table en indiquant le nom de la base de données, celui de la table et le nombre de colonnes.
* Valeur de retour :
    - 0 si tout s'est bien passé
    - -1 s'il y a une erreur. La variable errno est définie à la valeur de l'erreur :
        - INIT_NOT_DONE si l'inittialisation n'a pas été effectuée (avec MDB_INIT())
        - TABLE_ALREADY_EXISTS si la table existe déjà
        - ERR_CREATING_COL s'il y a une erreur lors de la création du fichier de la colonne
        - ERR_MEMORY s'il y a une erreur au niveau de la mémoire

`MDB_AddToCol(char * database, char * table, char * column, char * value)` : Ajouter un élément dans un colonne en indiquant la DB, la table et la colonne.
* Valeur de retour :
    - 0 si tout s'est bien passé
    - -1 s'il y a une erreur. La variable errno est définie à la valeur de l'erreur :
        - INIT_NOT_DONE si l'inittialisation n'a pas été effectuée (avec MDB_INIT())
        - DB_NOT_EXISTS si la base de données n'existe pas
        - TABLE_NOT_EXISTS si la table n'existe pas
        - COL_NOT_EXISTS si la colonne n'existe pas
        - ERR_WRITING_FILE s'il y a une erreur lors de l'écriture d'un fichier
        - INVALID_VALUE si les valeurs passées en paramètre ne sont pas correctes
        - ERR_MEMORY s'il y a une erreur au niveau de la mémoire

`MDB_GetNumOfElementsInCol(char *database, char *table, char *column_name)` : Obtenir le nombre d'éléments présents dans une colonne précise.
* Valeur de retour :
    - 0 si tout s'est bien passé
    - -1 s'il y a une erreur. La variable errno est définie à la valeur de l'erreur :
        - INIT_NOT_DONE si l'inittialisation n'a pas été effectuée (avec MDB_INIT())
        - DB_NOT_EXISTS si la base de données n'existe pas
        - TABLE_NOT_EXISTS si la table n'existe pas
        - COL_NOT_EXISTS si la colonne n'existe pas
        - ERR_READING_FILE s'il y a une erreur lors de la lecture d'un fichier
        - ERR_MEMORY s'il y a une erreur au niveau de la mémoire

`MDB_GetItemInCol(char *database, char *table, char *column_name, int element_id, char *__buffer_dest)` : Obtenir un élément d'une colonne en indiquant son indice
* Valeur de retour :
    - 0 si tout s'est bien passé
    - -1 s'il y a une erreur. La variable errno est définie à la valeur de l'erreur :
        - INIT_NOT_DONE si l'inittialisation n'a pas été effectuée (avec MDB_INIT())
        - DB_NOT_EXISTS si la base de données n'existe pas
        - TABLE_NOT_EXISTS si la table n'existe pas
        - COL_NOT_EXISTS si la colonne n'existe pas
        - ERR_READING_FILE s'il y a une erreur lors de la lecture d'un fichier
        - ERR_MEMORY s'il y a une erreur au niveau de la mémoire