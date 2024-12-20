#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define FILE_NAME "file.txt"

int main() {
    // Open the file
    int fd = open(FILE_NAME, O_RDONLY); 
    char buffer[256];
    // Read 256 bytes
    read(fd, buffer, 256);
    printf("%s [%s]\n", FILE_NAME, buffer);
    // Close the file
    close(fd); 
    return 0;
}