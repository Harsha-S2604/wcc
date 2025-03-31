#include <stdio.h>
#include <stdlib.h>

char flags[] = {'c', 'l', 'm', 'w'};

int isFlagValid(char flag) {
    int flagPresent = 0;
    int totalFlags = sizeof(flags);

    // binary search to check for valid flags
    int left = 0, right = totalFlags - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        char validFlag = flags[mid];

        if (validFlag == flag) {
            flagPresent = 1;
            break;
        }

        if (validFlag > flag) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return flagPresent;
}

int areFlagsValid(char* flags[], int totalArgs) {
    int flagsValid = 1;
    
    for (int i = 1; i < totalArgs; i++) {
        if (flags[i][0] == '-') {
            int validFlag = isFlagValid(flags[i][1]);

            if (!validFlag) {
                flagsValid = 0;
                break;
            }
        }
    }

    return flagsValid;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "please provide the valid filename\n");
        exit(1);
    }

    int flagsValid = areFlagsValid(argv, argc);
    if (!flagsValid) {
        printf("please provide the valid flags.\nThe Valid flags are:\n-c, -l, -m, -w\n");
        exit(1);
    }

    return 0;
}