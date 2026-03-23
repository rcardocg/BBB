#include "../lib/stdio.h"

void main(){
    int n = 0;
    while(1){
        PRINT("P1: %d\n", n);
        n = (n+1)%10;
    }
} 