#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct flags {
    int c;
    int l;
    int w;
    int m;
} flags;

int MAX_BYTES_STORAGE = 1024;

unsigned long long int* bytes;
unsigned long long int* lines;
char** fileNames;

FILE* getFileStream(char* fileName) {
    FILE* fileStream = fopen(fileName, "r");
    return fileStream;
}

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
                flagOps.w = 1;
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

int getFileIdx (char* userFlags[], int totalArgs) {
    int fileIdx = -1;

    for (int i = 1; i < totalArgs; i++) {
        char* userFlag = userFlags[i];
        if (userFlag[0] == '-') {
            continue;
        } else {
            fileIdx = i;
            break;
        }
    }

    return fileIdx;
}

void allocateMemory(flags flagOps, int size) {
    fileNames = (char**)realloc(fileNames, size * sizeof(char*));

    if (flagOps.c) {
        bytes = (unsigned long long int*)realloc(bytes, size * sizeof(long long int));
    }

    if (flagOps.l) {
        lines = (unsigned long long int*)realloc(lines, size * sizeof(long long int));
    }
}

void initDefaultValue(flags flagOps, int idx) {
    if (flagOps.c) {
        bytes[idx] = 0;
    }

    if (flagOps.l) {
        lines[idx] = 0;
    }
}

void display(flags flagOps, int totalFiles) {
    unsigned long long int totalBytes = 0;
    unsigned long long int totalLines = 0;
    unsigned long long int i;

    for (i = 0; i < totalFiles; i++) {
        char* fileName = fileNames[i];
        
        if (flagOps.c) {
            unsigned long long int byte = bytes[i];
            totalBytes += byte;
            fprintf(stdout, "%lld ", byte);
        }

        if (flagOps.l) {
            unsigned long long int line = lines[i];
            totalLines += line;
            fprintf(stdout, "%lld ", line);
        }

        fprintf(stdout, "%s\n", fileName);
    }

    fprintf(stdout, "%lld %lld %s\n", totalBytes, totalLines, "total");
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "[ERROR]:: Please provide the valid filename\n");
        exit(1);
    }

    flags flagOps = getFlags(argv, argc);
    int fileIdx = getFileIdx(argv, argc);
    if (fileIdx == -1) {
        fprintf(stderr, "[ERROR]:: Please provide the valid filename\n");
        exit(1);
    }

    for (int argIdx = fileIdx; argIdx < argc; argIdx++) {
        char* fileName = argv[argIdx];

        FILE* fileStream = getFileStream(fileName);
        if (fileStream == NULL) {
            fprintf(stderr, "[ERROR]:: Failed to open the file %s\n", fileName);
            continue;
        }

        allocateMemory(flagOps, (argIdx - fileIdx) + 1);
        initDefaultValue(flagOps, argIdx - fileIdx);

        fileNames[argIdx - fileIdx] = fileName;

        char character;
        while ((character = fgetc(fileStream)) != EOF) {
            if (flagOps.c) {
                bytes[argIdx - fileIdx]++;
            } else {
                bytes = NULL;
            }

            if (flagOps.l) {
                if (character == '\n') {
                    lines[argIdx - fileIdx]++;
                }
            } else {
                lines = NULL;
            }
        }

        fclose(fileStream);
    }

    int totalFiles = argc - fileIdx;
    display(flagOps, totalFiles);

    free(bytes);
    free(lines);
    free(fileNames);

    return 0;
}