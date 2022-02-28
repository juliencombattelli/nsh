#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

namespace nsh::termios {
extern int main(int argc, char* argv[]);
}

static struct termios orig_termios;

static void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= static_cast<tcflag_t>(~(ECHO | ICANON));
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

namespace nsh::tools {

int main(int argc, char* argv[])
{
    enableRawMode();
    // setvbuf(stdin, NULL, _IONBF, 0);
    // setvbuf(stdout, NULL, _IONBF, 0);
    return termios::main(argc, argv);
}

} // namespace nsh::tools

// #include <nsh/nsh.h>
// #include <nsh/nsh_io_plugin_default.h>

// #include <stdio.h>

// int main(void)
// {
//     enableRawMode();
//     // setvbuf(stdin, NULL, _IONBF, 0);
//     // setvbuf(stdout, NULL, _IONBF, 0);
//     nsh_status_t status = NSH_STATUS_OK;
//     nsh_t nsh = nsh_init(nsh_io_make_default_plugin(), &status); // status intentionally ignored
//     nsh_register_command(&nsh, "null", NULL); // NSH_NON_NULL precondition not satisfied
//     nsh_run(&nsh);
//     return 0;
// }
