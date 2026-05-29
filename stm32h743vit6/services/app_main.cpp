#include "app_main.h"

#include <riz/coro/execution/execute.hpp>
#include <riz/hal/uart.h>
#include <riz/io/uart_service.h>
#include <riz/timer/timer_service.h>

#include "led_blink.h"
#include "uart1_echo.h"
#include "uart_hello.h"

#include <main.h>

extern UART_HandleTypeDef huart1;

namespace {
riz::coro::execution::scheduler g_sched;
std::byte g_uart1_rx_buffer[128];
}

riz::hal::uart g_uart1 {huart1, riz::hal::uart::transfer_mode::dma, g_uart1_rx_buffer};
riz::io::uart_service g_uart1_svc {g_uart1};

void app_main() {
    auto task_led_blink = riz::coro::execution::start(led_blink(g_sched));
    auto task_uart_echo = riz::coro::execution::start(uart1_echo(g_sched, g_uart1_svc));
    auto task_uart_hello = riz::coro::execution::start(uart_hello(g_sched, g_uart1_svc));

    g_uart1.start_receive(g_uart1_rx_buffer, sizeof(g_uart1_rx_buffer));
    g_uart1.enable_irq();
    for (;;) {
        riz::timer::timer_service::instance().run();
        g_uart1_svc.run();
        g_sched.run();
    }
}
