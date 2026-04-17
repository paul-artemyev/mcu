#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#define DEVICE_NAME "RP2040 Control Device"
#define DEVICE_VRSN "v1.0.0"

void version_callback(const char* args)
{
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
    printf("LED turned ON\n");
}

void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
    printf("LED turned OFF\n");
}

void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
    printf("LED blinking mode activated\n");
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint32_t period_ms = 0;
    sscanf(args, "%u", &period_ms);
    
    if (period_ms == 0)
    {
        printf("Error: invalid period. Usage: set_period <milliseconds>\n");
        return;
    }
    
    led_task_set_blink_period_ms(period_ms);
    printf("Blink period set to %u ms\n", period_ms);
}

void help_callback(const char* args);

void help_callback(const char* args)
{
    printf("Available commands:\n");
    for (int i = 0; i < sizeof(device_api) / sizeof(api_t) - 1; i++)
    {
        if (device_api[i].command_name != NULL)
        {
            printf("  %s - %s\n", device_api[i].command_name, device_api[i].command_help);
        }
    }
}

api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "switch on led"},
    {"off", led_off_callback, "switch off led"},
    {"blink", led_blink_callback, "provide unblocking blinking"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in milliseconds"},
    {"help", help_callback, "print commands description"},
    {NULL, NULL, NULL},
};

int main()
{
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();

    char* received_string = NULL;

    while (1)
    {
        received_string = stdio_task_handler();
        if (received_string != NULL)
        {
            protocol_task_handle(received_string);
        }
        led_task_handler();
    }

    return 0;
}