/* Searches and build macro declarations and prints it to a new .am file.
All the macros are stored in a linked list and each node holds a macro,
which holds as an individual linked list. While the data in the each individual list is the
the lines of the macro, up to 80 characters per line.
We create an .am file */

#include "pre_proccesor.h"

void pre_proccesor(FILE *file, char fileName[])
{
    int macroFlag = FALSE;
    FILE *sourceFile;
    char *token;
    char isMacro[LINE_LENGTH];
    FILE *newFile;
    String line;

    Table *macroTable;
    char delemiters[] = " \n\t\r";

    sourceFile = fopen(file, "r");
    if (sourceFile == NULL)
    {
        printf("Source File openning failed");
        return;
    }

    newFile = fopen(strcat(fileName, ".am"), "w");
    if (newFile == NULL)
    {
        printf("Creating output file failed");
        return;
    }

    /* If the source file hasn't opened or is empty, we don't initialze the Macro Table */
    if ((fgets(line, LINE_LENGTH, sourceFile) != EOF))
    {
        printf("Empty source file");
        return;
    }

    macroTable = (Table *)malloc(sizeof(Table));
    if (macroTable == NULL)
    {
        printf("Memory allocation for Macro Table failed");
        return;
    }

    while (fgets(line, LINE_LENGTH, sourceFile) != EOF)
    {
        if (macroFlag == FALSE)
        {
            strncpy(isMacro, line, 6);
            if ((strcmp(isMacro, "macro ")) == 0) /* Checks if the first word is "macro " */
            {
                token = strchr(line, delemiters);
                if (search_list(macroTable, token) == 0) /* Valid macro insert */
                {
                    macroFlag = TRUE;
                    LinkedList *curr;
                    curr = (LinkedList *)malloc(sizeof(LinkedList));
                    if (curr == NULL)
                    {
                        printf("Memory allocation failed");
                        return;
                    }
                    else{
                        printf("Macro is already defined");
                        return;
                    }

                    memcpy(curr->macroName, token, LINE_LENGTH); /* Names the macro */

                    if (macroTable->head == NULL)
                    { /* If this is the first line */
                        macroTable->head = curr;
                        macroTable->tail = curr;
                    }
                    else
                    { /* Manuelly insert each line at the end of the LinkedList */
                        macroTable->tail->next = curr;
                        macroTable->tail = curr;
                    }
                }
            }
        }
        else
        { /* Filling the lines of the macro, each line is a node */
            if (strcmp(isMacro, "endmacro") == 0)
            { /* End of macro*/
                macroFlag = FALSE;
                continue;
            }
            if (find_macro(macroTable, line) != NULL)
            {
                print_macro(find_macro(macroTable, line), newFile);
            }
            /* Sets the lines of the macro*/
            Node *curr;           
            curr = (Node *)malloc(sizeof(Node));
            if (curr == NULL)
            {
                printf("Memory allocation failed");
                return;
            }

            LinkedList *list; 
            list = (LinkedList *)malloc(sizeof(LinkedList));
            if (list == NULL)
            {
                printf("Memory allocation failed");
                return;
            }

            strcpy(line, curr->line);

            if (list->head == NULL)
            { /* If this is the first line */
                list->head = curr;
                list->tail = curr;
            }
            else
            { /* Manuelly insert each line at the end of the LinkedList*/
                list->tail->next = curr;
                list->tail = curr;
            }
        }
        memset(isMacro, '\0', sizeof(char) * LINE_LENGTH); /* resets the comparison array for the next itiration */
        fputs(line, newFile);                              /* Prints the line into the new file */
    }


    free_list_of_lists(macroTable);
    fclose(sourceFile);
    fclose(newFile);
}
