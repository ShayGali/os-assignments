#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Define server IP and port
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4269
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int get_response = 0;
    int opt;
    while ((opt = getopt(argc, argv, "r")) != -1) {
        switch (opt) {
            case 'r':
                get_response = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s -r\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0) {
        perror("invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Print success message
    printf("Connected to server - %s:%d\n", SERVER_IP, SERVER_PORT);
    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        if (get_response) {
            int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received == -1) {
                perror("receive failed");
                exit(EXIT_FAILURE);
            } else if (bytes_received == 0) {
                printf("peer has closed the connection\n");
                break;
            }

            buffer[bytes_received] = '\0';  // null-terminate the string
            printf("%s", buffer);
        }
        if ((fgets(buffer, BUFFER_SIZE, stdin)) == NULL) {
            perror("error reading input");
            exit(EXIT_FAILURE);
        }
        int bytes_sent = send(sock, buffer, strlen(buffer), 0);
        if (bytes_sent == -1) {
            perror("send failed");
            exit(EXIT_FAILURE);
        } else if (bytes_sent == 0) {
            printf("peer has closed the connection\n");
            break;
        } else if (bytes_sent < strlen(buffer)) {
            printf("sent only %d bytes of %ld\n", bytes_sent, strlen(buffer));
            exit(EXIT_FAILURE);
        }
    }

    close(sock);
    return 0;
}