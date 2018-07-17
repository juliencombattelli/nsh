#include <nsh/shell.h>
#include <stdio.h>

int main(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    shell_init();
    run_shell();
    return 0;
}

