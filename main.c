#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

typedef struct flags {
    int c;
    int l;
    int w;
    int m;
} flags;

int MAX_BYTES_STORAGE = 1024;

unsigned long long int* bytes;
unsigned long long int* lines;
unsigned long long int* words;
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

    if (flagOps.w) {
        words = (unsigned long long int*)realloc(words, size * sizeof(long long int));
    }
}

void initDefaultValue(flags flagOps, int idx) {
    if (flagOps.c) {
        bytes[idx] = 0;
    }

    if (flagOps.l) {
        lines[idx] = 0;
    }

    if (flagOps.w) {
        words[idx] = 0;
    }
}

void display(flags flagOps, int totalFiles) {
    unsigned long long int totalBytes = 0;
    unsigned long long int totalLines = 0;
    unsigned long long int totalWords = 0;
    unsigned long long int i;

    // display individual file
    for (i = 0; i < totalFiles; i++) {
        char* fileName = fileNames[i];

        if (flagOps.w) {
            unsigned long long int word = words[i];
            totalWords += word;
            fprintf(stdout, "%lld ", word);
        }

        if (flagOps.l) {
            unsigned long long int line = lines[i];
            totalLines += line;
            fprintf(stdout, "%lld ", line);
        }
        
        if (flagOps.c) {
            unsigned long long int byte = bytes[i];
            totalBytes += byte;
            fprintf(stdout, "%lld ", byte);
        }

        fprintf(stdout, "%s\n", fileName);
    }

    // display total values
    if (flagOps.w) {
        fprintf(stdout, "%lld ", totalWords);
    }

    if (flagOps.l) {
        fprintf(stdout, "%lld ", totalLines);
    }

    if (flagOps.c) {
        fprintf(stdout, "%lld ", totalBytes);
    }

    fprintf(stdout, "total\n");
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
        int idx = argIdx - fileIdx;
        char* fileName = argv[argIdx];

        FILE* fileStream = getFileStream(fileName);
        if (fileStream == NULL) {
            fprintf(stderr, "[ERROR]:: Failed to open the file %s\n", fileName);
            continue;
        }

        allocateMemory(flagOps, idx + 1);
        initDefaultValue(flagOps, idx);

        fileNames[idx] = fileName;

        char character;
        int inWord = 0;
        int totalCharacters = 0;

        while ((character = fgetc(fileStream)) != EOF) {
            if (flagOps.c) {
                bytes[idx]++;
            } else {
                bytes = NULL;
            }

            if (flagOps.l) {
                if (character == '\n') {
                    lines[idx]++;
                }
            } else {
                lines = NULL;
            }

            if (flagOps.w) {
                if (!inWord && !isspace(character) && character != '\n') {
                    inWord = 1;
                    words[idx]++;
                } else if (isspace(character) || character == '\n') {
                    if (words[idx] > 6539) {
                        printf("doc\n");
                        putchar(character);
                    }
                    inWord = 0;
                }
            } else {
                words = NULL;
            }

            // if (flagOps.m) {
            //     if (character != '\n') {
            //         totalCharacters++;
            //     }
            // } else {

            // }
        }

        fclose(fileStream);
    }

    int totalFiles = argc - fileIdx;
    display(flagOps, totalFiles);

    free(bytes);
    free(lines);
    free(words);
    free(fileNames);

    return 0;
}  