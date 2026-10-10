# leaf_spy

A small header-only wrapper around [Boost.LEAF](https://www.boost.org/doc/libs/release/libs/leaf/doc/html/index.html).

`leaf_spy::try_catch` is `leaf::try_catch` plus one extra catcher, the **Spy**. The Spy is a handler by pointers: it sees **any** set of types in an exception, even if none of your handlers matched. It is the place to check an agreement (for example "every exception has a domain"), to log everything in one format, or to do something specific.

Documentation: <https://goloveshin.github.io/leaf_spy_doc/>

## Requirements

- C++20
- Boost.LEAF. If the target `Boost::leaf` does not exist, the library fetches `boostorg/leaf` itself.

## Use with CMake

```cmake
include(FetchContent)

FetchContent_Declare(
    leaf_spy
    GIT_REPOSITORY https://github.com/goloveshin/leaf_spy.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leaf_spy)

target_link_libraries(my_app PRIVATE leaf_spy::leaf_spy)
```

```cpp
#include <leaf_spy.hpp>
```

The unit tests and the example are built only when `leaf_spy` is the top-level project. To control it from a parent project, set the option `LEAF_SPY_BUILD_TESTS`.

## Build the tests

```text
cmake -S . -B build -G Ninja
cmake --build build
build/leaf_spy_tests
```

## Layout

| path | what it is |
| --- | --- |
| `include/` | the headers: `leaf_spy.hpp`, `captured_leaf_exception.hpp` |
| `tests/` | unit tests (doctest) |
| `example/` | a small project of three libraries that shows the library in use |

## License

[Boost Software License 1.0](LICENSE)
