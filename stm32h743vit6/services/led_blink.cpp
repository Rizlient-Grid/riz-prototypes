#include "led_blink.h"

#include <riz/coro/sleep.h>

#include <main.h>

auto led_blink(riz::coro::execution::scheduler& sched) noexcept
    -> riz::coro::resumable::schedulable_task<void> {
    for (;;) {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        co_await riz::coro::sleep(std::chrono::milliseconds(1000));
    }
}
