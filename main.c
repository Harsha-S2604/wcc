#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct flags {
    int c;
    int l;
    int f;
    int m;
} flags;

flags getFlags(char* userFlags[], int totalArgs) {
    int opt;
    flags flagOps = { 0, 0, 0, 0 };

    while ((opt = getopt(totalArgs, userFlags, "clwm")) != -1) {
        switch (opt) {
            case 'c':
                flagOps.c = 1;
                break;
            case 'l':
                flagOps.l = 1;
                break;
            case 'w':
                flagOps.f = 1;
                break;
            case 'm':
                flagOps.m = 1;
                break;
            default:
                break;
        }
    }

    return flagOps;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "please provide the valid filename\n");
        exit(1);
    }

    flags flagOps = getFlags(argv, argc);
    return 0;
}