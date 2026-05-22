#pragma once
#include <stdint.h>

void mem_task_init(void);
void mem(uint32_t addr);
void wmem(uint32_t addr, uint32_t value);