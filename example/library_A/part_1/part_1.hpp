#pragma once

// exceptions
#include "exception_declaration.hpp"

// std
#include <functional>


// throws a leaf exception library_A / part_1
//
// testing: no
//
inline void function_lib_A_part_1()
{
    leaf::throw_exception(
        exception::domain::library_A{},
        exception::subdomain::library_A::part_1{},
        exception::e_what_info{ "library_A, part_1" }
    );
}
