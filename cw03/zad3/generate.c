#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int randomInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main(int argc, char** argv) {
    if(argc < 4) {
        printf("Too few arguments\n");
    }

    srand(time(NULL));

    int n = atoi(argv[1]), min = atoi(argv[2]), max = atoi(argv[3]);

    char* file_name = (char*) calloc(100, sizeof(char));

    FILE* tests_list = fopen("tests_list", "w+");

    for(int i = 0; i < n; i++) {
        char str[3];
        sprintf(str, "%d", i);

        strcpy(file_name, "tests/");
        strcat(file_name, str);
        strcat(file_name, "_A");
        FILE* fA = fopen(file_name, "w+");
        fprintf(tests_list, "%s ", file_name);

        strcpy(file_name, "tests/");
        strcat(file_name, str);
        strcat(file_name, "_B");
        FILE* fB = fopen(file_name, "w+");
        fprintf(tests_list, "%s ", file_name);

        strcpy(file_name, "tests/");
        strcat(file_name, str);
        strcat(file_name, "_C");
        fprintf(tests_list, "%s\n", file_name);

        int ARows = randomInt(min, max);
        int AColumns = randomInt(min, max);
        int BColumns = randomInt(min, max);


        for(int i = 0; i < ARows; i++) {
            for(int j = 0; j < AColumns; j++) {
                fprintf(fA, "%d", randomInt(-100, 100));
                fprintf(fA, " ");
            }
            fprintf(fA, "\n");
        }

        for(int i = 0; i < AColumns; i++) {
            for(int j = 0; j < BColumns; j++) {
                fprintf(fB, "%d", randomInt(-100, 100));
                fprintf(fB, " ");
            }
            fprintf(fB, "\n");
        }

        fclose(fA);
        fclose(fB);
    }

    return 0;
}