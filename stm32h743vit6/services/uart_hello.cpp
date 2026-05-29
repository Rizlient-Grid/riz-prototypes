#include "uart_hello.h"

#include <riz/coro/awaiter/uart_transmit_awaiter.h>
#include <riz/coro/sleep.h>

auto uart_hello(riz::coro::execution::scheduler& sched, riz::io::uart_service& svc) noexcept
    -> riz::coro::resumable::schedulable_task<void> {
    static constexpr char msg[] = "hello world\r\n";
    for (;;) {
        co_await svc.transmit(msg, sizeof(msg) - 1);
        co_await riz::coro::sleep(std::chrono::milliseconds(100));
    }
}
