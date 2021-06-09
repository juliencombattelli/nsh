#include <nsh/nsh.h>

namespace nsh::tools {

int main(int /*argc*/, char* /*argv*/[])
{
    nsh_init();
    nsh_run();
    return 0;
}

}
