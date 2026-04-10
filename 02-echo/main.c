#include<stdio.h>
#include<stdlib.h>
#include<pico/stdlib.h>

#define LED_PIN 25

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        char symbol = getchar();
        printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
        if(symbol == 'e') gpio_put(LED_PIN, 1);
        if(symbol == 'v') gpio_put(LED_PIN, 0);
        if(symbol == 'd') printf("led enable done\n");
    }
}