#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

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

int main() {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Disable os icmp network stack
    system("echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_all");

    // Create a raw socket for ICMP
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    printf("ICMP server listening...\n");

    while (1) {
        // Receive ICMP packet
        int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Failed to receive packet");
            continue;
        }

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));

        printf("Received ICMP packet: Type=%d, Code=%d\n", icmp_header->type, icmp_header->code);

        // Prepare ICMP Echo Reply
        if (icmp_header->type == ICMP_ECHO) {
            char *data = buffer + (ip_header->ihl * 4) + sizeof(struct icmphdr);
            int data_len = bytes_received - (ip_header->ihl * 4) - sizeof(struct icmphdr);

            // Modify ICMP header for Echo Reply
            icmp_header->type = ICMP_ECHOREPLY;
            icmp_header->checksum = 0;
            icmp_header->checksum = checksum((unsigned short *)icmp_header, sizeof(struct icmphdr) + data_len);

            // Send ICMP reply
            if (sendto(sockfd, buffer + (ip_header->ihl * 4), sizeof(struct icmphdr) + data_len, 0, 
                       (struct sockaddr *)&client_addr, addr_len) < 0) {
                perror("Failed to send ICMP reply");
            } else {
                printf("ICMP reply sent\n");
            }
        }
    }
    
    return 0;
}
