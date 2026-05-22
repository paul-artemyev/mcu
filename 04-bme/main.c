#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "led-task/led-task.h"
#include "stdio-task/stdio-task.h"
#include "../libs/protocol/include/protocol-task.h"
#include "../libs/bme280-driver/include/bme280-driver.h"
#include "../libs/bme280-driver/include/bme280-regs.h"

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
    printf("  write_reg <hex_addr> <hex_value> - write BME280 register\n");
    printf("  bme280_init - initialize BME280 with oversampling x1, normal mode\n");
    printf("  temp_raw - read raw temperature value (20-bit)\n");
    printf("  pres_raw - read raw pressure value (20-bit)\n");
    printf("  hum_raw - read raw humidity value (16-bit)\n");
    printf("  read_all - read all three raw values\n");
    printf("  temp - read temperature in °C\n");
    printf("  pres - read pressure in hPa\n");
    printf("  hum - read humidity in %%\n");
    printf("  all - read all measurements in SI units\n");
    printf("  help - print this help\n");
}

int32_t read_temp_20bit(void)
{
    uint8_t data[3] = {0};
    bme280_read_regs(BME280_REG_temp_msb, data, 3);
    return ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
}

uint32_t read_pres_20bit(void)
{
    uint8_t data[3] = {0};
    bme280_read_regs(BME280_REG_press_msb, data, 3);
    return ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
}

uint16_t read_hum_16bit(void)
{
    uint8_t data[2] = {0};
    bme280_read_regs(BME280_REG_hum_msb, data, 2);
    return (data[0] << 8) | data[1];
}

void temp_raw_callback(const char* args) 
{ 
    int32_t raw = bme280_read_temp_raw();
    printf("Temp raw (20-bit): %ld (0x%05lX)\n", raw, raw); 
}

void pres_raw_callback(const char* args) 
{ 
    uint32_t raw = bme280_read_pres_raw();
    printf("Press raw (20-bit): %lu (0x%05lX)\n", raw, raw); 
}

void hum_raw_callback(const char* args)  
{ 
    uint32_t raw = bme280_read_hum_raw();
    printf("Hum raw (16-bit): %lu (0x%04lX)\n", raw, raw); 
}

void bme280_init_callback(const char* args)
{
    uint8_t id;
    bme280_read_regs(BME280_REG_id, &id, 1);
    if (id != 0x60) { 
        printf("ERROR: BME280 not detected! ID=0x%02X\n", id); 
        return; 
    }
    printf("BME280 detected (ID: 0x%02X)\n", id);
    
    bme280_write_reg(BME280_REG_ctrl_hum, 0b001 << 0);  
    printf("Ctrl_hum configured: 0x%02X\n", 0b001 << 0);
    
    bme280_write_reg(BME280_REG_config, (0b001 << 5)); 
    printf("Config configured: 0x%02X\n", 0b001 << 5);
    
    uint8_t ctrl_val = (0b001 << 5) | (0b001 << 2) | (0b11 << 0);
    bme280_write_reg(BME280_REG_ctrl_meas, ctrl_val);
    printf("Ctrl_meas configured: 0x%02X\n", ctrl_val);
    
    printf("BME280 initialized\n");
    sleep_ms(100);
}

void temp_callback(const char* args)
{
    float temp = bme280_read_temperature();
    printf("Temperature: %.2f °C\n", temp);
}

void pres_callback(const char* args)
{
    float press = bme280_read_pressure();
    printf("Pressure: %.2f hPa (%.2f Pa)\n", press, press * 100.0f);
}

void hum_callback(const char* args)
{
    float hum = bme280_read_humidity();
    printf("Humidity: %.2f %%\n", hum);
}


api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"read_regs", read_regs_callback, "read BME280 registers. Usage: read_regs <hex_addr> <hex_count>"},
    {"write_reg", write_reg_callback, "write BME280 register. Usage: write_reg <hex_addr> <hex_value>"},
    {"bme280_init", bme280_init_callback, "initialize BME280 with oversampling x1, normal mode"},
    
    {"temp_raw", temp_raw_callback, "read raw temperature value (20-bit)"},
    {"pres_raw", pres_raw_callback, "read raw pressure value (20-bit)"},
    {"hum_raw", hum_raw_callback, "read raw humidity value (16-bit)"},
    
    {"temp", temp_callback, "read temperature in °C"},
    {"pres", pres_callback, "read pressure in hPa (hectopascals)"},
    {"hum", hum_callback, "read humidity in %"},
    
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