/* Searches and build macro declarations and prints it to a new .am file. 
All the macros are stored in a linked list and each node holds a macro,
which holds as an individual linked list. While the data in the each individual list is the 
the lines of the macro, up to 80 characters per line */


#include "pre_proccesor.h"

void pre_proccesor(FILE *file, char fileName[]){

    FILE *newFile = fopen(fileName, "w");
    if (newFile == NULL){
        printf("Create output failed");
    }

    Table *macroTable;
    macroTable = (Table *) malloc(sizeof(Table));
}