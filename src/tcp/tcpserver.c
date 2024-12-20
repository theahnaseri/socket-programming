#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s [port]\n\n", argv[0]);
        return EXIT_FAILURE;
    }
    const int port = atoi(argv[1]);

    int server_sock, client_sock;
    
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addrlen;

    char buffer[BUFFER_SIZE];
    int nbytes;

    // Create server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket failed");
        return EXIT_FAILURE;
    }

    // Bind server socket to the address
    memset(&server_addr, 0x00, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(server_sock, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_sock);
        return EXIT_FAILURE;
    }

    printf("Server is listening on port %d\n\n", port);

    // Accept a connection
    client_addrlen = sizeof(struct sockaddr_in);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addrlen);
    if (client_sock < 0) {
        perror("Accept failed");
        close(server_sock);
        return EXIT_FAILURE;
    }

    printf("%s connected!\n\n", inet_ntoa(client_addr.sin_addr));
    
    while (1) {
        // Recv data
        memset(buffer, 0x00, BUFFER_SIZE);
        nbytes = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (nbytes < 0) {
            perror("Recv failed");
            close(server_sock);
            close(client_sock);
            return EXIT_FAILURE;
        }
        printf("Client: %s\n", buffer);

        // Send data
        memset(buffer, 0x00, BUFFER_SIZE);
        printf("Server: ");
        fflush(stdout);
        nbytes = read(0, buffer, BUFFER_SIZE);
        if (nbytes < 0) {
            perror("STDIN read failed");
            close(server_sock);
            close(client_sock);
            return EXIT_FAILURE;
        }
        printf("\n");
        nbytes = send(client_sock, buffer, BUFFER_SIZE, 0);
        if (nbytes < 0) {
            perror("Send failed");
            close(server_sock);
            close(client_sock);
            return EXIT_FAILURE;
        }
    }

    // Close the sockets
    close(server_sock);
    close(client_sock);

    return EXIT_SUCCESS;
}
