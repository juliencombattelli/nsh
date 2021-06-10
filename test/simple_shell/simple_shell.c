#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= (tcflag_t)(~(ECHO | ICANON));
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#include <nsh/nsh.h>
#include <nsh/nsh_io_plugin_default.h>

#include <stdio.h>

int main(void)
{
    enableRawMode();
    // setvbuf(stdin, NULL, _IONBF, 0);
    // setvbuf(stdout, NULL, _IONBF, 0);
    nsh_status_t status = NSH_STATUS_OK;
    nsh_t nsh = nsh_init(nsh_io_make_default_plugin(), &status); // status intentionally ignored
    nsh_register_command(&nsh, "null", NULL);                    // NSH_NON_NULL precondition not satisfied
    nsh_run(&nsh);
    return 0;
}
