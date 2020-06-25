#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct matrix {
    int** values;
    int columns;
    int rows;
} matrix;


matrix* readMatrix(char* file_name)
{
    FILE* f = fopen(file_name, "r");
    char* row = (char*)calloc(1000, sizeof(char));
    matrix* mat = (matrix*)malloc(sizeof(matrix));
    mat->values = (int**)calloc(1000, sizeof(int*));
    int i = 0, j;
    while(fgets(row, 1000, f) != NULL)
    {
        mat->values[i] = (int*)calloc(1000, sizeof(int));
        j = 0;
        char* elem = strtok(row, " \n");
        while(elem != NULL)
        {
            mat->values[i][j++] = atoi(elem);
            elem = strtok(NULL, " \n");
        }
        i++;
    }
    mat->rows = i; mat->columns = j;
    free(row);
    fclose(f);
    return mat;
}

void writeMatrix(matrix* m)
{
    for(int i = 0; i < m->rows; i++)
    {
        for(int j = 0; j < m->columns; j++)
        {
            printf("%d ", m->values[i][j]);
        }
        printf("\n");
    }
}

matrix* multiplyMatrices(matrix* mA, matrix* mB)
{
    matrix* mC = (matrix*)malloc(sizeof(matrix));
    mC->columns = mB->columns;
    mC->rows = mA->rows;
    mC->values = (int**)calloc(mC->rows, sizeof(int*));
    for(int i = 0; i < mC->rows; i++) mC->values[i] = (int*)calloc(mC->columns, sizeof(int));

    for(int i = 0; i < mC->rows; i++)
    {
        for(int j = 0; j < mC->columns; j++)
        {
            mC->values[i][j] = 0;
            for(int k = 0; k < mA->columns; k++)
            {
                mC->values[i][j] += mA->values[i][k] * mB->values[k][j];
            }
        }
    }

    return mC;
}

bool compareMatrices(matrix* mA, matrix* mB)
{
    if(mA->columns != mB->columns || mA->rows != mB->rows) return false;

    for(int i = 0; i < mA->rows; i++)
    {
        for(int j = 0; j < mA->columns; j++)
        {
            if(mA->values[i][j] != mB->values[i][j]) return false;
        }
    }

    return true;
}


int main(int argc, char** argv)
{
    if(argc < 2)
        printf("Too few arguments\n");


    char* tests_list = argv[1];
    FILE* f = fopen(tests_list, "r");
    char fileName[1000];
    int i = 0;

    
    matrix* mA, *mB, *mC;
    while(fscanf(f, "%s", fileName) != EOF)
    {
        switch (i % 3)
        {
            case 0:
                mA = readMatrix(fileName);
                break;
            case 1:
                mB = readMatrix(fileName);
                break;
            case 2:
                mC = readMatrix(fileName);
                bool ifMatricesTheSame = compareMatrices(multiplyMatrices(mA, mB), mC);
                printf("Data generated nr %d: are with check data are:\t%s\n", i/3, ifMatricesTheSame ? "THE SAME" : "DIFFRENT");
                break;
        }
        i++;
    }
    return 0;
}