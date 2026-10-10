// exceptions
#include "exception.hpp"

// libraries
#include "library_A.hpp"
#include "library_B.hpp"
#include "library_C.hpp"


int main()
{
    std::cout << "------ case 1: throw an exception from library_A / part_1\n";

    try_catch_with_spy([&]
    {
        function_lib_A_part_1();
    });

    std::cout << "------ case 2: the exception has no domain - we see a warning about the broken agreement\n";

    try_catch_with_spy([&]
    {
        function_lib_B_part_1_miss_domain();
    });

    std::cout << "------ case 3: an exception from one domain is augmented by another domain\n";

    try_catch_with_spy([&]
    {
        function_lib_B_part_2_execute_callback(function_lib_C_part_1_get_callback());
    });

    std::cout << "------ case 4: an inner catch through a direct handler succeeds\n";

    try_catch_with_spy([&]
    {
        function_lib_C_part_2_intermediate_catch_success_no_throw();
    });

    std::cout << "------ case 5: an inner catch through a direct handler fails, the exception comes here\n";

    try_catch_with_spy([&]
    {
        function_lib_C_part_3_intermediate_catch_failure();
    });

    std::cout << "------ case 6: an inner catch through a direct handler succeeds, but we choose to throw it further, so it comes here\n";

    try_catch_with_spy([&]
    {
        function_lib_C_part_4_intermediate_catch_success_and_throw();
    });

    return 0;
}
