/*
get first name and optional last name (they both can have a space in between)
then a comma (,) (a name will never have a comma in it)
and then a phone number

and write it to the file "phonebook.txt".

in the end of each record, we will put line feed character (\n)

the command that we will execute is:
for arg in args:
    echo arg >> phonebook.txt
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("Error: missing arguments\n");
        return 1;
    }

    // insert the command arguments into the phonebook.txt
    for (int i = 1; i < argc; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            int fd = open("phonebook.txt", O_WRONLY | O_APPEND, 0666);  // open the file in write only mode and append mode, 0666 is the permission
            if (fd == -1) {
                perror("open");
                return 1;
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            if (i == argc - 1)  // if it is the last argument, don't print the space, and print a newline character
                execlp("echo", "echo", argv[i], NULL);
            else
                execlp("echo", "echo", "-n", argv[i], "", NULL); // -n: don't print the newline character. "": print a space
            perror("execlp");
            return 1;
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}