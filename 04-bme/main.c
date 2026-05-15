#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "led-task/led-task.h"
#include "stdio-task/stdio-task.h"
#include "../libs/protocol/include/protocol-task.h"
#include "../libs/bme280-driver/include/bme280-driver.h"
#include "led-task/led-task.h"
#include "stdio-task/stdio-task.h"


#define DEVICE_NAME "BME280 Control Device"
#define DEVICE_VRSN "v1.0.0"
#define BME280_I2C_ADDR 0x76

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
    printf("I2C read: addr=0x%02X, length=%d\n", BME280_I2C_ADDR, length);
    int result = i2c_read_timeout_us(i2c1, BME280_I2C_ADDR, buffer, length, false, 100000);
    if (result < 0) {
        printf("I2C read error: %d\n", result);
    }
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
    printf("I2C write: addr=0x%02X, size=%d, data[0]=0x%02X\n", BME280_I2C_ADDR, size, data[0]);
    int result = i2c_write_timeout_us(i2c1, BME280_I2C_ADDR, data, size, false, 100000);
    if (result < 0) {
        printf("I2C write error: %d\n", result);
    }
}

void version_callback(const char* args)
{
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void read_regs_callback(const char* args)
{
    if (!args || strlen(args) == 0) {
        printf("Error: address and count required. Usage: read_regs <hex_addr> <hex_count>\n");
        return;
    }
    
    char arg1[20], arg2[20];
    int parsed = sscanf(args, "%s %s", arg1, arg2);
    
    if (parsed != 2) {
        printf("Error: both address and count required. Usage: read_regs <hex_addr> <hex_count>\n");
        return;
    }
    
    uint32_t addr = strtoul(arg1, NULL, 16);
    uint32_t count = strtoul(arg2, NULL, 16);
    
    if (addr > 0xFF) {
        printf("Error: address must be <= 0xFF\n");
        return;
    }
    
    if (count > 0xFF) {
        printf("Error: count must be <= 0xFF\n");
        return;
    }
    
    if (addr + count > 0x100) {
        printf("Error: address + count must be <= 0x100\n");
        return;
    }
    
    uint8_t buffer[256] = {0};
    
    printf("Attempting to read from address 0x%02X, count %d...\n", addr, count);
    
    bme280_read_regs((uint8_t)addr, buffer, (uint8_t)count);
    
    printf("Reading %d registers starting from address 0x%02X:\n", count, addr);
    for (int i = 0; i < count; i++)
    {
        printf("bme280 register [0x%02X] = 0x%02X\n", addr + i, buffer[i]);
    }
}

void write_reg_callback(const char* args)
{
    if (!args || strlen(args) == 0) {
        printf("Error: address and value required. Usage: write_reg <hex_addr> <hex_value>\n");
        return;
    }
    
    char arg1[20], arg2[20];
    int parsed = sscanf(args, "%s %s", arg1, arg2);
    
    if (parsed != 2) {
        printf("Error: both address and value required. Usage: write_reg <hex_addr> <hex_value>\n");
        return;
    }
    
    uint32_t addr = strtoul(arg1, NULL, 16);
    uint32_t value = strtoul(arg2, NULL, 16);
    
    if (addr > 0xFF) {
        printf("Error: address must be <= 0xFF\n");
        return;
    }
    
    if (value > 0xFF) {
        printf("Error: value must be <= 0xFF\n");
        return;
    }
    
    bme280_write_reg((uint8_t)addr, (uint8_t)value);
    
    printf("Written 0x%02X to BME280 register [0x%02X]\n", value, addr);
}

void help_callback(const char* args)
{
    printf("Available commands:\n");
    printf("  version - get device name and firmware version\n");
    printf("  read_regs <hex_addr> <hex_count> - read BME280 registers\n");
    printf("  help - print commands description\n");
}

api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"read_regs", read_regs_callback, "read BME280 registers. Usage: read_regs <hex_addr> <hex_count>"},
    {"write_reg", write_reg_callback, "write BME280 register. Usage: write_reg <hex_addr> <hex_value>"},
    {"help", help_callback, "print commands description"},
    {NULL, NULL, NULL},
};
int main()
{
    stdio_init_all();
    sleep_ms(2000);
    
    i2c_init(i2c1, 100000);
    
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    
    gpio_pull_up(14);
    gpio_pull_up(15);
    
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);
    
    protocol_task_init(device_api);
    
    printf("\n=== BME280 Test Program ===\n");
    printf("Type 'help' for available commands\n\n");
    
    uint8_t chip_id = 0;
    bme280_read_regs(0xD0, &chip_id, 1);
    printf("BME280 Chip ID: 0x%02X (expected 0x60)\n", chip_id);
    
    if (chip_id == 0x60) {
        printf("BME280 detected successfully!\n");
    } else {
        printf("BME280 not detected! Check wiring and I2C address.\n");
    }
    printf("\n");
    
    char* received_string = NULL;
    
    while (1)
    {
        received_string = stdio_task_handler();
        if (received_string != NULL)
        {
            protocol_task_handle(received_string);
        }
    }
    
    return 0;
}