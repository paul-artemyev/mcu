#include<stdio.h>
#include<stdlib.h>
#include<pico/stdlib.h>

int main()
{
    stdio_init_all();
    while (1)
    {
        char symbol = getchar();
        printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
    }
}