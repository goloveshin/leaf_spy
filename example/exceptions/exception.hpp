#pragma once

// leaf_spy
#include "leaf_spy.hpp"
#include "exception_spy_handler.hpp"

// exceptions
#include "scope_guard.hpp"

// std
#include <functional>
#include <iostream>
#include <sstream>


// runs the job inside try_catch and prints the spy_handler_t text
//
// testing: no
//
template <typename ... DirectHandlers>
inline void try_catch_with_spy(
    const std::function<void()>& job,
    const bool is_throw_out = false,
    DirectHandlers&& ... direct_handlers
)
{
    std::ostringstream ss;

    auto print_on_exit = scope_guard_t(
        []{},
        [&]{
            if (const auto info = ss.str(); !info.empty())
            {
                std::cout << info;
            }
        }
    );

    ::leaf_spy::try_catch(
        {
            .job_that_may_throw = job,
            .notify_that_job_executed_with_throw = []{},
            .notify_that_job_executed_without_throw = []{},
            .is_throw_out = is_throw_out
        },
        spy_handler_t{ss},
        std::forward<DirectHandlers>(direct_handlers)...
    );
}
