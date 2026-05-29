#include "uart1_bridge.h"
#include <riz/hal/uart.h>

extern riz::hal::uart g_uart1;

void uart1_on_tx_complete_isr(void) {
    g_uart1.on_transmit_complete_isr();
}

void uart1_on_rx_complete_isr(void) {
    g_uart1.on_receive_complete_isr();
}

void uart1_on_rx_idle_isr(void) {
    g_uart1.on_receive_idle_isr();
}

void uart1_on_error_isr(void) {
    g_uart1.on_error_isr();
}
