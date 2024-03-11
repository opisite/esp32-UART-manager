#include "uart_manager.h"
#include "driver/uart.h"
#include <string.h>

#define TX_PIN 1
#define RX_PIN 3
#define UART_NUM UART_NUM_0

#define NEWLINE "\r\n"

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

void uart_print(const char* message) {
    uart_write_bytes(UART_NUM, message, strlen(message));
    uart_write_bytes(UART_NUM, &NEWLINE, 2);
}