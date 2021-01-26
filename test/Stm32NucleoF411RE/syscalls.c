#include <_ansi.h>
#include <reent.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "stm32f4xx_hal.h"

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

register char* stack_ptr asm("sp");

caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char* heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
        heap_end = &end;

    prev_heap_end = heap_end;

    if (heap_end + incr > stack_ptr)
    {
        //		write(1, "Heap and stack collision\n", 25);
        //		abort();
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    /* Return fixed data for the timezone.  */
    if (tzp) {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    if (tp) {
        tp->tv_sec = 0;
        tp->tv_usec = HAL_GetTick() * 1000;
    }

    return 0;
}
void initialise_monitor_handles()
{
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{
    _kill(status, -1);
    while (1) { }
}

int _write(int file, char* ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _read(int file, char* ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = __io_getchar();
    }

    return len;
}

int _open(char* path, int flags, ...)
{
    /* Pretend like we always fail */
    return -1;
}

int _wait(int* status)
{
    errno = ECHILD;
    return -1;
}

int _unlink(char* name)
{
    errno = ENOENT;
    return -1;
}

int _times(struct tms* buf)
{
    return -1;
}

int _stat(char* file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _link(char* old, char* new)
{
    errno = EMLINK;
    return -1;
}

int _fork(void)
{
    errno = EAGAIN;
    return -1;
}

int _execve(char* name, char** argv, char** env)
{
    errno = ENOMEM;
    return -1;
}

char* getcwd(char* buf, size_t size)
{
    const char cwd[] = "./";
    if (sizeof(cwd) > size) {
        return NULL;
    }
    strncpy(buf, "./", size);
    return buf;
}

int mkdir(const char* path, mode_t mode)
{
    return 0;
}