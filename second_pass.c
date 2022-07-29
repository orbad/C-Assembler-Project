/**
 * second_pass function builds the output files, translate the words to bits, and uses the symbol table.
 * every line is being splitted to words, and each kind of operand gets is exact translation
 * the basic idea is sending each process of operand, translation or special case into a different function.
 * at the end, the entries and externals files are being made.
 * author: Or Badani 316307586, Eli Gandin 209327915
 */


#include "second_pass.h"

void second_pass (char *fileName, symbol * symTable,object * objects) {

    int i,action,funct,entryUpdate,lineCounter=DEF_VAL,DC=DEF_VAL,lastExtSize=DEF_VAL,extSize=DEF_VAL,symTableSize=objects->memVariables[symTable_size].num,printCounter=OPS_FIRST_VALUE;
    int ICF= (objects->memVariables[IC_size].num) - OPS_FIRST_VALUE; /*ICF is really -100*/
    int errorFlag = FALSE,programFailure=FALSE;
    string line,copyLine,firstPrintedLine;
    char *token,*tokenB;
    char delimit[] = " \n\t\r ,";
    char cleanLine[BINARY_LENGTH] = "0000000000\0",binaryLine[BINARY_LENGTH];
    int *values; /* This int pointer array will get the int array from middleWordFunctions */
    FILE *sFile,*newCode;
    dataList *dataLine=(dataList *) malloc((objects->memVariables[DC_size].num + 1) * sizeof(dataList)); /*will hold the data lines*/
    extAddress *e,*extAdd;
    extAdd= (extAddress *) malloc( sizeof(extAddress));/*will hold the extern addresses*/

    sFile = fopen(strcat(fileName,".am") , "r");
    fileName[strlen(fileName)-3]='\0';
    newCode = fopen(strcat(fileName, ".ob"), "w");
    fileName[strlen(fileName)-3]='\0';

    strcpy(binaryLine, cleanLine);
    /*adding the first new code line, with IC and DC values*/
    sprintf(firstPrintedLine, "\t%d %d\n", ICF, objects->memVariables[DC_size].num);
    fputs(firstPrintedLine, newCode);


    while (fgets(line, MAX_CHAR, sFile) != NULL) {
        lineCounter++; /*counting the original lines for error print*/

        strcpy(copyLine, line);
        token = strtok(copyLine, "\n"); /*in the beginning token gets the full line, and checks if it is empty*/

        /*skipping empty\whitespaces\comment lines*/
        if (!token)
            continue;

        while (isspace((unsigned char) *token)) token++;
        if (strcmp("", token) == 0||line[0]==';')
            continue;

        token = strtok(copyLine, delimit); /*token gets the first word */

        if (strchr(token, ':')) /*skipping labels*/
            token = (strtok(NULL, delimit));

        while (isspace((unsigned char) *token)) token++;


        if (strstr(token, ".entry")) { /*instruction line*/
            token = (strtok(NULL, delimit)); /*the first word of the sentence*/

            removeTails(token);
            entryUpdate=isSymbol(token,symTable,symTableSize);
            if (entryUpdate==NEUTRAL) {
                printf("error in line %d: %s does not appear in symbol table \n",printCounter,token);
                errorFlag = TRUE;
                continue;
            }
            else {
                symTable[entryUpdate].source = R;
                continue;
            }
        }

        if (strstr(token, ".extern")) { /*avoiding extern line*/
            continue;
        }

        if ((strstr(token, ".data"))) {
            token = (strtok(NULL, "\n")); /*getting the full sentence content, so the delimiter is \n*/
            DC = dataTranslation(token, dataLine, DC);
            continue;
        }

        /*translating data values*/
        if (strstr(token, ".string")) {
            token = (strtok(NULL, delimit));
            DC = stringTranslation(token, dataLine, DC);
            continue;
        }



        binaryLine[A]++; /*first line will always be Absolute */

        action = findAction(token, objects->actionTable);

        /*creating the first binary line*/
        printCounter=firstWordProcess(objects->actionTable[action].opcode, binaryLine, newCode, printCounter);


        /*all actions beside rts and stop require more than one word*/
        if (action<14) {

            token = (strtok(NULL, delimit));
            if (token)
                tokenB = (strtok(NULL, delimit));


            funct=objects->actionTable[action].funct; /*getting the specific action's funct*/

            /*will make the second binary codee, and afterwords the rest*/
            values=middleWordProcess(token, tokenB, funct, symTable, symTableSize, newCode,extAdd,extSize, printCounter,errorFlag);

            errorFlag=values[errorFlag_ind];
            printCounter=values[printCounter_ind];
            extSize=values[extAdd_ind];
        }
        if (errorFlag)
            programFailure=TRUE;

        if (lastExtSize<extSize){
            lastExtSize++;
            e = (extAddress *) realloc(extAdd, sizeof(extAddress) * (extSize+1));
            if (!e) {
                printf("memory allocation for extern addresses table failed\n");
                exit(0);
            }
            extAdd = e;
        }

        printCounter++; /*for next line to print*/
        /*cleaning the line towards the next line*/
        strcpy(binaryLine, cleanLine);

    }

    for(i=0;i<(DC); i++) { /*printing data lines from datalist to new code*/
        specialBasePrint(dataLine[i].dLine, newCode, printCounter);
        printCounter++;
    }

    if (!programFailure) {   /*making the entries and externals files, only if there was no error found - and there are any*/
        printf("program completed successfully.\n");
        if (extSize>DEF_VAL)
            makeExtFile(fileName, extAdd, extSize);
        if (objects->memVariables[symTable_size].num>DEF_VAL)
            makeEntFile(fileName, symTable, objects->memVariables);
    }
    else
        printf("program terminated by an error.");

    free(values);
    free(dataLine);
    free(extAdd);
    fclose(newCode);
    fclose(sFile);

}

