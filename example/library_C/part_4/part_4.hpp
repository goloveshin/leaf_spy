#pragma once

// exceptions
#include "exception_declaration.hpp"


// throws a leaf exception library_C / part_4
//
// testing: no
//
inline void function_lib_C_part_4_intermediate_catch_success_and_throw()
{
    // intermediate catch through a direct handler succeeds, but we choose to throw it further
    try_catch_with_spy([&]
        {
            leaf::throw_exception(
                exception::domain::library_C{},
                exception::subdomain::library_C::part_4{},
                exception::e_what_info{ "library_C, part_4" },
                exception::details::special_data_t{}
            );
        },
        true,
        [](const exception::details::special_data_t&)
        {
            std::cout << "direct handler: library_C, part_4: catch special data and rethrow\n";
            throw;
        }
    );
}
