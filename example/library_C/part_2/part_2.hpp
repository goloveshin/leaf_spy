#pragma once

// exceptions
#include "exception_declaration.hpp"


// throws a leaf exception library_C / part_2
//
// testing: no
//
inline void function_lib_C_part_2_intermediate_catch_success_no_throw()
{
    // intermediate catch through a direct handler succeeds, the exception does not go further
    try_catch_with_spy(
        [&]
        {
            leaf::throw_exception(
                exception::domain::library_C{},
                exception::subdomain::library_C::part_2{},
                exception::e_what_info{ "library_C, part_2" }
            );
        },
        true,
        [](const exception::domain::library_C&, const exception::subdomain::library_C::part_2&)
        {
            std::cout << "direct handler: library_C, part_2\n";
        }
    );
}
