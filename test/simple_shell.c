#include <nsh/nsh.h>

#include <stdio.h>

int main(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    nsh_init();
    run_nsh();
    return 0;
}
