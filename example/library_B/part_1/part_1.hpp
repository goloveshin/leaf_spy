#pragma once

// exceptions
#include "exception_declaration.hpp"


// throws a leaf exception library_B / part_1
//
// testing: no
//
inline void function_lib_B_part_1_miss_domain()
{
    leaf::throw_exception(
        // miss domain
        exception::subdomain::library_B::part_1{},
        exception::e_what_info{ "library_B, part_1" }
    );
}
