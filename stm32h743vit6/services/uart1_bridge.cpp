#include "uart1_bridge.h"
#include <riz/io/uart_service.h>

extern riz::io::uart_service g_uart1_svc;

void uart1_on_tx_complete_isr(void) {
    g_uart1_svc.on_tx_complete();
}

void uart1_on_rx_complete_isr(void) {
    g_uart1_svc.on_rx_complete();
}

void uart1_on_rx_idle_isr(void) {
    g_uart1_svc.on_rx_idle();
}

void uart1_on_error_isr(void) {
    g_uart1_svc.on_rx_error();
}
