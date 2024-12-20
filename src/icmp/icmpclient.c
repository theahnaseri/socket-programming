#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <time.h>

#define BUFFER_SIZE 1024

// Calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [ip]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *ip = argv[1];
    
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create a raw socket for ICMP
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // Prepare ICMP Echo Request
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = getpid() & 0xFFFF;
    icmp_header.un.echo.sequence = 1;
    icmp_header.checksum = 0;

    char *data = "Hello ICMP";
    int data_len = strlen(data);

    memcpy(buffer, &icmp_header, sizeof(icmp_header));
    memcpy(buffer + sizeof(icmp_header), data, data_len);

    // Calculate checksum
    icmp_header.checksum = checksum(buffer, sizeof(icmp_header) + data_len);
    memcpy(buffer, &icmp_header, sizeof(icmp_header));

    // Send ICMP request
    if (sendto(sockfd, buffer, sizeof(icmp_header) + data_len, 0, 
               (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to send packet");
        exit(EXIT_FAILURE);
    }
    printf("ICMP request sent to %s\n", ip);

    // Receive ICMP reply
    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);
    int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&reply_addr, &addr_len);
    if (bytes_received < 0) {
        perror("Failed to receive packet");
    } else {
        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
        printf("Received ICMP reply: Type=%d, Code=%d\n", icmp_reply->type, icmp_reply->code);
        printf("Data: %s\n", buffer + (ip_header->ihl * 4) + sizeof(struct icmphdr));
    }

    close(sockfd);
    
    return 0;
}
