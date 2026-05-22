#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "mem-task/mem-task.h"

#include "ili9341-driver.h"
#include "ili9341-display.h"
#include "ili9341-front.h"

extern const ili9341_font_t jetbrains_font;

#define DEVICE_NAME "display-pico-device"
#define DEVICE_VRSN "v1.0.0"

#define ILI9341_PIN_MISO 4
#define ILI9341_PIN_CS   10
#define ILI9341_PIN_SCK  6
#define ILI9341_PIN_MOSI 7
#define ILI9341_PIN_DC   8
#define ILI9341_PIN_RESET 9

static ili9341_display_t ili9341_display = {0};

void rp2040_spi_write(const uint8_t* data, uint32_t size) {
    spi_write_blocking(spi0, data, size);
}

void rp2040_spi_read(uint8_t* buffer, uint32_t length) {
    spi_read_blocking(spi0, 0, buffer, length);
}

void rp2040_gpio_cs_write(bool level) {
    gpio_put(ILI9341_PIN_CS, level);
}

void rp2040_gpio_dc_write(bool level) {
    gpio_put(ILI9341_PIN_DC, level);
}

void rp2040_gpio_reset_write(bool level) {
    gpio_put(ILI9341_PIN_RESET, level);
}

void rp2040_delay_ms(uint32_t ms) {
    sleep_ms(ms);
}

void display_init(void) {
    ili9341_hal_t ili9341_hal = {0};
    ili9341_hal.spi_write = rp2040_spi_write;
    ili9341_hal.spi_read = rp2040_spi_read;
    ili9341_hal.gpio_cs_write = rp2040_gpio_cs_write;
    ili9341_hal.gpio_dc_write = rp2040_gpio_dc_write;
    ili9341_hal.gpio_reset_write = rp2040_gpio_reset_write;
    ili9341_hal.delay_ms = rp2040_delay_ms;
    
    ili9341_init(&ili9341_display, &ili9341_hal);
    ili9341_set_rotation(&ili9341_display, ILI9341_ROTATION_90);
    ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    sleep_ms(300);
}

void version_callback(const char* args) {
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args) {
    led_task_state_set(LED_STATE_ON);
}

void led_off_callback(const char* args) {
    led_task_state_set(LED_STATE_OFF);
}

void led_blink_callback(const char* args) {
    led_task_state_set(LED_STATE_BLINK);
}

void led_blink_set_period_ms_callback(const char* args) {
    uint32_t period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms == 0) return;
    led_task_set_blink_period_ms(period_ms);
}

void mem_callback(const char* args) {
    uint32_t addr;
    sscanf(args, "%x", &addr);
    mem(addr);
}

void wmem_callback(const char* args) {
    uint32_t addr = 0, value = 0;
    sscanf(args, "%x %x", &addr, &value);
    wmem(addr, value);
}

void disp_screen_callback(const char* args) {
    uint32_t c = 0;
    int result = sscanf(args, "%x", &c);
    uint16_t color = COLOR_BLACK;
    if (result == 1) color = RGB888_2_RGB565(c);
    ili9341_fill_screen(&ili9341_display, color);
}

void disp_px_callback(const char* args) {
    if (!args || strlen(args) == 0) return;
    
    int x, y;
    uint32_t color_hex;
    
    if (sscanf(args, "%d %d %x", &x, &y, &color_hex) != 3) return;
    if (x < 0 || x >= 320 || y < 0 || y >= 240) return;
    
    uint16_t color = RGB888_2_RGB565(color_hex);
    ili9341_draw_pixel(&ili9341_display, (uint16_t)x, (uint16_t)y, color);
}

void disp_line_callback(const char* args) {
    if (!args || strlen(args) == 0) return;
    
    int x0, y0, x1, y1;
    uint32_t color_hex;
    
    if (sscanf(args, "%d %d %d %d %x", &x0, &y0, &x1, &y1, &color_hex) != 5) return;
    
    if (x0 < 0 || x0 >= 320 || x1 < 0 || x1 >= 320 ||
        y0 < 0 || y0 >= 240 || y1 < 0 || y1 >= 240) return;
    
    uint16_t color = RGB888_2_RGB565(color_hex);
    ili9341_draw_line(&ili9341_display, (uint16_t)x0, (uint16_t)y0, 
                      (uint16_t)x1, (uint16_t)y1, color);
}

