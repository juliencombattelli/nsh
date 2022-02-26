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

template<typename List>
struct Front;

template<typename Head, typename... Tail>
struct Front<TypeList<Head, Tail...>>
{
    using type = Head;
};

template<typename List>
struct Back;

template<typename... Head, typename Tail>
struct Back<TypeList<Head..., Tail>>
{
    using type = Tail;
};

template<typename List, typename Element>
struct PushFront;

template<typename... Elements, typename Element>
struct PushFront<TypeList<Elements...>, Element>
{
    using type = TypeList<Element, Elements...>;
};

template<typename List>
struct PopFront;

template<typename Head, typename... Tail>
struct PopFront<TypeList<Head, Tail...>>
{
    using type = TypeList<Tail...>;
};

template<typename List, unsigned Index>
struct Get
{
    using type = typename Get<typename PopFront<List>::type, Index - 1>::type;
};

template<typename List>
struct Get<List, 0>
{
    using type = typename Front<List>::type;
};

} // namespace nsh::meta
