#ifndef NSH_HPP_
#define NSH_HPP_

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
template<typename T, typename... U>
inline constexpr bool AllSame = std::conjunction_v<std::is_same<T, U>...>;

/**
 * @brief Shell command class associating a name and a command to execute.
 * @tparam THandler Type of the callable to execute when the command is invoked.
 *
 * THandler can be any callable type like pointer to function, functor, std::function.
 */
template<typename THandler>
struct Cmd
{
    using HandlerType = THandler;
    std::string_view name;
    THandler handler;
};

/**
 * @brief List of commands that a nsh shell can use.
 * @tparam TCmd Type of the commands of the list.
 * @tparam N Commands count in the list.
 *
 * This class behaves like a sorted std::array of N TCmds.
 * It cannot be empty by design: it always contains at least one element.
 */
template<typename TCmd, std::size_t N>
    requires(N > 0)
class CmdList
{
public:
    using CmdType = TCmd;

    constexpr CmdList() = default;

    template<typename TCmdFirst, typename... TCmds>
        requires(AllSame<TCmdFirst, TCmds...>)
    constexpr explicit CmdList(TCmdFirst cmdfirst, TCmds... cmds) noexcept
        : list{ cmdfirst, cmds... }
    {
        std::ranges::sort(list, {}, &TCmdFirst::name);
    }

    constexpr auto begin() noexcept { return list.begin(); }
    constexpr auto begin() const noexcept { return list.begin(); }
    constexpr auto end() noexcept { return list.end(); }
    constexpr auto end() const noexcept { return list.end(); }
    constexpr auto size() const noexcept { return list.size(); }
    constexpr auto operator[](std::size_t pos) { return list[pos]; }
    constexpr auto operator[](std::size_t pos) const { return list[pos]; }

private:
    std::array<TCmd, N> list;
};

/**
 * @brief Deduction guide for CmdList template class.
 *
 * Allow the construction of the list without explicitly specifying its size.
 * The size template argument is deduced from the size of TCmds parameter pack.
 */
template<typename TCmdFirst, typename... TCmds>
explicit CmdList(TCmdFirst, TCmds...) -> CmdList<TCmdFirst, 1 + sizeof...(TCmds)>;

/**
 * @brief Concept specifying that a type can be used as a command autocompleter for nsh.
 * TODO remove dependency to TCmds
 */
template<typename T, typename TCmds>
concept Autocompleter = requires(T t, TCmds cmds, std::string_view str)
{
    // add default constructability
    { t.autocomplete(cmds, str) } /*-> std::same_as<std::span<typename TCmds::CmdType>>*/;
};

/**
 * @brief Concept specifying that a type can be used as a command suggester for nsh.
 */
template<typename T, typename TCmds>
concept Suggester = requires(T t, TCmds cmds, std::string_view str)
{
    // add default constructability
    { t.suggest(cmds, str) } /*-> std::same_as<std::span<typename TCmds::CmdType>>*/;
};

struct DefaultAutocompleter
{
    template<typename TCmdList>
    static constexpr auto autocomplete(TCmdList&& cmds, std::string_view str) noexcept
    {
        // Since list is already sorted lexicographically, all elements starting
        // with str are contiguous. So we search for the first one that matches,
        // the last one that does not match, and return a span from the first
        // element to the last one.
        const auto starts_with_str = [&](auto cmd) { return cmd.name.starts_with(str); };
        const auto first = std::find_if(cmds.begin(), cmds.end(), starts_with_str);
        const auto last = std::find_if_not(first, cmds.end(), starts_with_str);
        return std::span(first, last);
    }
};

template<std::size_t N>
constexpr std::size_t levenshteinDistance(
    std::string_view string_a, std::string_view string_b) noexcept
{
    const auto size_a = string_a.size();
    const auto size_b = string_b.size();

    if (size_a > N || size_b > N) {
        return std::numeric_limits<std::size_t>::max();
    }

    std::array<std::size_t, N> distances;
    std::iota(distances.begin(), distances.end(), std::size_t{ 0 });

    for (std::size_t i = 0; i < size_a; ++i) {
        std::size_t previous_distance = 0;
        for (std::size_t j = 0; j < size_b; ++j) {
            distances[j + 1] = std::min({
                std::exchange(previous_distance, distances[j + 1])
                    + (string_a[i] == string_b[j] ? 0 : 1), // replacement cost
                distances[j] + 1, // insertion cost
                distances[j + 1] + 1 // deletion cost
            });
        }
    }
    return distances[size_b];
}

struct DefaultSuggester
{
    template<typename TCmdList>
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

template<typename THandler, typename TAutocompleter = DefaultAutocompleter,
    typename TSuggester = DefaultSuggester>
struct Settings
{
    using Handler = THandler;
    using Autocompleter = TAutocompleter;
    using Suggester = TSuggester;
    std::size_t LineBufferSize = 128;
    std::size_t MaxArgumentCount = 32;
    std::size_t CmdNameSize = 16;
    std::size_t CmdCount = 32;
    std::size_t HistorySize = 16;
};

template<auto settings>
class Nsh
{
public:
    using Settings = decltype(settings);
    Settings::Autocompleter autocompleter;
    Settings::Suggester suggester;
    CmdList<Cmd<typename Settings::Handler>, settings.CmdCount> cmds;
};

} // namespace nsh

#endif // NSH_HPP_