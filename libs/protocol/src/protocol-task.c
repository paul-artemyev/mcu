#include "protocol-task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static api_t* api = NULL;
static int commands_count = 0;

void protocol_task_init(api_t* device_api)
{
    api = device_api;
    commands_count = 0;
    while (api[commands_count].command_name != NULL)
    {
        commands_count++;
    }
}

void protocol_task_handle(char* command_string)
{
    if (!command_string)
    {
        return;
    }

    const char* command_name = command_string;
    const char* command_args = NULL;

    char* space_symbol = strchr(command_string, ' ');

    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        command_args = "";
    }

    printf("received command: '%s' with args: '%s'\n", command_name, command_args);

    for (int i = 0; i < commands_count; i++)
    {
        if (strcmp(command_name, api[i].command_name) == 0)
        {
            printf("Command '%s': '%s'\n", api[i].command_name, api[i].command_help);
            api[i].command_callback(command_args);
            return;
        }
    }

    printf("Command not found: '%s'\n", command_name);
    return;
}

void mem_command_callback(const char* args)
{
    if (!args || strlen(args) == 0) {
        printf("Error: address required. Usage: mem <hex_address>\n");
        return;
    }
    
    uint32_t address = strtoul(args, NULL, 16);
    
    uint32_t value = *(volatile uint32_t*)address;
    
    printf("Value at address 0x%08X: 0x%08X (%u)\n", address, value, value);
}

void wmem_command_callback(const char* args)
{
    if (!args || strlen(args) == 0) {
        printf("Error: address and value required. Usage: wmem <hex_address> <hex_value>\n");
        return;
    }
    
    char arg1[20], arg2[20];
    int parsed = sscanf(args, "%s %s", arg1, arg2);
    
    if (parsed != 2) {
        printf("Error: both address and value required. Usage: wmem <hex_address> <hex_value>\n");
        return;
    }
    
    uint32_t address = strtoul(arg1, NULL, 16);
    uint32_t value = strtoul(arg2, NULL, 16);
    
    *(volatile uint32_t*)address = value;
    
    printf("Written 0x%08X to address 0x%08X\n", value, address);
}