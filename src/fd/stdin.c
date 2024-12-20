#include <stdio.h>
#include <unistd.h>

int main() {
    char buffer[256];
    printf("stdin: ");
    fflush(stdout);
    // Read 256 bytes from stdin
    read(0, buffer, 256);
    printf("buffer: [%s]\n", buffer);
    return 0;
}