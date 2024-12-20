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
    if (argc < 3) {
        printf("Usage: %s [ip] [port]\n\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* ip = argv[1];
    const int port = atoi(argv[2]);

    int client_sock;
    struct sockaddr_in server_addr;

    char buffer[BUFFER_SIZE];
    int nbytes;

    // Create socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket failed");
        return EXIT_FAILURE;
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Connect to server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Connection failed");
        close(client_sock);
        return EXIT_FAILURE;
    }

    printf("Connect to %s:%i\n\n", ip, port);
    
    while (1) {
        // Send data
        memset(buffer, 0x00, BUFFER_SIZE);
        printf("Client: ");
        fflush(stdout);
        nbytes = read(0, buffer, BUFFER_SIZE);
        if (nbytes < 0) {
            perror("STDIN read failed");
            close(client_sock);
            return EXIT_FAILURE;
        }
        printf("\n");
        nbytes = send(client_sock, buffer, BUFFER_SIZE, 0);
        if (nbytes < 0) {
            perror("Send failed");
            close(client_sock);
            return EXIT_FAILURE;
        }

        // Recv Data
        memset(buffer, 0x00, BUFFER_SIZE);
        nbytes = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (nbytes < 0) {
            perror("Recv failed");
            close(client_sock);
            return EXIT_FAILURE;
        }
        printf("Server: %s\n", buffer);
    }
    

    // Close the socket
    close(client_sock);

    return EXIT_SUCCESS;
}
