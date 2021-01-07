#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int delay_ms = 0;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            { "delay", required_argument, 0, 0 },
            { 0, 0, 0, 0 }
        };

        int c = getopt_long(argc, argv, "d:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'd':
            delay_ms = atoi(optarg);
            break;

        case '?':
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            usleep(delay_ms * 1000);
            puts(argv[optind++]);
        }
    }

    exit(EXIT_SUCCESS);
}
