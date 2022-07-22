#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#define LINE_LENGTH 81
#define LABEL_LENGTH 31
#define BINARY_LENGTH 11
#define FALSE 0
#define TRUE 1
#define DEF_VAL 0
#define NEUTRAL -1
#define OPS_FIRST_VALUE 100


typedef char String[LINE_LENGTH];


/*will hold the system reserved words to be checked all over the program*/
typedef struct system_reserved_words {
    char* words;
} sysReserved;

typedef struct language_reserved{
    char* letters;
} langReserved;

/*will hold the actions names and opcodes*/
typedef struct action_table {
    char* name;
    int opcode ;
} action;

/*will hold the dynamic values collected all over the program, like sizes and flags*/
typedef struct dynamic_values_and_error_flags {
    int num;
} dynamicMem;

/*a symbol table */
typedef struct Symbol {
    String name;
    int address;
    int base;
    int offset;
    int source; /*A\R\E */

} symbol;

/*a "father" object that holds all the new objects to be sent to each pass */
typedef struct Static_Objects {
    sysReserved *memory;
    dynamicMem *memVariables;
    action *actionTable;

} object;

/*to navigate through the objects*/

enum symbol_source{A = 0, R=2, E=1 };

enum dynamic_values_flags{symTable_size = 0, DC_size, IC_size, err_flag};

enum staticSizes{sys_res_size = 31, instructions_size=16, memVariables_size = 4};



int main(int argc, char *argv[]);

void pre_processor(FILE *sFile, char *fileName);

symbol* first_pass(char *fileName, object *objects);

void second_pass (char *fileName, symbol * symTable,object *objects);


/*token process functions */

void removeTails(char * line);

/*action table functions */
void tableBuild(action *opcode);

/*Project's 32-base language*/
void langBuild(langReserved *base);

void memoryBuild(sysReserved *memory);

int findAction(char* token, action table[] );

/*symbol table functions */

int isSymbol(String token, symbol * symTable, int tableSize);

/*memory holder functions */

int memCheck(char* token, sysReserved *memory );

/*conversion functions */
int extractNumber (char * word);

void specialBasePrint(char reversedLine[], FILE *file, int printCounter);
