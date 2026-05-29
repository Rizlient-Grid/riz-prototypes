#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void uart1_on_tx_complete_isr(void);
void uart1_on_rx_complete_isr(void);
void uart1_on_rx_idle_isr(void);
void uart1_on_error_isr(void);

#ifdef __cplusplus
}
#endif
