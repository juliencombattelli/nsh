#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static long stol(const char* str)
{
    errno = 0;
    char* end = NULL;
    const long l = strtol(str, &end, 0);
    if (errno == ERANGE || *end != '\0' || str == end) {
        printf("strtol: Error when parsing string\n");
        exit(-1);
    }
    return l;
}

#define USEC_IN_MSEC 1000u

int main(int argc, char** argv)
{
    long delay_ms = 0;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            { "delay", required_argument, 0, 0 },
            { 0, 0, 0, 0 }
        };

        int c = getopt_long(argc, argv, "d:", long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'd':
            delay_ms = stol(optarg);
            break;

        case '?':
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            usleep(delay_ms * USEC_IN_MSEC);
            puts(argv[optind++]);
        }
    }

    exit(EXIT_SUCCESS);
}
