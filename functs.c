#include "header.h"


/* Removing the whitespaces at the end of each sentence */
void removeTails(char * line)
{
    int index, i;
    /* Set default index */
    index = NEUTRAL;

    /* Find last index of non-white space character */
    i = 0;
    while(line[i] != '\0')
    {
        if((line[i]!=' ')&& (line[i] != '\t') && (line[i] != '\n'))
            index= i;
        i++;
    }

    line[index + 1] = '\0';
}

/* Searching the action table list for actions */
int findAction(char* token, action table[] ) {
    int i;
    for (i = DEF_VAL; i < instructions_size; i++)
        if (!strcmp(token, table[i].name))   /* need to compare to macro names*/
            return i;
    return NEUTRAL;
}

/* Searching the system Reserved table list  */
int memCheck(char* token, sysReserved memory[] ) {
    int i;
    for (i = DEF_VAL; i < sys_res_size; i++)
        if (!strcmp(token, memory[i].words))
            return i;
    return NEUTRAL;

}

/* Getting a number out of a string*/
int extractNumber (char * word){
    char c;
    int negative = FALSE;
    int i,digit,num=0;
    for(i=0;i<strlen(word);i++) {
        c = word[i];
        if(c == '-') /*detecting this is a negative value*/
            negative=TRUE;
        else if (c >= '0' && c <= '9') /*to confirm it's a digit*/
        {
            digit = c - '0';
            num = num * 10 + digit;
        }
    }
    if (negative)
        num=num*-1;
    return num;
}

/* Checking if the token is a known symbol from the symbol table */
int isSymbol(string token, symbol * symTable, int tableSize){
    int i;
    for (i = 0; i <= tableSize; i++) {
        if (!strcmp(token, symTable[i].name))
            return i;
    }
    return NEUTRAL;
}


/* This method receives an integer (doesn't matter if positive or negative) from -1022 to 1023
    And provides a 2-length array which consists the number's representation in the project's 32-base */
char* specialBaseConverter(int val)
{
    static char conv[3] = { 0 };
    const char base[] = "!@#$%^&*<>abcdefghijklmnopqrstuv\0";
    int first, second;
    first = val & 31; /* First 5 bits */
    second = ((val & ~31) >> 5) & 31; /* second 5 bits */

    conv[0] = base[second];
    conv[1] = base[first];
    return conv;
}

/* The binary translation is being calculated the other way (left to right).
 * before printing we reverse it */
char *strReverse(char *str)
{
    char *c1, *c2;
    if (! str || ! *str)
        return str;
    /*changing each first char with last char */
    for (c1 = str, c2 = str + strlen(str) - 1; c2 > c1; ++c1, --c2)
    {
        *c1 ^= *c2;
        *c2 ^= *c1;
        *c1 ^= *c2;
    }
    return str;
}

void intTo32BasePrint(int printCounter, FILE *file ,int binaryValueInInt){

    fprintf(file ,"%s\t", specialBaseConverter(printCounter));
    fprintf(file,"%s\n", specialBaseConverter(binaryValueInInt));
}

void binaryTo32BasePrint(int printCounter , FILE *file ,char binaryString[]){
    int num1;
    char[] reverseBinaryString= strReverse(binaryString);
    num1=atoi(reverseBinaryString);
    // num1=atoi(binaryString); /* Check in tests whether strReverse is needed */
    int rem,base=1;
    int decimal_num=0;
    while (num1>0){
        rem=num1%10;
        decimal_num+= rem*base;
        num1/=10;
        base *=2;
    }
    fprintf(file, "%s\t", specialBaseConverter(printCounter));
    fprintf(file, "%s\n", specialBaseConverter(decimal_num));
}

int binaryConnection(operandBuilder a){
    int sum=0;
    sum+=(a.opcode<<opcode_add) + (a.source_operand<<source_add) + (a.destination_operand<<dest_add) + (a.ARE);
    return sum;
}

/* This is the main print to ob file functions.
 * a single binary line is being reversed and transformed to the project's 32 special base.
  */
