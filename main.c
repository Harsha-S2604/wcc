#include <stdio.h>

char flags[] = {'c', 'l', 'm', 'w'};

int isFlagValid(char flag) {
    int flagPresent = 0;
    int totalFlags = sizeof(flags);

    // binary search to check for valid
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

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Please provide the valid filename\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int validFlag = isFlagValid(argv[i][1]);

            if (!validFlag) {
                printf("Please provide the valid flags.\nThe Valid flags are:\n-c, -l, -m, -w\n");
                return 0;
            }
        }
        
        
    }

    return 1;
}