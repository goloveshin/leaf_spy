#pragma once

// leaf_spy
#include "captured_leaf_exception.hpp"

// Boost.LEAF
#include <boost/leaf/context.hpp>
#include <boost/leaf/diagnostics.hpp>
#include <boost/leaf/result.hpp>

// std
#include <exception>
#include <functional>
#include <type_traits>


namespace leaf_spy
{

namespace leaf = boost::leaf;

struct leaf_spy_args_t
{
    using job_that_may_throw_ft = std::function<void ()>;
    using notify_that_job_executed_with_throw_ft = std::function<void ()>;
    using notify_that_job_executed_without_throw_ft = std::function<void ()>;

    // run the job that may throw
    const job_that_may_throw_ft& job_that_may_throw;

    // report that the job finished with an exception
    const notify_that_job_executed_with_throw_ft& notify_that_job_executed_with_throw;

    // report that the job finished without an exception
    const notify_that_job_executed_without_throw_ft& notify_that_job_executed_without_throw;

    // throw the exception further
    const bool is_throw_out = true;
};

// every operator() argument is a pointer
//
template <typename List>
struct args_are_pointers;

template <template <typename ...> class List, typename ... Arg>
struct args_are_pointers<List<Arg...>>
    : std::bool_constant<(std::is_pointer_v<Arg> && ...)>
{
};

// SpyHandler::visit walks the slots, operator() names which slots to read.
//
template <typename SpyHandler, typename Context>
concept SpyHandlerConcept =
    args_are_pointers<leaf::detail::fn_mp_args<SpyHandler>>::value
    && requires(
        SpyHandler& spy_handler,
        Context& ctx,
        const leaf::error_id id,
        std::exception* exception
    )
{
    spy_handler.visit(ctx, id, exception);
};

// First try to catch the exception from args.job_that_may_throw() with direct_handlers (like leaf::try_catch())
// - if none matched, inspect the exception through spy_handler and throw it further
// - if one matched and did throw;, throw the original leaf exception further
// - if one matched and threw a new exception, throw that new exception further
// - if one matched and did not throw, do not throw further
// - an exception leaves the function only when args.is_throw_out is set
//
// testing: yes (unit)
//
template <typename SpyHandler, typename ... DirectHandlers>
void try_catch(
    const leaf_spy_args_t& args,
    SpyHandler&& spy_handler,
    DirectHandlers&& ... direct_handlers // handlers like leaf::try_catch()
)
{
    // slots from direct_handlers and from the pointers in spy_handler::operator()
    auto ctx = leaf::make_context(direct_handlers..., spy_handler);

    // visit is present, operator() arguments are pointers
    static_assert(SpyHandlerConcept<SpyHandler, decltype(ctx)>);

    // exceptions from job_that_may_throw are loaded into this context
    ctx.activate();

    // deactivate the context on any exit if dispatch has not deactivated it yet
    //
    struct deactivate_on_exit_t
    {
        decltype(ctx)& context;

        ~deactivate_on_exit_t()
        {
            if (context.is_active())
            {
                context.deactivate();
            }
        }
    };
    deactivate_on_exit_t deactivate_on_exit{ctx};

    auto dispatch = [&](const leaf::error_id id, std::exception* exception)
    {
        args.notify_that_job_executed_with_throw();

        // handle_error works on an inactive context
        if (ctx.is_active())
        {
            ctx.deactivate();
        }

        // by default assume a direct_handler handled it
        bool is_need_spy_handler = false;
        bool is_need_rethrow = false;
        // throw; in a handler is this same exception
        const std::exception_ptr original = std::current_exception();
        std::exception_ptr thrown_instead;
        {
            try
            {
                ctx.template handle_error<void>(
                    id,
                    std::forward<DirectHandlers>(direct_handlers)...,
                    [&]
                    {
                        // no direct_handler matched
                        is_need_spy_handler = true;
                        is_need_rethrow = true;
                    }
                );
            }
            catch (std::exception& thrown)
            {
                // throw; is the same object, throw expr is a new exception
                is_need_spy_handler = false;
                if (exception && &thrown == exception)
                {
                    is_need_rethrow = true;
                }
                else
                {
                    thrown_instead = std::current_exception();
                }
            }
            catch (...)
            {
                // not a std::exception: throw; is the original, anything else is new
                is_need_spy_handler = false;
                const std::exception_ptr thrown = std::current_exception();
                if (original && thrown == original)
                {
                    is_need_rethrow = true;
                }
                else
                {
                    thrown_instead = thrown;
                }
            }
        }

        // process the slots
        if (is_need_spy_handler)
        {
            spy_handler.visit(ctx, id, exception);
        }

        if (thrown_instead && args.is_throw_out)
        {
            std::rethrow_exception(thrown_instead);
        }

        if (is_need_rethrow && args.is_throw_out)
        {
            // unload moves the slots outward, throw keeps the same exception
            if (id.value() != 0)
            {
                ctx.unload(id);
            }
            throw;
        }
    };

    try
    {
        try
        {
            args.job_that_may_throw();
            args.notify_that_job_executed_without_throw();
        }
        catch (captured_leaf_exception& captured)
        {
            // unload into the still-active context, then the original leaf exception
            captured.result->value();
        }
    }
    catch (std::exception& exception)
    {
        dispatch(leaf::detail::unpack_error_id(exception), &exception);
    }
    catch (...)
    {
        // no std::exception: the id comes from the thread, there is no exception pointer
        dispatch(leaf::current_error(), nullptr);
    }
}

}// namespace leaf_spy
