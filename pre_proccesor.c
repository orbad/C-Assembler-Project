/* Searches and build macro declarations and prints it to a new .am file.
All the macros are stored in a linked list and each node holds a macro,
which holds as an individual linked list. While the data in the each individual list is the
the lines of the macro, up to 80 characters per line.
We create an .am file */

#include "pre_proccesor.h"

void pre_proccesor(FILE *file, char fileName[])
{
    int macroFlag = FALSE;
    char *token;
    char isMacro[LINE_LENGTH];
    FILE *newFile;
    String line;

    Table *macroTable;
    char delemiters[] = " \n\t\r";

    newFile = fopen(fileName, "w");
    if (newFile == NULL)
    {
        printf("Creating output file failed");
        return;
    }

    newFile = fopen(strcat(fileName, ".am"), "w");
    if (newFile == NULL)
    {
        printf("Creating output file failed");
        return;
    }

    /* If the source file hasn't opened or is empty, we don't initialze the Macro Table */
    if ((fgets(line, LINE_LENGTH, file) != NULL))
    {
        printf("Empty source file");
        return;
    }
    rewind(file); /* Resetting the cursor */

    macroTable = (Table *)malloc(sizeof(Table));
    if (macroTable == NULL)
    {
        printf("Memory allocation for Macro Table failed");
        return;
    }

while (fgets(line, LINE_LENGTH, file) != NULL)
    {
                memset(isMacro, '\0', sizeof(char) * LINE_LENGTH); /* resets the comparison array for the next itiration */
        if (macroFlag == FALSE)
        {
            strncpy(isMacro, line, 6);
            if ((strcmp(isMacro, "macro ")) == 0) /* Checks if the first word is "macro " */
            {
                token = strchr(line, *delemiters);
                trimString(token);
                removeTails(token);
                if (search_list(macroTable, token) == 0) /* Valid macro insert */
                {
                    macroFlag = TRUE;
                    LinkedList *currList;
                    currList = (LinkedList *)malloc(sizeof(LinkedList));
                    if (currList == NULL)
                    {
                        printf("Memory allocation failed");
                        return;
                    }
                    memcpy(currList->macroName, token, LINE_LENGTH); /* Names the macro */

                    if (macroTable->head == NULL)
                    { /* If this is the first line */
                        macroTable->head = currList;
                        macroTable->tail = currList;
                    }
                    else
                    { /* Manuelly insert each line at the end of the LinkedList */
                        macroTable->tail->next = currList;
                        macroTable->tail = currList;
                    }
                    continue; /* Not pronting the macro defenition to*/
                }
            }
            else
            {
                if (strcmp(line,"\n") == 0){
                     fputs(line, newFile); /* print the empty line */
                     continue;
                }
                else if (find_macro(macroTable, line) != NULL)
                {
                    print_macro(find_macro(macroTable, line), newFile);
                    continue;
                }
            }
        }
        else
        { /* Filling the lines of the macro, each line is a node */
            strncpy(isMacro, line, 8);
            if (strcmp(isMacro, "endmacro") == 0)
            { /* End of macro*/
                macroFlag = FALSE;
                memset(isMacro, '\0', sizeof(char) * LINE_LENGTH);
                continue;
            }

            /* Sets the lines of the macro*/
            Node *currNode;
            currNode = (Node *)malloc(sizeof(Node));
            if (currNode == NULL)
            {
                printf("Memory allocation failed");
                return;
            }

            memcpy(currNode->line, line, LINE_LENGTH);

            if (macroTable->tail->tail == NULL) /* The last linked list is the one we're working on */
            {                                   /* If this is the first line */
                macroTable->tail->head = currNode;
                macroTable->tail->tail = currNode;
            }
            else
            { /* Manuelly insert each line at the end of the LinkedList*/
                macroTable->tail->tail->next = currNode;
                macroTable->tail->tail = currNode;
            }
        }
        fputs(line, newFile);                              /* Prints the line into the new file */
        memset(isMacro, '\0', sizeof(char) * LINE_LENGTH); /* resets the comparison array for the next itiration */
        memset(line, '\0', sizeof(char) * LINE_LENGTH);    /* resets the line */
    }
    free_list_of_lists(macroTable);
    fclose(file);
    fclose(newFile);
    }
