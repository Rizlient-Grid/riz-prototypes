#pragma once

#include <riz/coro/execution/scheduler.h>
#include <riz/coro/resumable/schedulable_task.hpp>

auto led_blink(riz::coro::execution::scheduler& sched) noexcept
	-> riz::coro::resumable::schedulable_task<void>;