void disp_rect_callback(const char* args) {
    if (!args || strlen(args) == 0) return;
    
    int x, y, w, h;
    uint32_t color_hex;
    
    if (sscanf(args, "%d %d %d %d %x", &x, &y, &w, &h, &color_hex) != 5) return;
    
    if (x < 0 || x >= 320 || y < 0 || y >= 240 || w <= 0 || h <= 0) return;
    
    if (x + w > 320) w = 320 - x;
    if (y + h > 240) h = 240 - y;
    
    uint16_t color = RGB888_2_RGB565(color_hex);
    ili9341_draw_rect(&ili9341_display, (uint16_t)x, (uint16_t)y, 
                      (uint16_t)w, (uint16_t)h, color);
}

void disp_frect_callback(const char* args) {
    if (!args || strlen(args) == 0) return;
    
    int x, y, w, h;
    uint32_t color_hex;
    
    if (sscanf(args, "%d %d %d %d %x", &x, &y, &w, &h, &color_hex) != 5) return;
    
    if (x < 0 || x >= 320 || y < 0 || y >= 240 || w <= 0 || h <= 0) return;
    
    if (x + w > 320) w = 320 - x;
    if (y + h > 240) h = 240 - y;
    
    uint16_t color = RGB888_2_RGB565(color_hex);
    ili9341_draw_filled_rect(&ili9341_display, (uint16_t)x, (uint16_t)y, 
                             (uint16_t)w, (uint16_t)h, color);
}

void disp_text_callback(const char* args) {
    if (!args || strlen(args) == 0) return;
    
    int x, y;
    char text[100];
    uint32_t color_hex, bg_color_hex;
    
    if (sscanf(args, "%d %d %99s %x %x", &x, &y, text, &color_hex, &bg_color_hex) != 5) return;
    if (x < 0 || x >= 320 || y < 0 || y >= 240) return;
    
    uint16_t color = RGB888_2_RGB565(color_hex);
    uint16_t bg_color = RGB888_2_RGB565(bg_color_hex);
    
    ili9341_draw_text(&ili9341_display, (uint16_t)x, (uint16_t)y, text, 
                      &jetbrains_font, color, bg_color);
}

void help_callback(const char* args);

api_t device_api[] = {
    {"version", version_callback, "get device name and firmware version"},
    {"on",      led_on_callback,  "turn LED on"},
    {"off",     led_off_callback, "turn LED off"},
    {"blink",   led_blink_callback, "LED blinking mode"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in ms"},
    {"mem", mem_callback, "read memory: mem <hex_addr>"},
    {"wmem", wmem_callback, "write memory: wmem <hex_addr> <hex_value>"},
    {"disp_screen", disp_screen_callback, "fill screen: disp_screen <color_hex>"},
    {"disp_px", disp_px_callback, "draw pixel: disp_px <x> <y> <color_hex>"},
    {"disp_line", disp_line_callback, "draw line: disp_line <x0> <y0> <x1> <y1> <color_hex>"},
    {"disp_rect", disp_rect_callback, "draw rectangle: disp_rect <x> <y> <w> <h> <color_hex>"},
    {"disp_frect", disp_frect_callback, "draw filled rectangle: disp_frect <x> <y> <w> <h> <color_hex>"},
    {"disp_text", disp_text_callback, "draw text: disp_text <x> <y> <text> <color_hex> <bg_color_hex>"},
    {"help", help_callback, "show this help"},
    {NULL, NULL, NULL}
};

void help_callback(const char* args) {
    printf("\n=== Available Commands ===\n");
    for (int i = 0; device_api[i].command_name != NULL; i++) {
        printf("  %-12s - %s\n", device_api[i].command_name, device_api[i].command_help);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("\n=== Display Control Device ===\n");
    printf("Firmware: %s\n", DEVICE_VRSN);
    printf("Type 'help' for commands\n\n");
    
    led_task_init();
    stdio_task_init();
    protocol_task_init(device_api);
    
    spi_init(spi0, 62500000);
    gpio_set_function(ILI9341_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(ILI9341_PIN_CS);
    gpio_init(ILI9341_PIN_DC);
    gpio_init(ILI9341_PIN_RESET);
    gpio_set_dir(ILI9341_PIN_CS, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_DC, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_RESET, GPIO_OUT);
    
    gpio_put(ILI9341_PIN_CS, 1);
    gpio_put(ILI9341_PIN_DC, 0);
    gpio_put(ILI9341_PIN_RESET, 0);
    sleep_ms(10);
    gpio_put(ILI9341_PIN_RESET, 1);
    sleep_ms(120);
    
    printf("SPI initialized for ILI9341\n");
    
    display_init();
    
    char* received_string = NULL;
    while (1) {
        received_string = stdio_task_handler();
        if (received_string != NULL) {
            protocol_task_handle(received_string);
        }
        led_task_handler();
        sleep_ms(10);
    }
    
    return 0;
}