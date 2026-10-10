// doctest
#include <doctest/doctest.h>

// leaf_spy
#include "leaf_spy.hpp"

// std
#include <exception>
#include <stdexcept>
#include <string>


namespace leaf_spy
{

namespace
{

struct mark_a
{
    int value = 0;
};

struct mark_b
{
    int value = 0;
};

struct bottom_skip_t
{
};

struct seen_marks_t
{
    bool saw_a = false;
    bool saw_b = false;
    int a = 0;
    int b = 0;
};

struct bind_t
{
    seen_marks_t* marks = nullptr;

    void operator()(const mark_a*, const mark_b*) const
    {
    }

    // types that the context holds at this level
    //
    template <typename Context>
    void visit(
        Context& ctx,
        const leaf::error_id id,
        std::exception*
    ) const
    {
        if (const auto* a = ctx.template get<mark_a>(id))
        {
            marks->saw_a = true;
            marks->a = a->value;
        }
        if (const auto* b = ctx.template get<mark_b>(id))
        {
            marks->saw_b = true;
            marks->b = b->value;
        }
    }
};

const leaf_spy_args_t::notify_that_job_executed_with_throw_ft on_throw = []{};
const leaf_spy_args_t::notify_that_job_executed_without_throw_ft on_ok = []{};

constexpr int value_a = 11;
constexpr int value_b = 22;

// a leaf exception with mark_a and mark_b
//
void throw_marks()
{
    leaf::throw_exception(
        std::runtime_error("job"),
        mark_a{value_a},
        mark_b{value_b}
    );
}

// arguments of one try_catch call
//
leaf_spy_args_t make_args(
    const leaf_spy_args_t::job_that_may_throw_ft& job,
    const bool is_throw_out
)
{
    return leaf_spy_args_t{
        .job_that_may_throw = job,
        .notify_that_job_executed_with_throw = on_throw,
        .notify_that_job_executed_without_throw = on_ok,
        .is_throw_out = is_throw_out
    };
}

}// namespace

TEST_SUITE("leaf_spy")
{

// the lower try_catch sees the types and throws them further,
// the upper one catches the same types
//
TEST_CASE("upper catches types rethrown by bottom")
{
    seen_marks_t bottom_marks;
    seen_marks_t upper_marks;
    bool upper_caught = false;

    const leaf_spy_args_t::job_that_may_throw_ft bottom_job = []
    {
        throw_marks();
    };

    const leaf_spy_args_t::job_that_may_throw_ft upper_job = [&]
    {
        try_catch(
            make_args(bottom_job, true),
            bind_t{&bottom_marks},
            [](const bottom_skip_t&){}
        );
    };

    try_catch(
        make_args(upper_job, true),
        bind_t{&upper_marks},
        [&](const mark_a& a, const mark_b& b)
        {
            upper_caught = true;
            upper_marks.saw_a = true;
            upper_marks.saw_b = true;
            upper_marks.a = a.value;
            upper_marks.b = b.value;
        }
    );

    CHECK(bottom_marks.saw_a);
    CHECK(bottom_marks.saw_b);
    CHECK(upper_caught);
    CHECK(upper_marks.saw_a);
    CHECK(upper_marks.saw_b);
    CHECK(upper_marks.a == bottom_marks.a);
    CHECK(upper_marks.b == bottom_marks.b);
    CHECK(upper_marks.a == value_a);
    CHECK(upper_marks.b == value_b);
}

// no direct_handler matched, is_throw_out true: the spy sees the slots, the exception leaves
//
TEST_CASE("is_throw_out true rethrows when no direct handler matches")
{
    seen_marks_t marks;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    CHECK_THROWS((try_catch(
        make_args(job, true),
        bind_t{&marks},
        [](const bottom_skip_t&){}
    )));

    CHECK(marks.saw_a);
    CHECK(marks.saw_b);
    CHECK(marks.a == value_a);
    CHECK(marks.b == value_b);
}

// no direct_handler matched, is_throw_out false: the spy sees the slots, the exception stays inside
//
TEST_CASE("is_throw_out false swallows when no direct handler matches")
{
    seen_marks_t marks;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    CHECK_NOTHROW((try_catch(
        make_args(job, false),
        bind_t{&marks},
        [](const bottom_skip_t&){}
    )));

    CHECK(marks.saw_a);
    CHECK(marks.saw_b);
    CHECK(marks.a == value_a);
    CHECK(marks.b == value_b);
}

// a direct_handler caught and did not throw: the spy is not called, the exception does not leave
//
TEST_CASE("direct handler that does not throw swallows")
{
    seen_marks_t marks;
    bool handled = false;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    CHECK_NOTHROW((try_catch(
        make_args(job, true),
        bind_t{&marks},
        [&](const mark_a& a, const mark_b& b)
        {
            handled = true;
            CHECK(a.value == value_a);
            CHECK(b.value == value_b);
        }
    )));

    CHECK(handled);
    CHECK_FALSE(marks.saw_a);
    CHECK_FALSE(marks.saw_b);
}

// a direct_handler did throw; and is_throw_out true: the spy is not called, the original leaf exception leaves
//
TEST_CASE("direct handler throw; rethrows the original leaf exception when is_throw_out is true")
{
    seen_marks_t marks;
    seen_marks_t upper_marks;
    bool handled = false;
    bool upper_caught = false;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    const leaf_spy_args_t::job_that_may_throw_ft upper_job = [&]
    {
        try_catch(
            make_args(job, true),
            bind_t{&marks},
            [&](const mark_a&, const mark_b&)
            {
                handled = true;
                throw;
            }
        );
    };

    try_catch(
        make_args(upper_job, true),
        bind_t{&upper_marks},
        [&](const mark_a& a, const mark_b& b)
        {
            upper_caught = true;
            upper_marks.saw_a = true;
            upper_marks.saw_b = true;
            upper_marks.a = a.value;
            upper_marks.b = b.value;
        }
    );

    CHECK(handled);
    CHECK(upper_caught);
    CHECK_FALSE(marks.saw_a);
    CHECK_FALSE(marks.saw_b);
    CHECK(upper_marks.saw_a);
    CHECK(upper_marks.saw_b);
    CHECK(upper_marks.a == value_a);
    CHECK(upper_marks.b == value_b);
}

// a direct_handler threw std::runtime_error, is_throw_out true: the spy is not called, that exception leaves
//
TEST_CASE("direct handler that throws std::runtime_error throws it out when is_throw_out is true")
{
    seen_marks_t marks;
    bool handled = false;
    bool left = false;
    std::string what;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    try
    {
        try_catch(
            make_args(job, true),
            bind_t{&marks},
            [&](const mark_a&, const mark_b&)
            {
                handled = true;
                throw std::runtime_error("handler");
            }
        );
    }
    catch (const std::exception& ex)
    {
        left = true;
        what = ex.what();
    }

    CHECK(handled);
    CHECK(left);
    CHECK(what == "handler");
    CHECK_FALSE(marks.saw_a);
    CHECK_FALSE(marks.saw_b);
}

// a direct_handler caught and threw, is_throw_out false: the spy is not called, the exception does not leave
//
TEST_CASE("direct handler that throws stays inside when is_throw_out is false")
{
    seen_marks_t marks;
    bool handled = false;

    const leaf_spy_args_t::job_that_may_throw_ft job = []
    {
        throw_marks();
    };

    CHECK_NOTHROW((try_catch(
        make_args(job, false),
        bind_t{&marks},
        [&](const mark_a&, const mark_b&)
        {
            handled = true;
            throw std::runtime_error("handler");
        }
    )));

    CHECK(handled);
    CHECK_FALSE(marks.saw_a);
    CHECK_FALSE(marks.saw_b);
}

}

}// namespace leaf_spy
