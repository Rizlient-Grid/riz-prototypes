#include "app_main.h"

#include <riz/console/console.h>
#include <riz/console/logger.h>
#include <riz/coro/execution/execute.hpp>
#include <riz/hal/uart.h>
#include <riz/io/uart_service.h>
#include <riz/timer/timer_service.h>

#include "led_blink.h"
// #include "uart1_echo.h"
// #include "uart_hello.h"

#include <main.h>
#include <usb_device.h>
#include <usbd_hid.h>

extern UART_HandleTypeDef huart1;
extern USBD_HandleTypeDef hUsbDeviceHS;

namespace {
riz::coro::execution::scheduler g_sched;
std::byte g_uart1_ring_buf[128];
std::byte g_uart1_rx_dma_buf[128];

char g_line_buf[128];
const char* g_argv_buf[16];
std::byte g_console_write_buf[256];

riz::console::console* g_console = nullptr;
} // namespace

riz::coro::resumable::schedulable_task<void> cmd_echo(
    riz::coro::execution::scheduler& sched, int argc, const char* const* argv) {
    if (g_console && argc > 1) {
        for (int i = 1; i < argc; ++i) {
            g_console->print("%s%s", (i > 1 ? " " : ""), argv[i]);
        }
        g_console->print("\r\n");
    }
    TRACE("command ended");
    co_return;
}

riz::hal::uart g_uart1 {huart1, riz::hal::uart::transfer_mode::dma};
riz::io::uart_service g_uart1_svc {g_uart1, g_uart1_ring_buf, g_uart1_rx_dma_buf};

riz::console::command_entry g_commands[] = {
    {"echo", cmd_echo},
};

riz::console::console g_console_inst {
    g_uart1_svc, g_commands, g_line_buf, g_argv_buf, g_console_write_buf};

void app_main() {
    g_console = &g_console_inst;
    riz::console::logger::instance().init(*g_console);

    auto task_led_blink = riz::coro::execution::start(led_blink(g_sched));
    // auto task_uart_echo = riz::coro::execution::start(uart1_echo(g_sched, g_uart1_svc));
    // auto task_uart_hello = riz::coro::execution::start(uart_hello(g_sched, g_uart1_svc));
    auto task_console = riz::coro::execution::start(g_console_inst.run(g_sched));

    for (;;) {
        riz::timer::timer_service::instance().run();
        g_uart1_svc.run();
        g_sched.run();

        static uint32_t hid_last_tick = 0;
        uint32_t now = HAL_GetTick();
        if ((now - hid_last_tick) >= 50U) {
            hid_last_tick = now;
            uint8_t mouse_report[4] = {0x00U, 5U, 0U, 0U};
            (void)USBD_HID_SendReport(&hUsbDeviceHS, mouse_report, sizeof(mouse_report), HID_ID);
        }
    }
}
