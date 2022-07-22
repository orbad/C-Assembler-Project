/* Searches and build macro declarations and prints it to a new .am file.
All the macros are stored in a linked list and each node holds a macro,
which holds as an individual linked list. While the data in the each individual list is the
the lines of the macro, up to 80 characters per line */

#include "pre_proccesor.h"

void pre_proccesor(FILE *file, char fileName[])
{
    int macroFlag = FALSE;
    FILE *sourceFile;
    char ch;
    char *token;
    String line;
    char isMacro[LINE_LENGTH];


    LinkedList *macroTable;
    char delemiters[] = " \n\t\r";

    FILE *newFile = fopen(fileName, "w");
    if (newFile == NULL)
    {
        printf("Creating output file failed");
        return;
    }

    sourceFile = fopen(file, "r");
    if (sourceFile == NULL)
    {
        printf("Source File openning failed");
        return;
    }
    else
    { /* If the source file hasn't opened or is empty, we don't initialze the Macro Table */
        if ((ch = fgetc(sourceFile)) == EOF)
        {
            printf("Empty source file");
            return;
        }

        macroTable = (LinkedList *)malloc(sizeof(LinkedList));
        if (macroTable == NULL)
        {
            printf("Memory allocation for Macro Table failed");
            return;
        }
    }

    while (fgets(line, LINE_LENGTH, sourceFile) != EOF){ /* First loop- reading from the file to initialize Macro Table*/
        if (macroFlag == FALSE)
        {
            strncpy(isMacro, line, 6);
            if ((strcmp(isMacro, "macro ")) == 0)   /* Checks if the first word is "macro " */
            { 
                if (search_list != 0){
                    return;
                }
                macroFlag = TRUE;
                macroTable->macroName = strchr(line, delemiters);  /* Names the macro */
            }
        }
        else{ /* Filling the lines of the macro, each line is a node */
            if (strcmp(isMacro, "endmacro") == 0){ /* End of macro*/
                macroFlag = FALSE;
                continue;
            }
            Node *curr;
            LinkedList *list;

            curr = (Node *)malloc(sizeof(Node));
            list = (LinkedList *)malloc(sizeof(LinkedList));

            strcpy(line, curr->line);

            if (list->head == NULL){ /* If this is the first line */
                list->head = curr;
                list->tail = curr;
            }
            else{ 
                list->tail->next = curr;
                list->tail = curr;
            }
        }
    }

    memset(isMacro,'\0',sizeof(char)*LINE_LENGTH);
    rewind(sourceFile); /* Resetting the pointer of the file */

    while (fgets(line, LINE_LENGTH, sourceFile) != EOF){
        strncpy(isMacro, line, 6);
        char *ptr = NULL;
        if ((strcmp(isMacro, "macro ")) == 0){
            memset(isMacro,'\0',sizeof(char)*LINE_LENGTH);
            ptr = strchr(line, delemiters);
            print_macro(macroTable, ptr);

        }
            fputs(line,newFile);
    }
}
