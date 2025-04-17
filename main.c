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
unsigned long long int* characters;
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

    if (flagOps.m) {
        characters = (unsigned long long int*)realloc(characters, size * sizeof(long long int));
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

    if (flagOps.m) {
        characters[idx] = 0;
    }
}

void validateFlagOps(flags* flagOps) {
    if (!flagOps -> c && !flagOps -> m && !flagOps -> l && !flagOps -> w) {
        flagOps -> c = 1;
        flagOps -> m = 1;
        flagOps -> w = 1;
        flagOps -> l = 1;
    }
}

void display(flags flagOps, int totalFiles) {
    unsigned long long int totalBytes = 0;
    unsigned long long int totalLines = 0;
    unsigned long long int totalWords = 0;
    unsigned long long int totalChars = 0;
    unsigned long long int i;

    // display individual file
    for (i = 0; i < totalFiles; i++) {
        char* fileName = fileNames[i];

        if (flagOps.w) {
            unsigned long long int word = words[i];
            totalWords += word;
            fprintf(stdout, "words-%lld ", word);
        }

        if (flagOps.l) {
            unsigned long long int line = lines[i];
            totalLines += line;
            fprintf(stdout, "lines-%lld ", line);
        }
        
        if (flagOps.c) {
            unsigned long long int byte = bytes[i];
            totalBytes += byte;
            fprintf(stdout, "bytes-%lld ", byte);
        }

        if (flagOps.m) {
            unsigned long long int character = characters[i];
            totalChars += character;
            fprintf(stdout, "characters-%lld ", character);
        }

        fprintf(stdout, "%s\n", fileName);
    }

    // display total values
    if (flagOps.w) {
        fprintf(stdout, "words-%lld ", totalWords);
    }

    if (flagOps.l) {
        fprintf(stdout, "lines-%lld ", totalLines);
    }

    if (flagOps.c) {
        fprintf(stdout, "bytes-%lld ", totalBytes);
    }

    if (flagOps.m) {
        fprintf(stdout, "characters-%lld ", totalChars);
    }

    fprintf(stdout, "total\n");
}

int utf8_char_length(unsigned char c) {
    if ((c & 0x80) == 0x00) return 1;           // 0xxxxxxx (ASCII)
    else if ((c & 0xE0) == 0xC0) return 2;      // 110xxxxx
    else if ((c & 0xF0) == 0xE0) return 3;      // 1110xxxx
    else if ((c & 0xF8) == 0xF0) return 4;      // 11110xxx
    else return -1;                             // Invalid first byte
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "[ERROR]:: Please provide the valid filename\n");
        exit(1);
    }

    flags flagOps = getFlags(argv, argc);
    validateFlagOps(&flagOps);

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

        int inCluster = 0;
        int clusterLen = 0;

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
                    inWord = 0;
                }
            } else {
                words = NULL;
            }

            if (flagOps.m) {
                // handling clusters: cluster is a sequence of characters that needs to be treated as a single
                if (!inCluster) {
                    clusterLen = utf8_char_length((unsigned char)character);
                    if (clusterLen > 0) {
                        inCluster = 1;
                        characters[idx]++;
                    }

                }

                if (inCluster) {
                    clusterLen -= 1;
                    if (clusterLen == 0) {
                        inCluster = 0;
                    }
                }
                
            } else {
                characters = NULL;
            }
        }

        fclose(fileStream);
    }

    int totalFiles = argc - fileIdx;
    display(flagOps, totalFiles);

    free(bytes);
    free(lines);
    free(words);
    free(characters);
    free(fileNames);

    return 0;
}  