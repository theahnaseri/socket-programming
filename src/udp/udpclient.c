#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s [ip] [port]\n\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* ip = argv[1];
    const int port = atoi(argv[2]);

    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    socklen_t len = sizeof(servaddr);

    while (1) {
        // Get input from user
        printf("Enter message: ");
        fgets(buffer, BUF_SIZE, stdin);

        // Send message to server
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);
        printf("Request send to %s:%i\n", ip, port);

        // Receive response from server
        memset(buffer, 0, BUF_SIZE);
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        buffer[n] = '\0';
        printf("Server: %s\n\n", buffer);
    }

    close(sockfd);
    return 0;
}
