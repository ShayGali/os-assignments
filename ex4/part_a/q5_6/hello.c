/**
 * File: hello.c
 * make valgrind [argument] > valgrind_output2.txt 2>&1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char *string, *string_so_far;
    int i, length;
    length = 0;
    for (int i = 0; i < argc; i++)
    {
        length += strlen(argv[i]) + 1; // gathering the whole strings length
        string = malloc(length + 1);   // will be a problem, to many mallocs, and not reallocs, and there no free in the end

        /** copy the string built so far */

        if (string_so_far != (char *)0)
        { // while it is not nullpoiter '\0'
            strcpy(string, string_so_far);
        }
        else
        {
            *string = '\0'; // means this is the end of the string, and marking it with the nullpointer
        }
        strcat(string, argv[i]);
        if (i < argc - 1) // means this is not the last command
        {
            strcat(string, " ");
        }
        string_so_far = string;
    }
    printf("You enterd: %s\n", string_so_far);
    return 0;
}