#ifndef PROTOCOL_TASK_H
#define PROTOCOL_TASK_H

typedef struct {
    const char* command_name;
    void (*command_callback)(const char* args);
    const char* command_help;
} api_t;

void protocol_task_init(api_t* device_api);
void protocol_task_handle(char* command_string);

#endif