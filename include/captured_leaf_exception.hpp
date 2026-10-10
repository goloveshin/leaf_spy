#pragma once

// Boost.LEAF
#include <boost/leaf/result.hpp>

// std
#include <exception>
#include <memory>


namespace leaf_spy
{

namespace leaf = boost::leaf;

// for work on several threads: leaf slots stay on the thread that threw the exception
// result holds their copy until leaf_spy unloads it into the context of the thread that caught the exception
//
// testing: no
//
struct captured_leaf_exception : std::exception
{
    std::shared_ptr<leaf::result<void>> result;

    explicit captured_leaf_exception(leaf::result<void> result)
        : result(std::make_shared<leaf::result<void>>(std::move(result)))
    {
    }

    const char* what() const noexcept override
    {
        return "captured leaf exception";
    }
};

}// namespace leaf_spy
