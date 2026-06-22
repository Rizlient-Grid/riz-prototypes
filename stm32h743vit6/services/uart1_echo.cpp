#include "uart1_echo.h"

#include <riz/coro/awaiter/uart_receive_awaiter.h>
#include <riz/coro/awaiter/uart_transmit_awaiter.h>

auto uart1_echo(riz::coro::execution::scheduler& sched, riz::io::uart_service& svc) noexcept
    -> riz::coro::resumable::schedulable_task<void> {
    std::byte buf[1];
    bool idle = true;
    constexpr char kIdleMsg[] = "idle\r\n";

    co_await svc.transmit(kIdleMsg, sizeof(kIdleMsg) - 1);

    for (;;) {
        auto rc = co_await svc.receive(buf, sizeof(buf), 1000);
        if (rc == riz::errcode::success) {
            idle = false;
            co_await svc.transmit(buf, sizeof(buf));
        } else if (!idle) {
            idle = true;
            co_await svc.transmit(kIdleMsg, sizeof(kIdleMsg) - 1);
        }
    }
}
