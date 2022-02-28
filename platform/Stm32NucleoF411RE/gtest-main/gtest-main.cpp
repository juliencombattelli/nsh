#include "gtest/gtest.h"

#include <array>
#include <cstdio>

template<typename... Args>
static auto to_argv(Args... str)
{
    static_assert(
        std::conjunction_v<std::is_convertible<Args, char*>...>,
        "Args must be convertible to non-const char*");
    std::array argv { static_cast<char*>(str)..., static_cast<char*>(nullptr) };
    return argv;
}

namespace nsh::tools {

int main(int /*argc*/, char* /*argv*/[])
{
    char arg0[] = "gtest_main_on_nucleo";
    char arg1[] = "--gtest_color=yes";
    auto argv = to_argv(arg0, arg1);
    int argc = argv.size() - 1;

    puts("\r\n\r\n");
    testing::InitGoogleTest(&argc, argv.data());
    return RUN_ALL_TESTS();
}

} // namespace nsh::tools
