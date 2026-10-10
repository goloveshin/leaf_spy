#pragma once

// exceptions
#include "exception_declaration.hpp"

// Boost.LEAF
#include <boost/leaf/context.hpp>
#include <boost/leaf/diagnostics.hpp>

// std
#include <sstream>


namespace print_exception_part
{

struct visit_t
{
    std::ostringstream& ss;
};

// prints a domain or subdomain mark
//
// testing: no
//
template <typename Mark>
void visit_mark(const visit_t& args, const Mark* ptr, const char* part_name)
{
    if (ptr)
    {
        args.ss << part_name << std::endl;
    }
}

// testing: no
//
template <typename Pointer, typename VisitFunc>
void visit_helper(
    const visit_t& args,
    const Pointer* ptr,
    const std::string& part_name,
    const VisitFunc& visit_func
)
{
    if (ptr)
    {
        args.ss << part_name << ":\n";
        visit_func(*ptr);
    }
}

// testing: no
//
inline void visit(const visit_t& args, const exception::e_what_info* ptr)
{
    visit_helper(args, ptr, "what info", [&](const auto& ref)
    {
        args.ss << "message: " << ref.what << '\n';
    });
}

// testing: no
//
inline void visit(const visit_t& args, const leaf::diagnostic_info& info)
{
    visit_helper(args, &info, "Boost.LEAF diagnostic info", [&](const auto& ref)
    {
        args.ss << ref << '\n';
    });
}

// testing: no
//
inline void visit(const visit_t& args, const std::exception* ptr)
{
    if (!ptr)
    {
        return;
    }

    args.ss << "type: std::exception:\n";
    args.ss << "message: " << ptr->what() << '\n';
}

}// namespace print_exception_part

// names the leaf slot types and prints them
//
// testing: no
//
struct spy_handler_t
{
    std::ostringstream& ss;

    void operator()(
        const exception::domain::library_A*,
        const exception::domain::library_B*,
        const exception::domain::library_C*,
        const exception::subdomain::library_A::part_1*,
        const exception::subdomain::library_B::part_1*,
        const exception::subdomain::library_B::part_2*,
        const exception::subdomain::library_C::part_1*,
        const exception::subdomain::library_C::part_2*,
        const exception::subdomain::library_C::part_3*,
        const exception::subdomain::library_C::part_4*,
        const exception::e_what_info*
    ) const
    {
    }

    // print our exceptions
    //
    template <typename Context>
    void visit(
        Context& ctx,
        const leaf::error_id id,
        std::exception* exception
    ) const
    {
        namespace pep = print_exception_part;

        auto visit_args = pep::visit_t{ss};

        ss << "Exception details:\n";

        // print std::exception first
        pep::visit(visit_args, exception);

        // domains
        const auto* domain_library_a = ctx.template get<exception::domain::library_A>(id);
        const auto* domain_library_b = ctx.template get<exception::domain::library_B>(id);
        const auto* domain_library_c = ctx.template get<exception::domain::library_C>(id);

        pep::visit_mark(visit_args, domain_library_a, "domain: library_A");
        pep::visit_mark(visit_args, domain_library_b, "domain: library_B");
        pep::visit_mark(visit_args, domain_library_c, "domain: library_C");

        // no domain
        if (!domain_library_a
            && !domain_library_b
            && !domain_library_c
        )
        {
            ss << "-----> ERROR: no domain in exception!\n";
        }

        // subdomains
        const auto* subdomain_library_a_part_1 = ctx.template get<exception::subdomain::library_A::part_1>(id);
        const auto* subdomain_library_b_part_1 = ctx.template get<exception::subdomain::library_B::part_1>(id);
        const auto* subdomain_library_b_part_2 = ctx.template get<exception::subdomain::library_B::part_2>(id);
        const auto* subdomain_library_c_part_1 = ctx.template get<exception::subdomain::library_C::part_1>(id);
        const auto* subdomain_library_c_part_2 = ctx.template get<exception::subdomain::library_C::part_2>(id);
        const auto* subdomain_library_c_part_3 = ctx.template get<exception::subdomain::library_C::part_3>(id);
        const auto* subdomain_library_c_part_4 = ctx.template get<exception::subdomain::library_C::part_4>(id);

        pep::visit_mark(visit_args, subdomain_library_a_part_1, "subdomain: library_A::part_1");
        pep::visit_mark(visit_args, subdomain_library_b_part_1, "subdomain: library_B::part_1");
        pep::visit_mark(visit_args, subdomain_library_b_part_2, "subdomain: library_B::part_2");
        pep::visit_mark(visit_args, subdomain_library_c_part_1, "subdomain: library_C::part_1");
        pep::visit_mark(visit_args, subdomain_library_c_part_2, "subdomain: library_C::part_2");
        pep::visit_mark(visit_args, subdomain_library_c_part_3, "subdomain: library_C::part_3");
        pep::visit_mark(visit_args, subdomain_library_c_part_4, "subdomain: library_C::part_4");

        // no subdomain
        if (
            !subdomain_library_a_part_1
            && !subdomain_library_b_part_1
            && !subdomain_library_b_part_2
            && !subdomain_library_c_part_1
            && !subdomain_library_c_part_2
            && !subdomain_library_c_part_3
            && !subdomain_library_c_part_4
        )
        {
            ss << "-----> ERROR: no subdomain in exception!\n";
        }

        pep::visit(visit_args, ctx.template get<exception::e_what_info>(id));

        // native leaf::diagnostic_info
        pep::visit(
            visit_args,
            leaf::detail::diagnostic_info_(
                leaf::error_info(
                    id,
                    exception,
                    ctx.template get<leaf::e_source_location>(id)
                ),
                ctx
            )
        );
    }
};
