#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/***/
void run_program(char *args_as_string) {
    // tokenize the string - split by space
    char *token = strtok(args_as_string, " ");

    if (token == NULL) {
        fprintf(stderr, "No arguments provided\n");
        exit(1);
    }
    // create an array of strings to store the arguments
    char **args = (char **)malloc(sizeof(char *));
    int n = 0;          // number of arguments
    args[n++] = token;  // add the first argument

    // get the rest of the arguments
    while (token != NULL) {
        token = strtok(NULL, " ");                                    // get the next token (NULL - take the next token from the previous string)
        char **t = (char **)realloc(args, (n + 1) * sizeof(char *));  // allocate memory for the new argument
        // check if the memory allocation failed
        if (t == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(args);
            exit(1);
        }
        args = t;
        args[n++] = token;  // add the new argument and increment the number of arguments
    }

    // fork and execute the program
    int fd = fork();
    if (fd < 0) {  // fork failed
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    if (fd == 0) {  // child process
        execvp(args[0], args);
        fprintf(stderr, "Exec failed\n");
        free(args);
        exit(1);
    } else {
        wait(NULL);  // wait for the child process to finish
        // free the memory
        free(args);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 || argv[1][0] != '-' || argv[1][1] != 'e') {
        fprintf(stderr, "Usage: %s -e <value>\n", argv[0]);
        exit(1);
    }

    int opt;
    while ((opt = getopt(argc, argv, "e:")) != -1) {
        switch (opt) {
            case 'e':
                run_program(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -e <value>\n", argv[0]);
                exit(1);
        }
    }

    return 0;
}