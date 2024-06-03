/*
find then phone of a person in the phonebook.txt file.

the name of the person is given as a command line argument (argv[1]).
*/

/*
to find a person in the phonebook.txt we will exceute the following pipeline:
grep <name> phonebook.txt | cut -d, -f 2 | sed 's/ //g'

explain the pipeline:
1. grep <name> phonebook.txt
    this will search for the name in the phonebook.txt file and output the line that contains the name.
    if the name is not found, grep will not output anything.

2. cut -d, -f 2
    this will split the line by the comma (-d,) and output the second field (-f 2).
    the second field is the phone number.

3. sed 's/ //g'
    this will remove all spaces from the phone number.

    's/ //g':
        s: substitute - tell sed to replace a pattern with another pattern.
        / //: we will replace the space character with nothing. (the format is /pattern/replacement/)
        g: global - replace all occurrences of the pattern in the line.
*/

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: invalid arguments\n");
        printf("Usage: %s <name>\n", argv[0]);
        return 1;
    }

    char *name = argv[1];

    int pipefd[2];                // pipefd[0] is the read end, pipefd[1] is the write end
    int read_end = STDIN_FILENO;  // track on the read end of the pipe

    int pid;

    // create a pipeline - grep <name> phonebook.txt
    if ((pipe(pipefd)) == -1) {
        perror("pipe");
        return 1;
    }

    // create a child process
    if ((pid = fork()) == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {                      // child
        dup2(pipefd[1], STDOUT_FILENO);  // redirect stdout to write to the pipe (to pass the output to the next command)
        close(pipefd[0]);                // close the read end of the pipe

        // execute the pipeline
        execlp("grep", "grep", name, "phonebook.txt", NULL);
        perror("execlp");
        return 1;
    } else {  // parent
        // wait for the child to finish
        wait(NULL);
        close(pipefd[1]);      // close the write end of the pipe (we don't need it anymore)
        read_end = pipefd[0];  // update the read end of the pipe to the read end of the previous command (so we can read the output of the previous command)
    }

    // create a pipeline - cut -d, -f 2
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {                      // child
        dup2(read_end, STDIN_FILENO);    // redirect stdin to the read end of the pipe (to get the output of the previous command)
        dup2(pipefd[1], STDOUT_FILENO);  // redirect stdout to write to the pipe (to pass the output to the next command)
        close(pipefd[0]);

        // execute the pipeline
        execlp("cut", "cut", "-d,", "-f", "2", NULL);
        perror("execlp");
        return 1;
    } else {  // parent
        // wait for the child to finish
        wait(NULL);
        close(pipefd[1]);      // close the write end of the pipe (we don't need it anymore)
        read_end = pipefd[0];  // update the read end of the pipe to the read end of the previous command (so we can read the output of the previous command)
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {                    // child
        dup2(read_end, STDIN_FILENO);  // redirect stdin to the read end of the pipe (to get the output of the previous command)
        // we don't need to redirect stdout because we will print the output to the terminal
        close(pipefd[0]);

        // execute the pipeline
        execlp("sed", "sed", "s/ //g", NULL); // remove all spaces from the phone number, and print it to the terminal
        perror("execlp");
        return 1;
    } else {  // parent
        // wait for the child to finish
        wait(NULL);
        close(pipefd[1]);
        read_end = pipefd[0];
    }

    return 0;
}