/*turning on the correct action bit on the first line*/
int firstWordProcess(int action, char line[], FILE *newCode, int printCounter) {
    line[action]++;
    specialBasePrint(line, newCode,printCounter);
    return printCounter++;
}

/*processing the second and beyond words of each line.
 * this is the main function of the program, and splits to smaller functions for each operand
 * the beginning of the function is responsible for making  and sending to print the second binary line,
 * considering funct, opcode, addressing and so on.
 * the last part is responsible for creating the extra lines if needed (immediate or labels)
 * */
int* middleWordProcess(char *firstOperand, char *secondOperand, int funct, symbol *symTable, int symTableSize, FILE *newCode,extAddress *extAdd,int extSize, int printCounter,int errorFlag) {

    int doubleOpFlag=FALSE;
    int extraWord=NEUTRAL,extraWord2=NEUTRAL;
    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
    int *values=(int *) malloc(value_Array_Size * sizeof(int *));; /*0 - errorFlag, 1 - printCounter, 3 - extSize*/

    strcpy(line, cleanLine);

    /*the second word will always be Absolute.*/
    line[A]++;




    /*funct translation*/
    bitTranslation(funct_len, funct_op, funct, line);

    /*to know in advance if we need a source operand*/
    if(secondOperand)
        doubleOpFlag = TRUE;

    /*first and second operand translations*/
    extraWord=operandTranslation(firstOperand , line, doubleOpFlag, symTable,symTableSize,extAdd,printCounter,&extSize,&errorFlag);

    /*only execute if we have 2 operands */
    if(doubleOpFlag) {
        doubleOpFlag = FALSE;
        extraWord2 = operandTranslation(secondOperand, line, doubleOpFlag, symTable,symTableSize,extAdd,printCounter,&extSize,&errorFlag);
    }

    printCounter++;
    specialBasePrint(line, newCode, printCounter);

    /*only for extra words*/

    if (extraWord>=DEF_VAL) /*positive means we found a symbol*/
        printCounter=labelWordsProcess(symTable[extraWord].base, symTable[extraWord].offset, symTable[extraWord].source, newCode, printCounter);

    if (extraWord==-2)  /*means we found # in operand - so its direct*/
        printCounter=immediateWordProcess(firstOperand, newCode,printCounter);
    /*second operand extra words*/
    if (extraWord2>=DEF_VAL)
        printCounter=labelWordsProcess(symTable[extraWord2].base, symTable[extraWord2].offset, symTable[extraWord2].source, newCode, printCounter);

    if (extraWord2==-2)
        printCounter=immediateWordProcess(secondOperand, newCode,printCounter);

    values[errorFlag_ind]=errorFlag;
    values[printCounter_ind]=printCounter;
    values[extAdd_ind]=extSize;
    return values;

}


