#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Run the program with the given arguments
 */
void run_program(char *args_as_string) {
    // tokenize the string - split by space
    char *token = strtok(args_as_string, " ");

    if (token == NULL) {
        fprintf(stderr, "No arguments provided\n");
        exit(EXIT_FAILURE);
    }
    // create an array of strings to store the arguments
    char **args = (char **)malloc(sizeof(char *));
    int n = 0;          // number of arguments
    args[n++] = token;  // add the first argument

    // get the rest of the arguments
    while (token != NULL) {
        token = strtok(NULL, " ");                                // get the next token (NULL - take the next token from the previous string)
        args = (char **)realloc(args, (n + 1) * sizeof(char *));  // allocate memory for the new argument
        args[n++] = token;                                        // add the new argument and increment the number of arguments
    }

    // fork and execute the program
    int fd = fork();
    if (fd < 0) {  // fork failed
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }

    if (fd == 0) {  // child process
        execvp(args[0], args);
        fprintf(stderr, "Exec failed\n");
        free(args);
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);  // wait for the child process to finish
        // free the memory
        free(args);
        fflush(stdout);
    }
}

/**
 * Open a TCP server to listen to the given port
 * @param i_value the port to listen to (in format "TCPS<port>")
 * @param input_fd the file descriptor to store the new socket (return value)
 */
void i_handler(char *i_value, int *input_fd) {
    i_value += 4;  // skip the "TCPS" prefix
    int port = atoi(i_value);

    // create TCP socket that will listen to input on localhost:port
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }

    // allow the socket to be reused
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error binding socket");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections - at most 1
    if (listen(sockfd, 1) == -1) {
        perror("error listening on socket");
        exit(EXIT_FAILURE);
    }

    // accept the connection and change the input_fd to the new socket
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    *input_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (*input_fd == -1) {
        perror("error accepting connection");
        exit(EXIT_FAILURE);
    }
}

/**
 * Open a TCP client to the given server
 * @param o_value the server IP and port (in format "TCPC<server_ip>,<server_port>")
 * @param input_fd the current input file descriptor (so that it can be closed if needed)
 * @param output_fd the file descriptor to store the new socket (return value)
 */
void o_handler(char *o_value, int input_fd, int *output_fd) {
    o_value += 4;  // skip the "TCPC" prefix

    // split the string to get the server IP and port
    char *server_ip = strtok(o_value, ",");
    if (server_ip == NULL) {
        fprintf(stderr, "Invalid server IP\n");
        exit(EXIT_FAILURE);
    }

    // get the rest of the string after the comma
    char *server_port = strtok(NULL, ",");
    if (server_port == NULL) {
        fprintf(stderr, "Invalid server port\n");
        exit(EXIT_FAILURE);
    }
    int port = atoi(server_port);

    // open a TCP client to the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {  // TODO: support localhost as address
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // try to connect to the server and change the output_fd to the new socket
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error connecting to server");
        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }
        exit(EXIT_FAILURE);
    }

    *output_fd = sockfd;
}

/**
 * Open a TCP server to listen to the given port. The input and output file descriptors will be the same in the end of the function
 * @param b_value the port to listen to (in format "TCPS<port>")
 * @param input_fd the file descriptor to store the new socket (return value)
 * @param output_fd the file descriptor to store the new socket (return value)
 *
 */
void b_handler(char *b_value, int *input_fd, int *output_fd) {
    // open TCP server to listen to the port
    b_value += 4;  // skip the "TCPS" prefix
    int port = atoi(b_value);

    // create TCP socket that will listen to input on localhost:port
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }

    // allow the socket to be reused
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error binding socket");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections - at most 1
    if (listen(sockfd, 1) == -1) {
        perror("error listening on socket");
        exit(EXIT_FAILURE);
    }

    // accept the connection and change the input_fd & output_fd to the new socket
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (new_fd == -1) {
        perror("error accepting connection");
        exit(EXIT_FAILURE);
    }

    *input_fd = new_fd;
    *output_fd = new_fd;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s -e <value> [-b <value>] [-i <value>] [-o <value>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // parse the arguments
    int opt;
    char *e_value = NULL;
    char *b_value = NULL;
    char *i_value = NULL;
    char *o_value = NULL;

    while ((opt = getopt(argc, argv, "e:b:i:o:")) != -1) {
        switch (opt) {
            case 'e':
                e_value = optarg;
                break;
            case 'b':
                b_value = optarg;
                break;
            case 'i':
                i_value = optarg;
                break;
            case 'o':
                o_value = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -e <value> [-b <value>] [-i <value>] [-o <value>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    //check if we got the -e option (if not, print an error message and exit the program)
    if (e_value == NULL) {
        fprintf(stderr, "Option -e is required\n");
        fprintf(stderr, "Usage: %s -e <value> [-b <value>] [-i <value>] [-o <value>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // check if -b is used with -i or -o (if so, print an error message and exit the program)
    if (b_value != NULL && (i_value != NULL || o_value != NULL)) {
        fprintf(stderr, "Option -b cannot be used with -i or -o\n");
        fprintf(stderr, "Usage: %s -e <value> [-b <value>] [-i <value>] [-o <value>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // start the input and output file descriptors with the default values
    // and will change them if needed
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    if (i_value != NULL) {
        i_handler(i_value, &input_fd);
    }

    if (o_value != NULL) {
        o_handler(o_value, input_fd, &output_fd);
    }

    if (b_value != NULL) {
        b_handler(b_value, &input_fd, &output_fd);
    }

    // redirect the input and output to the new file descriptors
    if (input_fd != STDIN_FILENO) {
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            close(input_fd);
            if (output_fd != STDOUT_FILENO) {
                close(output_fd);
            }
            perror("dup2 input");
            exit(EXIT_FAILURE);
        }
    }

    if (output_fd != STDOUT_FILENO) {
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            close(output_fd);
            if (input_fd != STDIN_FILENO) {
                close(input_fd);
            }
            perror("dup2 output");
            exit(EXIT_FAILURE);
        }
    }

    // run the program with the given arguments
    run_program(e_value);
    // TODO: check how to close the sockets
    return 0;
}