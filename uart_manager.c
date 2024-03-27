#include "uart_manager.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <stdarg.h>
#include <string.h>

#define TX_PIN 1
#define RX_PIN 3
#define UART_NUM UART_NUM_0

#define LOG_BUF_SIZE 256
#define NEWLINE "\n\r"

void uart_manager_init(void) {
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
}

void log_str(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char log_buf[LOG_BUF_SIZE];
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);

    uart_write_bytes(UART_NUM, log_buf, strlen(log_buf));
    uart_write_bytes(UART_NUM, NEWLINE, strlen(NEWLINE));

}