#include <Stdio.hpp>

#include <cstdio>
#include <cstring>

namespace nsh {

char StdIo::get_char() noexcept
{
    return static_cast<char>(::getchar());
}

void StdIo::put_char(char c) noexcept
{
    ::putchar(c);
}

void StdIo::put_newline() noexcept
{
    put_buffer(newline.data(), newline.size());
}

void StdIo::put_string(const char* str) noexcept
{
    // TODO This implementation is suboptimal as two loops are executed:
    // one by strlen, one by nsh_io_put_buffer...
    // Migrating from null-terminated strings to mcsl's string_view could
    // solve this issue.
    put_buffer(str, static_cast<unsigned int>(::strlen(str)));
}

void StdIo::put_buffer(const char* str, unsigned int size) noexcept
{
    for (unsigned int i = 0; i < size; i++) {
        put_char(str[i]);
    }
}

void StdIo::put_prompt() noexcept
{
    put_buffer(prompt.data(), prompt.size());
}

void StdIo::erase_last_char() noexcept
{
    put_char('\b'); // go back to one character
    put_char(' '); // overwrite the char with whitespace
    put_char('\b'); // go back to the now removed char position
}

void StdIo::erase_chars(unsigned int count) noexcept
{
    for (unsigned int i = 0; i < count; i++) {
        erase_last_char();
    }
}

void StdIo::erase_line() noexcept
{
    put_buffer(csi_erase_line.data(), csi_erase_line.size());
    put_buffer(csi_move_to_begin_of_line.data(), csi_move_to_begin_of_line.size());
}

} // namespace nsh
