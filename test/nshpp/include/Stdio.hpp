#pragma once

#include <meta/FixedString.hpp>

namespace nsh {

struct StdIo
{
    static constexpr meta::basic_fixed_string newline = "\r\n";
    static constexpr meta::basic_fixed_string prompt = "> ";
    static constexpr meta::basic_fixed_string escape = "\x1b";
    static constexpr meta::basic_fixed_string csi = escape + "[";
    static constexpr meta::basic_fixed_string csi_erase_line = csi + "2K";
    static constexpr meta::basic_fixed_string csi_move_to_begin_of_line = "\r";

    static char get_char() noexcept;
    static void put_char(char c) noexcept;
    static void put_newline() noexcept;
    static void put_string(const char* str) noexcept;
    static void put_buffer(const char* str, unsigned int size) noexcept;
    static void put_prompt() noexcept;
    static void erase_last_char() noexcept;
    static void erase_chars(unsigned int count) noexcept;
    static void erase_line() noexcept;
};

} // namespace nsh
