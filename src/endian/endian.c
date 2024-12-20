#include <stdio.h>

/*Function to show bytes in memory, from location start to start+n*/
void show_mem_rep(unsigned char *start, int size){
    for (int i = 0; i < size; i++) {
        printf("    %.2X\n", start[i]);
    }
    printf("\n");
}

/*Main function to call above function for 0x01234567*/
int main(){
    int num = 0x0A0B0C0D;
    printf("num: 0x%08X\n", num);
    printf("mem: \n");
    show_mem_rep((unsigned char *)&num, sizeof(num));
    return 0;
}