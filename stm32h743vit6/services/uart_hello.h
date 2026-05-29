#pragma once

#include <riz/coro/execution/scheduler.h>
#include <riz/coro/resumable/schedulable_task.hpp>
#include <riz/io/uart_service.h>

auto uart_hello(riz::coro::execution::scheduler& sched, riz::io::uart_service& svc) noexcept
    -> riz::coro::resumable::schedulable_task<void>;