void specialBasePrint(char reversedLine[], FILE *file, int printCounter) {
    int i = DEF_VAL, j = DEF_VAL, index = 1, counter = DEF_VAL;
    int num2;
    String fullLine;
    char *pt; /* For strtol functionality */
    char cleanBinary[] = "00000", binary[] = "00000", base32[3];
    char code[11] = "", newLine[MAX_BINARY]="0000000000\0";

    strcpy(newLine, reversedLine);
    strReverse(newLine);

    for (j = 0; j < 5; j++) {
        for (i = 0; i < 4; i++) {

            binary[i] = newLine[counter];
            counter++;
        }
        /* We take the number and convert it to the project's 32-base using sprintf and specialBaseConverter function*/
        num2 = strtol(binary, &pt, 2);
        sprintf(base32, "%s", specialBaseConverter(num2));
        code[index] = base32[0];
        index++;
        strcpy(binary, cleanBinary);
    }
    sprintf(fullLine,"%s", specialBaseConverter(printCounter));
    strcat(fullLine,code);
    fputs( fullLine,file);

}



/* builds the action table, with the needed opcodes */
void tableBuild(action *opcode) {

    opcode[0].name = "mov";
    opcode[0].opcode = 0;

    opcode[1].name = "cmp";
    opcode[1].opcode = 1;

    opcode[2].name = "add";
    opcode[2].opcode = 2;

    opcode[3].name = "sub";
    opcode[3].opcode = 3;

    opcode[4].name = "not";
    opcode[4].opcode = 4;

    opcode[5].name = "clr";
    opcode[5].opcode = 5;

    opcode[6].name = "lea";
    opcode[6].opcode = 6;

    opcode[7].name = "inc";
    opcode[7].opcode = 7;

    opcode[8].name = "dec";
    opcode[8].opcode = 8;

    opcode[9].name = "jmp";
    opcode[9].opcode = 9;

    opcode[10].name = "bne";
    opcode[10].opcode = 10;


    opcode[11].name = "get";
    opcode[11].opcode = 11;

    opcode[12].name = "prn";
    opcode[12].opcode = 12;

    opcode[13].name = "jsr";
    opcode[13].opcode = 13;

    opcode[14].name = "rts";
    opcode[14].opcode = 14;

    opcode[15].name = "hlt";
    opcode[15].opcode = 15;

}

/* Builds the Project's 32-bit language" */
void langBuild(langReserved *base){
    base[0].letters="!";
    base[1].letters="@";
    base[2].letters="#";
    base[3].letters="$";
    base[4].letters="%";
    base[5].letters="^";
    base[6].letters="&";
    base[7].letters="*";
    base[8].letters="<";
    base[9].letters=">";
    base[10].letters= "a";
    base[11].letters= "b";
    base[12].letters= "c";
    base[13].letters= "d";
    base[14].letters= "e";
    base[15].letters= "f";
    base[16].letters= "g";
    base[17].letters= "h";
    base[18].letters= "i";
    base[19].letters= "j";
    base[20].letters= "k";
    base[21].letters= "l";
    base[22].letters= "m";
    base[23].letters= "n";
    base[24].letters= "o";
    base[25].letters= "p";
    base[26].letters= "q";
    base[27].letters= "r";
    base[28].letters= "s";
    base[29].letters= "t";
    base[30].letters= "u";
    base[31].letters= "v";
}

/*builds the memory table, with the system reserved values*/
void memoryBuild(sysReserved *memory) {
    memory[0].words="r0";
    memory[1].words="r1";
    memory[2].words="r2";
    memory[3].words="r3";
    memory[4].words="r4";
    memory[5].words="r5";
    memory[6].words="r6";
    memory[7].words="r7";
    memory[8].words="mov";
    memory[9].words="cmp";
    memory[10].words="add";
    memory[11].words="sub";
    memory[12].words="not";
    memory[13].words="clr";
    memory[14].words="lea";
    memory[15].words="inc";
    memory[16].words="dec";
    memory[17].words="jmp";
    memory[18].words="bne";
    memory[19].words="get";
    memory[20].words="prn";
    memory[21].words="jsr";
    memory[22].words="rts";
    memory[23].words="hlt";
    memory[24].words=".data";
    memory[25].words=".string";
    memory[26].words=".struct";
    memory[27].words=".entry";
    memory[28].words=".extern";
    memory[29].words="endm";
    memory[30].words="macro";
}
