#include "uart_manager.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <stdarg.h>
#include <string.h>
#include <pthread.h>

#define TX_PIN 1
#define RX_PIN 3
#define UART_NUM UART_NUM_0

#define LOG_BUF_SIZE 64
#define NEWLINE "\n\r"

static bool menu = false;
static bool waiting_for_arg;

static callback_entry_t *callbacks;
static int callback_str_table_len;
callback_func callback;
callback_params_t *callback_params;

static void _print(const char* fmt, va_list args) {
    char log_buf[LOG_BUF_SIZE];
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);

    uart_write_bytes(UART_NUM, log_buf, strlen(log_buf));
    uart_write_bytes(UART_NUM, NEWLINE, strlen(NEWLINE));
}

static void log_internal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _print(fmt, args);
    va_end(args);
}

callback_params_t *uart_manager_init(void *table, int table_len, callback_func func) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, 2048, 2048, 0, NULL, 0);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    callbacks = (callback_entry_t *)table;
    callback_str_table_len = table_len;
    callback = func;

    xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 10, NULL);

    callback_params = (callback_params_t *)malloc(sizeof(callback_params_t));
    return callback_params;
}

void uart_rx_task(void *pvParameters) {
    char rx_buffer[8];

    while(true) {
        int len = uart_read_bytes(UART_NUM, rx_buffer, sizeof(rx_buffer) - 1, 20 / portTICK_RATE_MS);
        if (len > 0) {
            input_handler(rx_buffer[0]);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void input_handler(char input) {
    if(input == 'q') {
        menu = true;
        waiting_for_arg = false;
        for(int x = 0; x < callback_str_table_len; x++) {
            log_internal("%c: %s", callbacks[x].code, callbacks[x].str);
        }
        return;
    } else if(input == 'c') {
        menu = false;
        waiting_for_arg = false;
        return;
    }

    if(!menu)
        return;

    if(input >= 97 && input <= 122) {
        callback_params->code = input;
        waiting_for_arg = true;
    } else if(input >= 48 && input <= 57 && waiting_for_arg) {
        waiting_for_arg = false;
        menu = false;
        callback_params->arg = input - 48;
        callback();
    }

}


void log_str(const char* fmt, ...) {
    if(menu)
        return;

    va_list args;
    va_start(args, fmt);
    _print(fmt, args);
    va_end(args);
}
