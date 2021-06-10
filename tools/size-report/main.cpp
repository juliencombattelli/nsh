#include <nsh/nsh.h>
#include <nsh/nsh_io_plugin_default.h>

namespace nsh::tools {

int main(int /*argc*/, char* /*argv*/[])
{
    nsh_status_t status = NSH_STATUS_OK;
    nsh_t nsh = nsh_init(nsh_io_make_default_plugin(), &status);
    nsh_run(&nsh);
    return 0;
}

} // namespace nsh::tools
