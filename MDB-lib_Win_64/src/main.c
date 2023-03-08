#include <stdio.h>
#include <stdlib.h>
#include <mdb.h>

int main() {
    printf("Initialisation...\n");
    MDB_Init();

    printf("Creation d'une DB...\n");
    MDB_CreateDatabase("maSuperDB");

    printf("Creation d'une table...\n");
    char *cols[] = {"Col1", "Col2", "Col3"};
    MDB_CreateTable("maSuperDB", "table", cols, 3);

    return 0;
}