/*translating a single operand to the correct addressing, and sending to print*/
int  operandTranslation(char * operand, char line[], int isSource, symbol *symTable, int symTableSize,extAddress *extAdd,int printCounter,int *extSize,int *errorFlag) {
    int regNum = DEF_VAL, addNum = DEF_VAL, extraWordFlag = FALSE, indexAdd = FALSE;

    /*if the operand is index ,it contains []. this check is first because we split the token. */
    if (strchr(operand, '[') || strchr(operand, ']')) { /*a line with any brackets must contain [] and a register in it*/
            regNum = extractNumber(operand); /*extracting the register number*/
            operand = strtok(operand, strchr(operand, '['));
            addNum = index;
            indexAdd = TRUE;
        }

    /*a positive result means we have a symbol*/
    extraWordFlag= isSymbol(operand, symTable,symTableSize);

    /*if the operand is direct - it will have a positive number, and will not be an index.*/
    if (extraWordFlag >= 0&&!indexAdd) {
        addNum = direct;

        /*storing ext symbols addresses */
        if (symTable[extraWordFlag].source == E) {
            string label;
            strcpy(label, operand);
            /*the externals base and offset will always be 2 lines after */
            updateExtAdd(label, printCounter + 2, extAdd,*extSize);
            *extSize=*extSize+1;
        }
    }

        /*if the operand is immediate - it will start with #*/
    else if (operand[DEF_VAL]=='#') {
        addNum = immediate;
        extraWordFlag=-2; /*NEUTRAL means there's no symbol, -2 will be immediate flag*/
    }

        /*if the operand is a register*/
    else if (operand[DEF_VAL]=='r') {
        regNum = extractNumber(operand);
        addNum = reg_direct;
    }

    /*this flag will be true if this is a source operand - only for 2 operands line*/
    if (isSource) {
        bitTranslation(add_len, source_add_op, addNum, line);
        bitTranslation(reg_len, source_reg_op, regNum, line);
    }
    else {
        /*all other cases, this is either the first and only operand or the second operand - needs to be a destination operand */
        bitTranslation(add_len, dest_add_op, addNum, line);
        bitTranslation(reg_len, dest_reg_op, regNum, line);
    }

    return extraWordFlag;

}

/*dealing with a label word , adding the collected base and offset from symbol table
 * sending to print a single line every time*/
int labelWordsProcess(int base, int offset, int source, FILE *newCode, int printCounter) {
    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
    strcpy(line,cleanLine);

    /*source was stored as E only if it was an extern. any other way its R*/
    if (source<=0)
        line[R]++; /*source contains the correct bit number for R or E*/
    else
        line[source]++;


    /*Translating the base to a new binary line*/
    bitTranslation(binary_line_len,binary_line_ind,base,line);
    printCounter++;
    specialBasePrint(line, newCode, printCounter);


    strcpy(line,cleanLine);

    if (source==NEUTRAL)
        line[R]++; /*source contains the correct bit number for R or E*/
    else
        line[source]++; /*source contains the correct bit number for E*/

    /*Translating the offset to a new binary line*/
    bitTranslation(binary_line_len,binary_line_ind,offset,line);
    printCounter++;
    specialBasePrint(line, newCode, printCounter);

    return printCounter;
}

/*dealing with immediate words, translating numbers to binary.
 * negative number is being dealt with in extract number - the "mashlim le2"
 * sending to print a single line every time*/
int immediateWordProcess(char *operand, FILE *newCode, int printCounter) {
    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
    int num = extractNumber(operand);
    strcpy(line, cleanLine);
    line[A]++; /*immediate word is Absolute*/

    operand++;

    bitTranslation(binary_line_len,binary_line_ind, num, line);

    printCounter++;
    specialBasePrint(line, newCode, printCounter);

    return printCounter;
}

/*main translation function (does not print).
 * this functions gets the exact bits length, the location on the line and the value,
 * it translates by dividing the number by 2 every time, and store the reminder.*/
