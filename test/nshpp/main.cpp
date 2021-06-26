#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <span>
#include <string_view>

namespace nsh {

/**
 * @brief Check if all arguments in parameter pack U are same as T.
 */
template <typename T, typename... U>
inline constexpr bool AllSame = std::conjunction_v<std::is_same<T, U>...>;

/**
 * @brief Shell command class associating a name and a command to execute.
 * @tparam THandler Type of the callable to execute when the command is invoked.
 *
 * THandler can be any callable type like pointer to function, functor, std::function.
 */
template <typename THandler>
struct Cmd {
    using HandlerType = THandler;
    constexpr Cmd(const char* str, THandler handler)
        : name(str)
        , handler(handler)
    {
    }

    std::string_view name;
    THandler handler;
};

/**
 * @brief List of commands that a nsh shell can use.
 * @tparam TCmd Type of the commands of the list.
 * @tparam N Commands count in the list.
 *
 * This class behaves like a std::array of N TCmds.
 * It cannot be empty by design: it always contains at least one element.
 */
template <typename TCmd, std::size_t N>
    requires(N > 0)
class CmdList {
public:
    using CmdType = TCmd;

    constexpr CmdList() = default;

    template <typename TCmdFirst, typename... TCmds>
        requires(AllSame<TCmdFirst, TCmds...>)
    constexpr explicit CmdList(TCmdFirst cmdfirst, TCmds... cmds) noexcept
        : list { cmdfirst, cmds... }
    {
        std::ranges::sort(list, {}, &TCmdFirst::name);
    }

    constexpr auto begin() noexcept
    {
        return list.begin();
    }
    constexpr auto begin() const noexcept
    {
        return list.begin();
    }
    constexpr auto end() noexcept
    {
        return list.end();
    }
    constexpr auto end() const noexcept
    {
        return list.end();
    }
    constexpr auto size() const noexcept
    {
        return list.size();
    }
    constexpr auto operator[](std::size_t pos)
    {
        return list[pos];
    }
    constexpr auto operator[](std::size_t pos) const
    {
        return list[pos];
    }

private:
    std::array<TCmd, N> list;
};

/**
 * @brief Deduction guide for CmdList template class.
 *
 * Allow the construction of the list without explicitly specifying its size.
 * The size template argument is deduced from the size of TCmds parameter pack.
 */
template <typename TCmdFirst, typename... TCmds>
explicit CmdList(TCmdFirst, TCmds...) -> CmdList<TCmdFirst, 1 + sizeof...(TCmds)>;

/**
 * @brief Concept specifying that a type can be used as a command autocompleter for nsh.
 */
template <typename T, typename TCmds>
concept Autocompleter = requires(T t, TCmds cmds, std::string_view str)
{
    // add default constructability
    { t.autocomplete(cmds, str) } /*-> std::same_as<std::span<typename TCmds::CmdType>>*/;
};

/**
 * @brief Concept specifying that a type can be used as a command suggester for nsh.
 */
template <typename T, typename TCmds>
concept Suggester = requires(T t, TCmds cmds, std::string_view str)
{
    // add default constructability
    { t.suggest(cmds, str) } /*-> std::same_as<std::span<typename TCmds::CmdType>>*/;
};

template <typename THandler>
struct Settings {
    using Handler = THandler;
    std::size_t LineBufferSize = 128;
    std::size_t MaxArgumentCount = 32;
    std::size_t CmdNameSize = 16;
    std::size_t CmdCount = 32;
    std::size_t HistorySize = 16;
};

template <typename TCmds,
    Autocompleter<TCmds> TAutocompleter,
    Suggester<TCmds> TSuggester>
class Nsh {
public:
    TAutocompleter autocompleter;
    TSuggester suggester;
};

} // namespace nsh

namespace my_shell {

struct my_context {
    std::size_t some_data;
};

using my_handler = int (*)(my_context&, int, char**);
using my_cmd = nsh::Cmd<my_handler>;

constexpr nsh::Settings<my_handler> my_settings {
    .LineBufferSize = 16,
    .MaxArgumentCount = 16,
    .CmdNameSize = 16,
    .CmdCount = 16,
    .HistorySize = 16,
};

constexpr my_cmd help("help",
    [](my_context&, int, char**) noexcept {
        puts("This is an helpful message!");
        return 42;
    });

constexpr my_cmd exit("exit",
    [](my_context&, int, char**) noexcept {
        puts("Exiting!");
        return 69;
    });

constexpr my_cmd toto("toto",
    [](my_context&, int, char**) noexcept {
        puts("Toto!");
        return 99;
    });

constexpr my_cmd tata("tata",
    [](my_context&, int, char**) noexcept {
        puts("Tata!");
        return 99;
    });

constexpr nsh::CmdList cmds(help, exit, toto, tata);

struct DefaultAutocompleter {
    template <typename TCmdList>
    static constexpr auto autocomplete(TCmdList&& cmds, std::string_view str) noexcept
    {
        // Since list is already sorted lexicographically, all elements starting with str
        // are contiguous. So we search for the first one that matches, the last one that
        // does not match, and return a span from the first element to the last one.
        const auto starts_with_str = [&](auto cmd) { return cmd.name.starts_with(str); };
        const auto first = std::find_if(cmds.begin(), cmds.end(), starts_with_str);
        const auto last = std::find_if_not(first, cmds.end(), starts_with_str);
        return std::span(first, last);
    }
};

template <std::size_t N>
constexpr std::size_t levenshteinDistance(std::string_view string_a, std::string_view string_b) noexcept
{
    const auto size_a = string_a.size();
    const auto size_b = string_b.size();

    if (size_a > N || size_b > N) {
        return std::numeric_limits<std::size_t>::max();
    }

    std::array<std::size_t, N> distances;
    std::iota(distances.begin(), distances.end(), std::size_t { 0 });

    for (std::size_t i = 0; i < size_a; ++i) {
        std::size_t previous_distance = 0;
        for (std::size_t j = 0; j < size_b; ++j) {
            distances[j + 1] = std::min({ std::exchange(previous_distance, distances[j + 1]) + (string_a[i] == string_b[j] ? 0 : 1),
                distances[j] + 1,
                distances[j + 1] + 1 });
        }
    }
    return distances[size_b];
}

struct DefaultSuggester {
    template <typename TCmdList>
    static constexpr auto suggest(TCmdList&& cmds, std::string_view str) noexcept
    {
        std::size_t minimal_distance = std::numeric_limits<std::size_t>::max();
        std::size_t matching_command = 0;
        for (std::size_t i = 0; i < cmds.size(); ++i) {
            // TODO string size (16) should not be hardcoded here
            const auto distance = levenshteinDistance<16>(str, cmds[i].name);
            if (distance < minimal_distance) {
                minimal_distance = distance;
                matching_command = i;
            }
        }
        return cmds[matching_command].name;
    }
};

} // namespace my_shell

int main(int argc, char** argv)
{
    using namespace my_shell;

    my_context ctx {
        .some_data = 0xcafebabe
    };

    nsh::Nsh<decltype(cmds), DefaultAutocompleter, DefaultSuggester> nsh;
    constexpr auto matching = nsh.autocompleter.autocomplete(cmds, "t");
    for (const auto& cmd : matching) {
        cmd.handler(ctx, argc, argv);
    }

    constexpr auto suggestion = nsh.suggester.suggest(cmds, "toot");
    puts(suggestion.data());
}
