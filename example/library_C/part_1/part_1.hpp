#pragma once

// exceptions
#include "exception_declaration.hpp"

// std
#include <iostream>
#include <functional>


// testing: no
//
inline std::function<void()> function_lib_C_part_1_get_callback()
{
    return []{
        std::cout << "callback from library_C, part_1\n";

        leaf::throw_exception(
            // this is the domain and subdomain where the callback code is written
            exception::domain::library_C{},
            exception::subdomain::library_C::part_1{},
            exception::e_what_info{ "callback from library_C, part_1" }
        );
    };
}