void bitTranslation(int bitLength, int index, int value, char line[]) {
    int negative = FALSE;
    int reminder = value % 2;
    index = index-bitLength+1; /* We want the reminder enter the result in the opposite way */

    if (value < 0) { /* Negative number requires a calculation with the 2's complement method */
        negative = TRUE;
        value = (value * NEUTRAL)-1;
    }
    reminder = value % 2;
    for (; bitLength > DEF_VAL; bitLength--) {

        if (!negative)
            line[index] = reminder + '0';
        else /*1 become 0 and the opposite */
            line[index] = (1 - reminder) + '0';
        value = value / 2;
        reminder = (value % 2);
        index++;
    }

}
/*translating a string to bits, by translating every character to ascii value, and 0 in the end
 * retuning the new DC value*/
int stringTranslation(char * token, dataList *dataLine,int DC) {
    int int2ascii=NEUTRAL;
    char cleanLine[BINARY_LENGTH] = "0000000000\0", line[BINARY_LENGTH];
    strcpy(line, cleanLine);
    if (token[DEF_VAL] == '\"') {
        token++;
        while (*token != '\"') {
            int2ascii = *token;
            line[A]++; /*data line will always be Absolute.*/
            bitTranslation(binary_line_len, binary_line_ind, int2ascii, line);
            strcpy(dataLine[DC].dLine, line);
          /*  printf("string! %s\n",line);*/

            strcpy(line, cleanLine);
            DC++;
            token++;
        }
        /*inserting zero's line to represent \0*/
        cleanLine[A]++;
        strcpy(dataLine[DC].dLine, cleanLine);
        DC++; /*updating DC next empty loctaion*/

    }
    return DC;
}

/*translating a data line to bits, by translating every number to binary.
 * retuning the new DC value*/

int dataTranslation(char * token, dataList *dataLine,int DC) {
    int num;

    char cleanLine[BINARY_LENGTH] = "0000000000\0", line[BINARY_LENGTH];
    char delimit[] = " \n\t\r ,";
    strcpy(line, cleanLine);

    strtok(token, delimit);
    while (token) {
        line[A]++; /*data line will always be Absolute.*/
        num = extractNumber(token);
        bitTranslation(binary_line_len, binary_line_ind, num, line);
        strcpy(dataLine[DC].dLine, line);

        strcpy(line, cleanLine);
        DC++;
        token = strtok(NULL, delimit);
    }
    return DC;
}

/*making the ent file, printing the special requirements*/
void makeEntFile(string fileName,symbol *symTable,dynamicMem *memVariables) {
    int i;
    char line[2*MAX_CHAR]; /*line will be getting 2 integers and chars added to it - so make it bigger */
    FILE *entries = fopen(strcat(fileName, ".ent"), "w");
    fileName[strlen(fileName)-4]='\0'; /*removing the .ent*/

    /*making the ent file*/
    for (i = 0; i <= memVariables[symTable_size].num; i++) {
        if (symTable[i].source == R) {
            sprintf(line, "%s,%04d,%04d\n",symTable[i].name, symTable[i].base , symTable[i].offset);
            fputs(line, entries);
        }
    }
    fclose(entries);
}


/*adding another ext address to the externals address table */
void updateExtAdd(string label, int address,extAddress *extAdd,int extSize) {
    strcpy(extAdd[extSize].name,label);
    extAdd[extSize].base=address;
    extAdd[extSize].offset=address+1;
}
/*making the externals file using the extAdd object we collected*/
void makeExtFile(string fileName,extAddress *extAdd,int extSize) {
    int i;
    char line[MAX_CHAR * 2],tempStr[MAX_CHAR * 2]; /*line will be getting 2 string 80 chars size. */
    FILE *externals = fopen(strcat(fileName, ".ext"), "w");

    fileName[strlen(fileName)-4]='\0'; /*removing the .ent*/

    /*updating the ext file*/
    for(i=0;i<extSize;i++) {
        sprintf(line, "%s BASE %d\n", extAdd[i].name, extAdd[i].base);
        fputs(line, externals);
        strcpy(line, tempStr);

        sprintf(line, "%s OFFSET %d\n\n", extAdd[i].name, extAdd[i].offset);
        fputs(line, externals);
        strcpy(line, tempStr);


    }

    fclose(externals);
}
