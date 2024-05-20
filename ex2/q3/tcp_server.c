#include <stdio.h>

// Linux and other UNIXes
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5060  // The port that the server listens
#define BUFFER_SIZE 1024

int main() {
    int listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == -1) {
        printf("Could not create listening socket : %d", errno);
        exit(EXIT_FAILURE);
    }

    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0) {
        printf("setsockopt() failed with error code : %d", errno);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;  // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1) {
        printf("Bind failed with error code : %d", errno);
        // close the socket
        close(listeningSocket);
        exit(EXIT_FAILURE);
    }

    int listenResult = listen(listeningSocket, 3);
    if (listenResult == -1) {
        printf("listen() failed with error code : %d", errno);
        // close the socket
        close(listeningSocket);
        exit(EXIT_FAILURE);
    }

    // Accept and incoming connection
    printf("Listening on %d\n", SERVER_PORT);
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        // accept() will block until a client connects to the server
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);

        if (clientSocket == -1) {  // if accept failed
            printf("listen failed with error code : %d", errno);
            // close the sockets
            close(listeningSocket);
            exit(EXIT_FAILURE);
        }

        printf("A new client connection accepted\n");

        // loop until the client closes the connection
        while (1) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead == 0) {
                printf("Client disconnected\n");
                break;
            }

            if (bytesRead == -1) {
                printf("recv() failed with error code : %d", errno);
                break;
            }

            printf("%s", buffer);
        }

        close(clientSocket);
    }

    close(listeningSocket);

    return 0;
}