#ifndef UART_MANAGER_H
#define UART_MANAGER_H

typedef struct {
    char code;
    char *str;
} callback_entry_t;

typedef struct {
    char code;
    int arg;
} callback_params_t;

typedef void (*callback_func)(void);

callback_params_t *uart_manager_init(void *table, int table_len, callback_func func);
void uart_rx_task(void *pvParameters);
void input_handler(char input);
void log_str(const char* fmt, ...);

#endif // UART_MANAGER_H
