#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

// start the input and output file descriptors with the default values
// and will change them if needed
int input_fd = STDIN_FILENO;
int output_fd = STDOUT_FILENO;

void cleanup_and_exit(int exit_code) {
    if (input_fd != STDIN_FILENO) {
        close(input_fd);
    }
    if (output_fd != STDOUT_FILENO && output_fd != input_fd) {
        close(output_fd);
    }
    exit(exit_code);
}

void close_program(int sig) {
    cleanup_and_exit(EXIT_SUCCESS);
}

/**
 * Open a TCP server to listen to the given port and accept the connection
 * @param port the port to listen to
 * @return the file descriptor of the connected socket
 */
int open_tcp_server_and_accept(int port) {
    // create TCP socket that will listen to input on localhost:port
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // allow the socket to be reused
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error binding socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // listen for incoming connections - at most 1
    if (listen(sockfd, 1) == -1) {
        perror("error listening on socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // accept the connection and change the input_fd to the new socket
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd == -1) {
        perror("error accepting connection");
        cleanup_and_exit(EXIT_FAILURE);
    }
    return client_fd;
}

/**
 * Connect to a TCP server
 * @param server_addr the server IP or hostname
 * @param server_port the server port
 * @param input_fd the file descriptor of the connection socket
 */
int connect_to_tcp_server(char *server_addr, char *server_port) {
    // get address info
    struct addrinfo hints, *res, *p;
    int status;
    int sockfd;

    // set up the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_STREAM;

    // get address info
    if ((status = getaddrinfo(server_addr, server_port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        cleanup_and_exit(EXIT_FAILURE);
    }

    // loop through the results and connect to the first we can
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("error creating socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("error connecting to server");
            continue;
        }

        break;  // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);  // free the linked list

    return sockfd;
}

/**
 * Open a UDP server to listen to the given port
 * Will wait to receive a dummy data to get the client address
 * @param port the port to listen to
 * @return the file descriptor of the connection socket
 */
int udp_server(int port) {
    // create UDP socket that will listen to input on localhost:port
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // allow the socket to be reused
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error binding socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // receive dummy data to get the client address
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (bytes_received == -1) {
        perror("error receiving data");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // call connect to save the client address
    if (connect(sockfd, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
        perror("error connecting to client");
        cleanup_and_exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Connect to a UDP server
 * @param server_ip the server IP or hostname
 * @param server_port the server port
 * @return the file descriptor of the connection socket
 */
int udp_client(char *server_ip, char *server_port) {
    // get address info
    struct addrinfo hints, *res, *p;
    int status;
    int sockfd;

    // set up the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_DGRAM;

    // get address info
    if ((status = getaddrinfo(server_ip, server_port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        cleanup_and_exit(EXIT_FAILURE);
    }

    // loop through the results and connect to the first we can
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("error creating socket");
            continue;
        }
        sendto(sockfd, "Conn msg\n", 9, 0, p->ai_addr, p->ai_addrlen);
        // "connect" to the server - so if we use sendto/recvfrom, we don't need to specify the server address
        connect(sockfd, p->ai_addr, p->ai_addrlen);

        break;  // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);  // free the linked list

    return sockfd;
}

int uds_server_stream(char *socket_path) {
    // create a socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error binding socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // listen for incoming connections - at most 1
    if (listen(sockfd, 1) == -1) {
        perror("error listening on socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // accept the connection and change the input_fd to the new socket
    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd == -1) {
        perror("error accepting connection");
        cleanup_and_exit(EXIT_FAILURE);
    }
    return client_fd;
}

int uds_server_datagram(char *socket_path) {
    // create a socket
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("%s\n", addr.sun_path);
        perror("error binding socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // receive dummy data to get the client address
    char buffer[1024];
    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (bytes_received == -1) {
        perror("error receiving data");
        cleanup_and_exit(EXIT_FAILURE);
    }

    return sockfd;
}

int uds_client_stream(char *socket_path) {
    // create a socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // connect to the server
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error connecting to server");
        cleanup_and_exit(EXIT_FAILURE);
    }

    return sockfd;
}

int uds_client_datagram(char *socket_path) {
    // create a socket
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("error creating socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // connect to the server
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    // connect the socket to the server
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("error connecting socket");
        cleanup_and_exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Parse the hostname and port from the given string
 * @param value the string to parse in the format "<hostname>,<port>"
 * @param hostname the pointer to store the hostname (return value)
 * @param port the pointer to store the port (return value)
 */
void parse_hostname_port(char *value, char **hostname, char **port) {
    // split the string to get the server IP/hostname and port
    *hostname = strtok(value, ",");
    if (*hostname == NULL) {
        fprintf(stderr, "Invalid server IP/hostname\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // get the rest of the string after the comma
    *port = strtok(NULL, ",");
    if (*port == NULL) {
        fprintf(stderr, "Invalid server port\n");
        cleanup_and_exit(EXIT_FAILURE);
    }
}

/**
 * Run the program with the given arguments
 */
void run_program(char *args_as_string) {
    // tokenize the string - split by space
    char *token = strtok(args_as_string, " ");

    if (token == NULL) {
        fprintf(stderr, "No arguments provided\n");
        cleanup_and_exit(EXIT_FAILURE);
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
            cleanup_and_exit(EXIT_FAILURE);
        }
        args = t;
        args[n++] = token;  // add the new argument and increment the number of arguments
    }
    // fork and execute the program
    int fd = fork();
    if (fd < 0) {  // fork failed
        fprintf(stderr, "Fork failed\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    if (fd == 0) {  // child process
        execvp(args[0], args);
        fprintf(stderr, "Exec failed\n");
        free(args);
        cleanup_and_exit(EXIT_FAILURE);
    } else {
        wait(NULL);  // wait for the child process to finish
        // free the memory
        free(args);
        fflush(stdout);
    }
}

/**
 * Update the input and output file descriptors
 * @param value the value to update the file descriptors
 * @param input_need_change 1 if the input file descriptor needs to be changed, 0 otherwise
 * @param output_need_change 1 if the output file descriptor needs to be changed, 0 otherwise
 */
void input_output_updater(char *value, int input_need_change, int output_need_change) {
    int new_fd;
    if (strncmp(value, "TCPS", 4) == 0) {
        // open TCP server to listen to the port
        value += 4;  // skip the "TCPS" prefix
        int port = atoi(value);
        new_fd = open_tcp_server_and_accept(port);

    } else if (strncmp(value, "TCPC", 4) == 0) {
        // open TCP client to connect to the server
        value += 4;  // skip the "TCPC" prefix
        char *server_ip, *server_port;
        parse_hostname_port(value, &server_ip, &server_port);
        new_fd = connect_to_tcp_server(server_ip, server_port);
    } else if (strncmp(value, "UDPS", 4) == 0) {
        // open UDP server to listen to the port
        value += 4;  // skip the "UDPS" prefix
        int port = atoi(value);
        new_fd = udp_server(port);
    } else if (strncmp(value, "UDPC", 4) == 0) {
        // open UDP client to connect to the server
        value += 4;  // skip the "UDPC" prefix
        char *server_ip, *server_port;
        parse_hostname_port(value, &server_ip, &server_port);
        new_fd = udp_client(server_ip, server_port);
    } else if (strncmp(value, "UDSS", 4) == 0) {
        // open Unix Domain Socket server on the given path
        value += 4;  // skip the "UDSS" prefix
        if (*value == 'D') {
            value++;  // skip the type character
            new_fd = uds_server_datagram(value);

        } else if (*value == 'S') {
            value++;  // skip the type character
            new_fd = uds_server_stream(value);
        }
    } else if (strncmp(value, "UDSC", 4) == 0) {
        // open Unix Domain Socket client to connect to the server
        value += 4;  // skip the "UDSC" prefix
        if (*value == 'D') {
            value++;  // skip the type character
            new_fd = uds_client_datagram(value);
        } else if (*value == 'S') {
            value++;  // skip the type character
            new_fd = uds_client_stream(value);
        }
    } else {
        fprintf(stderr, "Invalid input - Expected TCPS<port> or UDPS<port> or UDSS<type(D/S)><socket_path> or TCPC<server_ip>,<server_port> or UDPC<server_ip>,<server_port> or UDSC<type(D/S)><socket_path>\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    if (input_need_change) {
        input_fd = new_fd;
    }
    if (output_need_change) {
        output_fd = new_fd;
    }
}

void chat_handler() {
    // print to the stdout from the input_fd
    // send to output_fd from the stdin
    fd_set read_fds;
    int max_fd = input_fd;

    while (1) {
        FD_ZERO(&read_fds);

        // add the file descriptors to the set
        FD_SET(input_fd, &read_fds);  // we dont need to check if the input_fd == STDIN_FILENO, because it's a set
        FD_SET(STDIN_FILENO, &read_fds);

        // wait for any of the file descriptors to have data to read
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            cleanup_and_exit(EXIT_FAILURE);
        }

        // check if the input_fd has data to read (a socket or a file - not the stdin)
        if (input_fd != STDIN_FILENO && FD_ISSET(input_fd, &read_fds)) {
            char buffer[1024];
            int bytes_read = read(input_fd, buffer, sizeof(buffer));  // read from the input_fd
            if (bytes_read == -1) {
                perror("read");
                cleanup_and_exit(EXIT_FAILURE);
            }
            if (bytes_read == 0) {
                break;
            }
            // write to the stdout
            if (write(STDOUT_FILENO, buffer, bytes_read) == -1) {
                perror("write");
                cleanup_and_exit(EXIT_FAILURE);
            }
        }

        // check if the stdin has data to read (only if we need to write to the output_fd - output_fd != STDOUT_FILENO)
        if (FD_ISSET(STDIN_FILENO, &read_fds) && output_fd != STDOUT_FILENO) {
            char buffer[1024];
            int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));  // read from the stdin
            if (bytes_read == -1) {
                perror("read");
                cleanup_and_exit(EXIT_FAILURE);
            }
            if (bytes_read == 0) {
                break;
            }
            if (write(output_fd, buffer, bytes_read) == -1) {
                perror("write");
                cleanup_and_exit(EXIT_FAILURE);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *usage_msg = "Usage: %s [-e <value>] [-b <value>] [-i <value>] [-o <value>] [-t <value>]\n";
    if (argc < 2) {
        fprintf(stderr, usage_msg, argv[0]);
        cleanup_and_exit(EXIT_FAILURE);
    }

    // parse the arguments
    int opt;
    char *e_value = NULL;
    char *b_value = NULL;
    char *i_value = NULL;
    char *o_value = NULL;
    char *t_value = NULL;

    while ((opt = getopt(argc, argv, "e:b:i:o:t:")) != -1) {
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
            case 't':
                t_value = optarg;
                break;
            default:
                fprintf(stderr, usage_msg, argv[0]);
                cleanup_and_exit(EXIT_FAILURE);
        }
    }

    if (t_value != NULL) {
        signal(SIGALRM, close_program);
        int timeout = atoi(t_value);
        alarm(timeout);
    }

    // check if -b is used with -i or -o (if so, print an error message and exit the program)
    if (b_value != NULL && (i_value != NULL || o_value != NULL)) {
        fprintf(stderr, "Error: Option -b cannot be used with -i or -o\n");
        cleanup_and_exit(EXIT_FAILURE);
    }

    if (i_value != NULL) {
        input_output_updater(i_value, 1, 0);
    }

    if (o_value != NULL) {
        input_output_updater(o_value, 0, 1);
    }

    if (b_value != NULL) {
        input_output_updater(b_value, 1, 1);
    }

    if (e_value != NULL) {
        // redirect the input and output to the new file descriptors
        if (input_fd != STDIN_FILENO) {
            if (dup2(input_fd, STDIN_FILENO) == -1) {
                perror("dup2 input");
                cleanup_and_exit(EXIT_FAILURE);
            }
        }

        if (output_fd != STDOUT_FILENO) {
            if (dup2(output_fd, STDOUT_FILENO) == -1) {
                perror("dup2 output");
                cleanup_and_exit(EXIT_FAILURE);
            }
        }

        // run the program with the given arguments
        run_program(e_value);
    } else {
        chat_handler();
    }

    // end the program
    cleanup_and_exit(EXIT_SUCCESS);
}