   /**
 * second_pass function builds the output files, translate the words to bits, and uses the symbol table.
 * every line is being split to words, and each kind of operand gets is exact translation
 * the basic idea is sending each process of operand, translation or special case into a different function.
 * at the end, the entries and externals files are being made.
 * author: Or Badani 316307586, Eli Gandin 209327915
 */


#include "second_pass.h"

void second_pass (char *fileName, symbol * symTable,object * objects) {

    int i,action,entryUpdate,lineCounter=DEF_VAL,DC=DEF_VAL,lastExtSize=DEF_VAL,extSize=DEF_VAL,symTableSize=objects->memVariables[symTable_size].num,printCounter=OPS_FIRST_VALUE;
    int ICF= (objects->memVariables[IC_size].num) - OPS_FIRST_VALUE; /*ICF value is -100*/
    int errorFlag = FALSE, programFailure=FALSE;
    String line, copyLine, firstPrintedLine;
    char *token, *tokenB;
    char delimit[] = " \n\t\r ,";
    char cleanLine[BINARY_LENGTH] = "0000000000\0", binaryLine[BINARY_LENGTH];
    int *values; /* This int pointer array will get the int array from middleWordFunctions */
    FILE *sFile,*newCode;
    dataList *dataLine=(dataList *) malloc((objects->memVariables[DC_size].num + 1) * sizeof(dataList)); /* Will hold the data lines */
    operandBuilder *memBuild, *tempBuild;
    extAddress *e,*extAdd;
    memBuild= (operandBuilder *) malloc( sizeof(operandBuilder));/* Will hold all the operand fields */
    extAdd= (extAddress *) malloc( sizeof(extAddress));/* Will hold all the extern addresses */

    sFile = fopen(strcat(fileName,".am") , "r");
    fileName[strlen(fileName)-3]='\0';
    newCode = fopen(strcat(fileName, ".ob"), "w");
    fileName[strlen(fileName)-3]='\0';

    strcpy(binaryLine, cleanLine);

    /* Adding the first new code line, with IC and DC values */
    sprintf(firstPrintedLine, "\t%c %c\n", 'm', 'f';
    fputs(firstPrintedLine, newCode);


    while (fgets(line, LINE_LENGTH, sFile) != NULL) {
        lineCounter++; /* Counting the original lines in order to print errors */

        strcpy(copyLine, line);
        token = strtok(copyLine, "\n"); /* At first token gets the full line, and checks whether the line is empty */

        /* empty\whitespaces\comment handling*/
        if (!token) /* If token is an empty line, then ignore it */
            continue;

        while (isspace((unsigned char) *token)) token++;
        if (strcmp("", token) == 0||line[0]==';')
            continue;

        token = strtok(copyLine, delimit); /* Token gets the first word of the line */

        if (strchr(token, ':')) /* Skipping labels */
            token = (strtok(NULL, delimit));

        while (isspace((unsigned char) *token)) token++;


        if (strstr(token, ".entry")) { /* Instruction line */
            token = (strtok(NULL, delimit)); /* The first word of the sentence */

            removeTails(token);
            entryUpdate=isSymbol(token,symTable,symTableSize);
            if (entryUpdate==NEUTRAL) { /* If entry isn't mentioned in the symbol table */
                printf("error in line %d: %s does not appear in symbol table \n",printCounter,token);
                errorFlag = TRUE;
                continue;
            }
            else {
                symTable[entryUpdate].source = R;
                continue;
            }
        }

        if (strstr(token, ".extern")) { /* Avoiding extern line */
            continue;
        }

        if ((strstr(token, ".data"))) {
            token = (strtok(NULL, "\n")); /* Getting the full sentence content, so the delimiter is \n */
            DC = dataTranslation(token, dataLine, DC);
            continue;
        }

        /* Translating data values*/
        if (strstr(token, ".string")) {
            token = (strtok(NULL, delimit));
            DC = stringTranslation(token, dataLine, DC);
            continue;
        }

        /* Translating struct values*/
        if (strstr(token, ".struct")) {
            token = (strtok(NULL, delimit));
            DC = dataTranslation(token, dataLine, DC); /* Number handling (same as data) */
            token = (strtok(NULL, delimit));
            DC = stringTranslation(token, dataLine, DC); /* String handling */
            continue;
        }

        action = findAction(token, objects->actionTable);

        /* Creating the first binary line */
        printCounter=firstWordProcess(objects->actionTable[action].opcode, printCounter, newCode, memBuild);


        /* All actions beside rts and hlt require more than one word */
        if (action<14) {

            token = (strtok(NULL, delimit));
            if (token)
                tokenB = (strtok(NULL, delimit));


            /* Will make the second binary code, and afterwards the rest */
            values=middleWordProcess(token, tokenB, memBuild, symTable, symTableSize, newCode,extAdd,extSize, printCounter,errorFlag);

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

        printCounter++; /* For next line printing*/
        /* Cleaning the current line towards the next line */
        memBuild->opcode=-1;
        memBuild->ARE=-1;
        memBuild->source_operand=-1;
        memBuild->destination_operand=-1;
        strcpy(binaryLine, cleanLine);

    }

    for(i=0;i<(DC); i++) { /* Printing data lines from datalist to new code */
        // specialBasePrint(dataLine[i].dLine, newCode, printCounter);
        binaryTo32BasePrint(printCounter,newCode,dataLine[i].dLine);
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

/* Assigning the correct opcode according the provided Instruction (mov, cmp,...) */
int firstWordProcess(int action, int printCounter, FILE *newCode, operandBuilder *a) {
    a->opcode=action;
    a->ARE=A;
    int sum;
    if(action>13){
        a->source_operand=A;
        a->destination_operand=A;
        sum=binaryConnection(a);
        intTo32BasePrint(printCounter,newCode,sum);
    }
    return printCounter++;
}

/*processing the second and beyond words of each line.
 * this is the main function of the program, and splits to smaller functions for each operand
 * the beginning of the function is responsible for making  and sending to print the second binary line,
 * considering funct, opcode, addressing and so on.
 * the last part is responsible for creating the extra lines if needed (immediate or labels)
 * */
int* middleWordProcess(char *firstOperand, char *secondOperand, operandBuilder *memBuilder, symbol *symTable, int symTableSize, FILE *newCode,extAddress *extAdd,int extSize, int printCounter,int errorFlag) {

    int doubleOpFlag=FALSE;
    int sum=DEF_VAL;
    int extraWord=NEUTRAL,extraWord2=NEUTRAL;
    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
    int *values=(int *) malloc(value_Array_Size * sizeof(int *)); /*0 - errorFlag, 1 - printCounter, 3 - extSize*/


    strcpy(line, cleanLine);

    /* To know in advance if we need a source operand */
    if(secondOperand)
        doubleOpFlag = TRUE;
    else{
        memBuilder->source_operand=A;
    }

    /* First and second operand translations */
    extraWord=operandTranslation(firstOperand , memBuilder, doubleOpFlag, symTable,symTableSize,extAdd,printCounter,&extSize,&errorFlag);

    /*only execute if we have 2 operands */
    if(doubleOpFlag) {
        doubleOpFlag = FALSE;
        extraWord2 = operandTranslation(secondOperand, memBuilder, doubleOpFlag, symTable,symTableSize,extAdd,printCounter,&extSize,&errorFlag);
    }

    printCounter++;
    sum=binaryConnection(memBuilder);
    intTo32BasePrint(printCounter,newCode,sum);

    /*only for extra words*/

    if (extraWord>=DEF_VAL) /*positive means we found a symbol*/
        printCounter=labelWordsProcess(symTable[extraWord].address, symTable[extraWord].source, newCode, printCounter);

    if (extraWord==-2)  /*means we found # in operand - so its direct*/
        printCounter=immediateWordProcess(firstOperand, newCode,printCounter);
    /*second operand extra words*/
    if (extraWord2>=DEF_VAL)
        printCounter=labelWordsProcess(symTable[extraWord2].address, symTable[extraWord2].source, newCode, printCounter);

    if (extraWord2==-2)
        printCounter=immediateWordProcess(secondOperand, newCode,printCounter);

    values[errorFlag_ind]=errorFlag;
    values[printCounter_ind]=printCounter;
    values[extAdd_ind]=extSize;
    return values;

}


/* Translating an operand to the correct addressing, and sending to print*/
int  operandTranslation(char * operand, operandBuilder *memBuild, int isSource, symbol *symTable, int symTableSize,extAddress *extAdd,int printCounter,int *extSize,int *errorFlag) {
    int refNum = DEF_VAL, addNum = DEF_VAL, extraWordFlag = FALSE, indexAdd = FALSE;

    /* If the operand is structure ,it contains ".", this check is first because we'll split the token afterwards. */
    if (strchr(operand, '.')) { /* A line with a point (which is not .data/.string/.struct must contain a label and a reference number after it */
            refNum = extractNumber(operand); /* Extracting the reference number */
            operand = strtok(operand, strchr(operand, '.'));
            addNum = structure;
            indexAdd = TRUE;
        }

    /* A positive result means we have found a symbol */
    extraWordFlag= isSymbol(operand, symTable,symTableSize);

    /* If the operand is direct - it will result with a positive number, and will not be a structure. */
    if (extraWordFlag >= 0 && !indexAdd) {
        addNum = direct;

        /* Storing ext symbols addresses */
        if (symTable[extraWordFlag].source == E) {
            String label;
            strcpy(label, operand);
            /* The external's address be a line after */
            updateExtAdd(label, printCounter + 1, extAdd,*extSize);
            *extSize=*extSize+1;
        }
    }

        /* If the operand is immediate - it will start with # */
    else if (operand[DEF_VAL]=='#') {
        addNum = immediate;
        extraWordFlag=-2; /* NEUTRAL means there's no symbol, -2 will be immediate flag */
    }

        /*if the operand is a register*/
    else if (operand[DEF_VAL]=='r') {
        regNum = extractNumber(operand);
        addNum = reg_direct;
    }

    /* This flag will be true if this is a source operand - only for 2 operands line*/
    if (isSource) {
        memBuild->source_operand=addNum;
    }
    else {
        /* All other cases, this is either the first and only operand or the second operand - needs to be a destination operand */
        memBuild->destination_operand=addNum;
    }

    return extraWordFlag;

}

/* Dealing with a label word , adding the collected address from symbol table
 * sending to print a single line every time */
int labelWordsProcess(int address, int source, FILE *newCode, int printCounter) {
//    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
       int sum;
       operandBuilder* labelBuild;
       strcpy(line,cleanLine);

       labelBuild->destination_operand=address;
       labelBuild->source_operand=A;
       labelBuild->opcode=A;
    /* Source was stored as E only if it was an extern. any other way its R */
    if (source<=0)
        labelBuild->ARE=R; /* Source contains the correct bit number for R or E*/
    else
        labelBuild->ARE=source;


    /*Translating the base to a new binary line*/
//    bitTranslation(binary_line_len,binary_line_ind,base,line);
    printCounter++;
    // specialBasePrint(line, newCode, printCounter);
    sum= binaryConnection(labelBuild);
    intTo32BasePrint(printCounter,newCode,sum);


//    strcpy(line,cleanLine);


    /* Looks like offset handling */
//    if (source==NEUTRAL)
//       labelBuild->ARE=R; /*source contains the correct bit number for R or E*/
//    else
//        labelBuild->ARE=source; /* source contains the correct bit number for E */
//
//    /*Translating the offset to a new binary line*/
//    bitTranslation(binary_line_len,binary_line_ind,offset,line);
//    printCounter++;
    // specialBasePrint(line, newCode, printCounter);

    return printCounter;
}

/* Dealing with immediate words, adding numbers to operandBuild struct.
 * negative number is being dealt with 2's complement method.
 * sending to print a single line every time */
int immediateWordProcess(char *operand, FILE *newCode, int printCounter) {
    char cleanLine[BINARY_LENGTH] = "0000000000\0",line[BINARY_LENGTH];
    operandBuilder* immediateBuild;
    int sum=0;
    int num = extractNumber(operand);
    strcpy(line, cleanLine);
    immediateBuild->ARE=A; /* Immediate word is Absolute*/
    immediateBuild->source_operand=A; /* Immediate word has 0 at source operand location*/
    immediateBuild->opcode=A; /* Immediate word has 0 at opcode */
    immediateBuild->destination_operand=num; /* 8-bit number is located in destination_operand location */
    sum=binaryConnection(immediateBuild);
    operand++;
    printCounter++;
    // specialBasePrint(line, newCode, printCounter);
    intTo32BasePrint(printCounter, newCode ,sum);

    return printCounter;
}

/* Main translation function (does not print).
 * this functions gets the exact bit length, the location on the line and the value,
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
        DC++; /* updating DC next empty location */

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
    char line[2*LINE_LENGTH]; /*line will be getting 2 integers and chars added to it - so make it bigger */
    FILE *entries = fopen(strcat(fileName, ".ent"), "w");
    fileName[strlen(fileName)-4]='\0'; /*removing the .ent*/

    /*making the ent file*/
    for (i = 0; i <= memVariables[symTable_size].num; i++) {
        if (symTable[i].source == R) {
            sprintf(line, "%s\t %s\n",symTable[i].name, specialBaseConverter(intsymTable[i].address));
            fputs(line, entries);
        }
    }
    fclose(entries);
}


/*adding another ext address to the externals address table */
void updateExtAdd(String label, int address,extAddress *extAdd,int extSize) {
    strcpy(extAdd[extSize].name,label);
    extAdd[extSize].address=address;
}
/*making the externals file using the extAdd object we collected*/
void makeExtFile(string fileName,extAddress *extAdd,int extSize) {
    int i;
    char line[LINE_LENGTH * 2],tempStr[LINE_LENGTH * 2]; /*line will be getting 2 string 80 chars size. */
    FILE *externals = fopen(strcat(fileName, ".ext"), "w");

    fileName[strlen(fileName)-4]='\0'; /*removing the .ent*/

    /*updating the ext file*/
    for(i=0;i<extSize;i++) {
        sprintf(line, "%s\t %s\n", extAdd[i].name, specialBaseConverter(extAdd[i].address));
        fputs(line, externals);
    }
    fclose(externals);
}
