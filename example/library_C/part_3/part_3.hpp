#pragma once

// exceptions
#include "exception_declaration.hpp"


// throws a leaf exception library_C / part_3
//
// testing: no
//
inline void function_lib_C_part_3_intermediate_catch_failure()
{
    // intermediate catch through a direct handler fails, the exception goes further
    try_catch_with_spy([&]
        {
            leaf::throw_exception(
                exception::domain::library_C{},
                exception::subdomain::library_C::part_3{},
                exception::e_what_info{ "library_C, part_3" }
            );
        },
        true,
        // bad part!
        [](const exception::domain::library_C&, const exception::subdomain::library_C::part_2&)
        {
            std::cout << "direct handler: library_C, part_3\n";
        }
    );
}
