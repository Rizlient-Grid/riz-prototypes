#include "uart1_echo.h"

#include <riz/coro/awaiter/uart_receive_awaiter.h>
#include <riz/coro/awaiter/uart_transmit_awaiter.h>

auto uart1_echo(riz::coro::execution::scheduler& sched, riz::io::uart_service& svc) noexcept
    -> riz::coro::resumable::schedulable_task<void> {
    std::byte buf[1];
    for (;;) {
        auto rc = co_await svc.receive(buf, sizeof(buf));
        if (rc == riz::io::errcode::success) {
            co_await svc.transmit(buf, sizeof(buf));
        }
    }
}
