/* Searches and build macro declarations and prints it to a new .am file.
All the macros are stored in a linked list and each node holds a macro,
which holds as an individual linked list. While the data in the each individual list is the
the lines of the macro, up to 80 characters per line */

#include "pre_proccesor.h"

void pre_proccesor(FILE *file, char fileName[])
{
    int macroFlag = FALSE;
    FILE *sourceFile;
    char *token;
    char isMacro[LINE_LENGTH];

    String macroStr = "macro ";
    String endmStr = "endmacro";
    String line;

    Table *macroTable;
    char delemiters[] = " \n\t\r";

    sourceFile = fopen(file, "r");
    if (sourceFile == NULL)
    {
        printf("Source File openning failed");
        return;
    }

    FILE *newFile = fopen(fileName, "w");
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
                macroFlag = TRUE;
                LinkedList *curr;
                curr = (LinkedList *)malloc(sizeof(LinkedList));
                if (curr == NULL)
                {
                    printf("Memory allocation failed");
                    return;
                }

                memcpy(curr->macroName, token, LINE_LENGTH); /* Names the macro */

                if (macroTable->head == NULL)
                { /* If this is the first line */
                    macroTable->head = curr;
                    macroTable->tail = curr;
                }
                else
                { /* Manuelly insert each line at the end of the LinkedList*/
                    macroTable->tail->next = curr;
                    macroTable->tail = curr;
                }
                token = strchr(line, delemiters);
            }
        }
        else
        { /* Filling the lines of the macro, each line is a node */
            if (strcmp(isMacro, "endmacro") == 0)
            { /* End of macro*/
                macroFlag = FALSE;
                continue;
            }
            fputs(line, newFile); /* Prints the line into the new file */

            Node *curr;
            if (curr == NULL)
            {
                printf("Memory allocation failed");
                return;
            }

            LinkedList *list;
            if (list == NULL)
            {
                printf("Memory allocation failed");
                return;
            }

            curr = (Node *)malloc(sizeof(Node));
            list = (LinkedList *)malloc(sizeof(LinkedList));

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
        memset(isMacro, '\0', sizeof(char) * LINE_LENGTH); /* resets the array for the next itiration */
    }

    free_list_of_lists(macroTable);
}
