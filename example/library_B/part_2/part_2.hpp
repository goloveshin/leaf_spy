#pragma once

// exceptions
#include "exception_declaration.hpp"


// calls the callback
//
// testing: no
//
inline void function_lib_B_part_2_execute_callback(std::function<void()> callback)
{
    // attach the domain and subdomain of the place that called the callback
    auto load = leaf::on_error(
        exception::domain::library_B{},
        exception::subdomain::library_B::part_2{}
    );

    callback();
}
