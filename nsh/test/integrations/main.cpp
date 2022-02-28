#include <nsh/nsh.h>
#include <nsh/nsh_io_plugin_default.h>

#include <cstddef>

namespace nsh::termios {

int main(int, char*[])
{
    nsh_status_t status = NSH_STATUS_OK;
    nsh_t nsh = nsh_init(nsh_io_make_default_plugin(), &status); // status intentionally ignored
    nsh_register_command(&nsh, "null", NULL); // NSH_NON_NULL precondition not satisfied
    nsh_run(&nsh);
    return 0;
}

} // namespace nsh::termios
