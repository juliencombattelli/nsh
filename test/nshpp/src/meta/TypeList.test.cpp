#include <meta/TypeList.hpp>

#include <type_traits>

namespace nsh::meta::test {

static_assert(TypeList<>::size == 0);
static_assert(TypeList<void>::size == 0);
static_assert(TypeList<int>::size == 1);
static_assert(TypeList<int, float, char>::size == 3);
static_assert(std::is_same_v<TypeList<int, float, char, int>::Next::Next::Type, char>);

} // namespace nsh::meta::test
