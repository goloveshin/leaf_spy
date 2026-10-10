#pragma once

// Boost.LEAF
#include <boost/leaf/exception.hpp>
#include <boost/leaf/on_error.hpp>

// std
#include <filesystem>
#include <string>


namespace leaf = boost::leaf;


namespace exception
{

// for us these are libraries
//
namespace domain
{

struct library_A{};

struct library_B{};

struct library_C{};

}// namespace domain

// for us these are the significant parts of the libraries
//
namespace subdomain
{

namespace library_A
{

struct part_1{};

}// namespace library_A

namespace library_B
{

struct part_1{};

struct part_2{};

}// namespace library_B

namespace library_C
{

struct part_1{};

struct part_2{};

struct part_3{};

struct part_4{};

}// namespace library_C

}// namespace subdomain

// refining details, anything!
//
namespace details
{

struct special_data_t
{
};

}// namespace details

// some general information

struct e_what_info
{
    std::string what;
};

// agreement: an exception must contain a domain and a subdomain

}// namespace exception
