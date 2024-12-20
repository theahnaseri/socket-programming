#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    char buffer[BUFFER_SIZE];
    int nbyte;

    while (1) {
        memset(buffer, 0x00, BUFFER_SIZE);
        printf("stdin: ");
        fflush(stdout);
        // Avoid busy-waiting
        sleep(2);
        // Read 256 bytes from stdin
        nbyte = read(0, buffer, BUFFER_SIZE-1);
        if (nbyte > 0) {
            printf("buffer: [%s]\n", buffer);
        }
        else {
            printf("nothing\n");
        }
    }
    
    return 0;
}