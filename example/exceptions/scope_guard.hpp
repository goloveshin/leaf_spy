#pragma once

// std
#include <functional>


// calls on_construct_func on construction and on_destruct_func when leaving the scope
//
// testing: no
//
class scope_guard_t
{
public:
    scope_guard_t(
        const std::function<void()>& on_construct_func,
        const std::function<void()>& on_destruct_func
    )
        : on_destruct_func_(on_destruct_func)
    {
        if (on_construct_func)
        {
            on_construct_func();
        }
    }
    ~scope_guard_t()
    {
        if (on_destruct_func_)
        {
            on_destruct_func_();
        }
    }
    scope_guard_t(const scope_guard_t&) = delete;
    scope_guard_t& operator=(const scope_guard_t&) = delete;
    scope_guard_t(scope_guard_t&&) = delete;
    scope_guard_t& operator=(scope_guard_t&&) = delete;

private:
    std::function<void()> on_destruct_func_;
};
