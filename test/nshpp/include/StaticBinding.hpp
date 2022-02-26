#pragma once

#include <meta/TypeList.hpp>

#include <concepts>
#include <type_traits>

namespace nsh {

template<auto Input, auto Handler>
struct Bind
{
    using InputType = decltype(Input);
    using HandlerType = decltype(Handler);

    static constexpr auto input = Input;
    static constexpr auto handler = Handler;
};

template<auto Handler>
struct BindByDefault
{
    using HandlerType = decltype(Handler);

    static constexpr auto handler = Handler;
};

template<typename T>
concept BindType = requires // clang-format off
{
    { T::input } -> std::convertible_to<typename T::InputType>;
    { T::handler } -> std::convertible_to<typename T::HandlerType>;
}; // clang-format on

template<typename T>
concept DefaultBindType = requires // clang-format off
{
    { T::handler } -> std::convertible_to<typename T::HandlerType>;
}; // clang-format on

template<typename... Ts>
concept AreCompatible = requires // clang-format off
{
    typename std::common_type_t<Ts...>;
}; // clang-format on

namespace detail {

template<typename THandlerType, typename TInputType, typename TList>
struct GetFromInput
{
    static constexpr THandlerType value(TInputType input) noexcept
    {
        if (TList::Type::input == input) {
            return TList::Type::handler;
        }
        return GetFromInput<THandlerType, TInputType, typename TList::Next>::value(input);
    }
};

template<typename THandlerType, typename TInputType, typename TDefaultBind>
struct GetFromInput<THandlerType, TInputType, meta::TypeList<TDefaultBind>>
{
    static constexpr THandlerType value(TInputType) noexcept { return TDefaultBind::handler; }
};

} // namespace detail

template<DefaultBindType TDefaultBind, BindType... TBinds>
// clang-format off
    requires(AreCompatible<typename TBinds::InputType...>
          && AreCompatible<typename TDefaultBind::HandlerType, typename TBinds::HandlerType...>)
// clang-format on
struct StaticBinding
{
    using List = meta::TypeList<TBinds..., TDefaultBind>;
    using InputType = std::common_type_t<typename TBinds::InputType...>;
    using HandlerType
        = std::common_type_t<typename TDefaultBind::HandlerType, typename TBinds::HandlerType...>;

    static constexpr size_t size() noexcept { return List::size; }
    static constexpr HandlerType at(InputType input) noexcept
    {
        return detail::GetFromInput<HandlerType, InputType, List>::value(input);
    }
    constexpr HandlerType operator[](InputType input) noexcept { return at(input); }
};

} // namespace nsh
