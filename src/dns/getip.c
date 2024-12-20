#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [hostname]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct hostent *h;

    h = gethostbyname(argv[1]);
    if (h == NULL) {
        herror("gethostbyname");
        return EXIT_FAILURE;
    }

    printf("Hostname: %s\n", h->h_name);
    printf("IP Address: %s\n", inet_ntoa(*(struct in_addr*)(h->h_addr)));

    return EXIT_SUCCESS;
}