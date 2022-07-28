#include "header.h"



/*removing the whitespaces at the end of each sentence*/
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

/*searching the action table list for actions */
int findAction(char* token, action table[] ) {
    int i;
    for (i = DEF_VAL; i < instructions_size; i++)
        if (!strcmp(token, table[i].name))   /* need to compare to macro names*/
            return i;
    return NEUTRAL;
}

/*searching the system Reserved table list  */
int memCheck(char* token, sysReserved memory[] ) {
    int i;
    for (i = DEF_VAL; i < sys_res_size; i++)
        if (!strcmp(token, memory[i].words))
            return i;
    return NEUTRAL;

}

/*getting a number out of a string*/
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

/*checking if the token is a familiar symbol from the symbol table*/
int isSymbol(string token, symbol * symTable, int tableSize){
    int i;
    for (i = 0; i <= tableSize; i++) {
        if (!strcmp(token, symTable[i].name))
            return i;
    }
    return NEUTRAL;
}

/*the binary translation is being calculated the other way (left to right).
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

/*this is the main print to ob file functions.
 * a single binary line is being reversed and transformed to the special base.
  */
void specialBasePrint(char reversedLine[], FILE *file, int printCounter) {
    int i = DEF_VAL, j = DEF_VAL, index = 1, counter = DEF_VAL;
    int num2;
    string fullLine;
    char *pt; /*for strtol*/
    char cleanBinary[] = "00000", binary[] = "00000",hex[5];
    char code[] = "A0-B0-C0-D0-E0\n", newLine[MAX_BINARY]="0000000000\0";

    strcpy(newLine, reversedLine);
    strReverse(newLine);

    for (j = 0; j < 5; j++) {
        for (i = 0; i < 4; i++) {

            binary[i] = newLine[counter];
            counter++;
        }
        /*we take the convert the number to hexadecimal using sprintf and %x*/
        num2 = strtol(binary, &pt, 2);
        sprintf(hex, "%x", num2);
        code[index] = hex[0];
        index = index + 3;
        strcpy(binary, cleanBinary);
    }
    sprintf(fullLine,"0%d ",printCounter);
    strcat(fullLine,code);
    fputs( fullLine,file);

}



/*builds the action table, with the needed opcodes and functs*/
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
    memory[23].words="stop";
    memory[24].words=".data";
    memory[25].words=".string";
    memory[26].words=".struct";
    memory[27].words=".entry";
    memory[28].words=".extern";
    memory[29].words="endm";
    memory[30].words="macro";
}

