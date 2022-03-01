#include <nshpp/Binding.hpp>

namespace nsh::test {

struct CorrectBinding
{
    using InputType = char;
    using HandlerType = int(*)(int, char**) noexcept;

    constexpr size_t size() const noexcept
    {
        return 0;
    }
    constexpr HandlerType operator[](InputType) const noexcept
    {
        return nullptr;
    }
};
static_assert(Binding<CorrectBinding>);

struct IncorrectBindingNoInputType
{
    using HandlerType = int(*)(int, char**) noexcept;

    static constexpr size_t size() noexcept
    {
        return 0;
    }
    constexpr HandlerType operator[](int) const noexcept
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoInputType>);

struct IncorrectBindingNoHandlerType
{
    using InputType = char;

    static constexpr size_t size() noexcept
    {
        return 0;
    }
    constexpr void* operator[](InputType) const noexcept
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoHandlerType>);

struct IncorrectBindingNoValidSizeMethod1
{
    using InputType = char;
    using HandlerType = int(*)(int, char**) noexcept;

    constexpr size_t size() noexcept // missing const
    {
        return 0;
    }
    constexpr size_t size() const // missing noexcept
    {
        return 0;
    }
    constexpr HandlerType operator[](InputType) const noexcept
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoValidSizeMethod1>);

struct IncorrectBindingNoValidSizeMethod2
{
    using InputType = char;
    using HandlerType = int(*)(int, char**) noexcept;

    constexpr InputType size(int) const noexcept // wrong parameter type
    {
        return 0;
    }
    constexpr char size() const noexcept // wrong return type
    {
        return 0;
    }
    constexpr HandlerType operator[](InputType) const noexcept
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoValidSizeMethod2>);

struct IncorrectBindingNoValidSubscriptOperator1
{
    using InputType = char;
    using HandlerType = int(*)(int, char**) noexcept;

    constexpr size_t size() const noexcept
    {
        return 0;
    }
    constexpr HandlerType operator[](InputType) noexcept // missing const
    {
        return nullptr;
    }
    constexpr HandlerType operator[](InputType) const // missing noexcept
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoValidSubscriptOperator1>);

struct IncorrectBindingNoValidSubscriptOperator2
{
    using InputType = char;
    using HandlerType = int(*)(int, char**) noexcept;

    constexpr size_t size() const noexcept
    {
        return 0;
    }
    constexpr HandlerType operator[](float) const noexcept // wrong parameter type
    {
        return nullptr;
    }
    constexpr void* operator[](InputType) const noexcept // wrong return type
    {
        return nullptr;
    }
};
static_assert(!Binding<IncorrectBindingNoValidSubscriptOperator2>);

} // namespace nsh::test
