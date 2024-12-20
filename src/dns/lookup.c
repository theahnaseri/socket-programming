#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [hostname]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *hostname = argv[1];
    struct addrinfo hints, *res, *p;
    char ip_str[INET6_ADDRSTRLEN];

    // Initialize the hints structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP Stream sockets

    // Get address info
    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    printf("IP addresses for %s:\n", hostname);

    // Loop through the results
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        const char *ipver;

        // Get pointer to the address itself
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // Convert the IP to a string and print it
        inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));
        printf("  %s: %s\n", ipver, ip_str);
    }

    freeaddrinfo(res); // Free the linked list
    return EXIT_SUCCESS;
}
