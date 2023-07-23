#ifdef NSH_SIZE_REPORT_BASELINE

namespace nsh::tools {

int main(int /*argc*/, char* /*argv*/[])
{
    return 0;
}

} // namespace nsh::tools

#else

#include <nsh/nsh.h>

namespace nsh::tools {

int main(int /*argc*/, char* /*argv*/[])
{
    nsh_status_t status = NSH_STATUS_OK;
    nsh_t nsh = nsh_init(&status);
    nsh_run(&nsh);
    return 0;
}

} // namespace nsh::tools

#endif // NSH_SIZE_REPORT_BASELINE
