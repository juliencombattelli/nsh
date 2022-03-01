#pragma once

#include <concepts>
#include <cstddef>
#include <utility>

template<typename T>
concept Binding = requires(const T obj)  // clang-format off
{
    typename T::InputType;
    typename T::HandlerType;

    { obj.size() } noexcept -> std::same_as<size_t>;

    { obj.operator[](std::declval<typename T::InputType>()) } noexcept
        -> std::same_as<typename T::HandlerType>;

}; // clang-format on
