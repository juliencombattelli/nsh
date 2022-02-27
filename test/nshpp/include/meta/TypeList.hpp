#pragma once

#include <cstdlib>

namespace nsh::meta {

template<typename... Ts>
struct TypeList;

template<>
struct TypeList<>
{
    using Type = void;
    using Next = void;
    static constexpr size_t size = 0;
};

template<>
struct TypeList<void> : public TypeList<>
{};

template<typename T, typename... Ts>
struct TypeList<T, Ts...>
{
    using Type = T;
    using Next = TypeList<Ts...>;
    static constexpr size_t size = sizeof...(Ts) + 1;
};

template<typename TList>
struct Front;

template<typename THead, typename... TTail>
struct Front<TypeList<THead, TTail...>>
{
    using type = THead;
};

template<typename TList>
struct Back;

template<typename... THead, typename TTail>
struct Back<TypeList<THead..., TTail>>
{
    using type = TTail;
};

template<typename TList, typename TElement>
struct PushFront;

template<typename... TElements, typename TElement>
struct PushFront<TypeList<TElements...>, TElement>
{
    using type = TypeList<TElement, TElements...>;
};

template<typename TList>
struct PopFront;

template<typename THead, typename... TTail>
struct PopFront<TypeList<THead, TTail...>>
{
    using type = TypeList<TTail...>;
};

template<typename TList, unsigned Index>
struct Get
{
    using type = typename Get<typename PopFront<TList>::type, Index - 1>::type;
};

template<typename TList>
struct Get<TList, 0>
{
    using type = typename Front<TList>::type;
};

} // namespace nsh::meta
