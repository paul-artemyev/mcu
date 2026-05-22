#include "led-task/led-task.h"
#include "pico/stdlib.h"

static bool led_state = false;
static absolute_time_t last_blink_time = {0};
static uint32_t blink_period_ms = 500;  // период мигания по умолчанию 500 мс

void led_task_init(void) 
{
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);
    last_blink_time = get_absolute_time();
}

void led_task_handler(void) 
{
    absolute_time_t now = get_absolute_time();
    
    if (absolute_time_diff_us(last_blink_time, now) > (blink_period_ms * 1000)) {
        last_blink_time = now;
        led_state = !led_state;
        gpio_put(25, led_state);
    }
}

void led_task_set_blink_period_ms(uint32_t period_ms) 
{
    blink_period_ms = period_ms;
}

void led_task_on(void) 
{
    gpio_put(25, 1);
    led_state = true;
}

void led_task_off(void) 
{
    gpio_put(25, 0);
    led_state = false;
}

void led_task_toggle(void) 
{
    led_state = !led_state;
    gpio_put(25, led_state);
}