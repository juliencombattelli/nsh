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
