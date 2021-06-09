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

#include <stdio.h>

int main(void)
{
    enableRawMode();
    // setvbuf(stdin, NULL, _IONBF, 0);
    // setvbuf(stdout, NULL, _IONBF, 0);
    nsh_init();
    nsh_register_command("null", NULL); // NON_NULL precondition not satisfied
    nsh_run();
    return 0;
}
