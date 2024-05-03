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
        printf("Error: missing name argument\n");
        printf("Usage: %s <name>\n", argv[0]);
        return 1;
    }


    char *name = argv[1];

    // create the pipeline
    int pipefd[2]; // pipefd[0] is the read end, pipefd[1] is the write end

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // create the first child for the grep command
    int pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) { // if we are in the child
        // child 1
        // copy the write end of the pipe to the stdout file descriptor (so everything written to stdout will go to the pipe)
        dup2(pipefd[1], STDOUT_FILENO); 

        // close the original pipe file descriptors (we don't need them we have them in stdout)
        close(pipefd[0]);
        close(pipefd[1]);

        // execute grep
        execlp("grep", "grep", name, "phonebook.txt", NULL);

        // if we reach this line, it means that execlp failed
        perror("execlp");
        return 1;
    }

    // create the second child for the cut command
    int pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        return 1;
    }

    if (pid2 == 0) {
        // child 2
        // redirect stdin to the pipe
        dup2(pipefd[0], STDIN_FILENO);
        // close the original pipe file descriptors
        close(pipefd[0]);
        close(pipefd[1]);

        // execute cut
        execlp("cut", "cut", "-d,", "-f2", NULL);

        // if we reach this line, it means that execlp failed
        perror("execlp");
        return 1;
    }

    // create the third child for the sed command
    int pid3 = fork();
    if (pid3 == -1) {
        perror("fork");
        return 1;
    }

    if (pid3 == 0) {
        // child 3
        // redirect stdin to the pipe
        dup2(pipefd[0], STDIN_FILENO);
        // close the original pipe file descriptors
        close(pipefd[0]);
        close(pipefd[1]);

        // execute sed
        execlp("sed", "sed", "s/ //g", NULL);

        // if we reach this line, it means that execlp failed
        perror("execlp");
        return 1;
    }


    // close the pipe in the parent
    close(pipefd[0]);
    close(pipefd[1]);

    // wait for the children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    return 0;
}