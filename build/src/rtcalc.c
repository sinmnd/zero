#include <stdio.h>
#include <stdlib.h>

void calc_write(int v) {
    printf("The result is: %d\n", v);
}

int calc_read(char *str) {
    char buf[64];
    int val;
    printf("Enter a value for %s: ", str);
    fgets(buf, sizeof(buf), stdin);
    if(EOF == sscanf(buf, "%d", &val)) {
        printf("Invalid value: %s\n", buf);
        exit(EXIT_FAILURE);
    }
}