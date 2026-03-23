#include "stdio.h"
#include "../os/os.h"
#include <stdarg.h>

void PRINT(const char *f, ...){
    va_list args;
    va_start(args, f);

    while(*f){
        //viene int
        if(*f =='%' && *(f+1) == 'd'){
            int inputP1 = va_arg(args, int);
            char intBuffer[16]; // el int tiene 16 caracteres papi
            uart_itoa(inputP1, intBuffer);
            uart_puts(intBuffer);
            f+=2; //que va después de %d
        }
        //viene caracter
        else if(*f == '%' && *(f+1) == 'c'){
            char inputP2 = (char)va_arg(args, int);
            uart_putc(inputP2);
            f+=2;
        }

        else{
            uart_putc(*f++);
        }

    }
    va_end(args);
}