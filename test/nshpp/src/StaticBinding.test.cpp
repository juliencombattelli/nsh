#include "StaticBinding.hpp"

namespace nsh::test {

static_assert(BindType<Bind<0, [] { return 42; }>>);
static_assert(DefaultBindType<BindByDefault<[] { return 42; }>>);

} // namespace nsh::test
