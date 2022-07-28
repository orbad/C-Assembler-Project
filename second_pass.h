#include "header.h"

/*this object will hold the data lines of the ob file*/
typedef struct data_List {
    string dLine;
} dataList;

/*this object will store the extern labels addresses, for the externals file*/
typedef struct extern_addresses {
    string name;
    int base;
    int offset;

} extAddress;

/*enums for navigating through the different binary translating processes*/
enum operation{funct_op=15, source_reg_op=11, source_add_op=7, dest_reg_op=5, dest_add_op=1};

enum operation_length{funct_len=4, reg_len=4, add_len=2};

enum addressing{immediate = 0 , direct=1, index=2, reg_direct=3 };

enum full_binary_translation{binary_line_len=16, binary_line_ind=15 };

/*for finding the indexes in the value array */
enum values_Array_index{errorFlag_ind = 0 , printCounter_ind=1 ,extAdd_ind=2, value_Array_Size=3};


/*functions for translating to bits, handling each operand and each line functions */

int firstWordProcess(int action, char line[], FILE *newCode, int printCounter);

void bitTranslation(int bitLength, int index, int value, char line[]);

int stringTranslation(char * token, dataList *dataLine,int DC);

int dataTranslation(char * token, dataList *dataLine,int DC);

int* middleWordProcess(char *firstOperand, char *secondOperand, int funct, symbol *symTable, int symTableSize, FILE *newCode,extAddress *extAdd,int extSize, int printCounter,int errorFlag);

int operandTranslation(char * operand, char line[], int isSource, symbol *symTable, int symTableSize,extAddress *extAdd,int printCounter,int *extSize,int *errorFlag);

int labelWordsProcess(int base, int offset, int source, FILE *newCode, int printCounter);

int immediateWordProcess(char *operand, FILE *newCode, int printCounter);

/*extern and entries making files functions*/

void updateExtAdd(string label, int address, extAddress *extAdd,int extSize);

void makeExtFile(string fileName,extAddress *extAdd,int extSize);

void makeEntFile(string fileName,symbol *symTable,dynamicMem *memVariables);



