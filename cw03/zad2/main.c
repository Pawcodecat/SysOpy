#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define MAX_ABS_VAL  100
#define MAX_LINE_LEN  100000
#define MAX_INPUT_LINES  100000
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))

typedef struct matrix {
    char* file;
    int columns;
    int rows;
    int outColumns;
} matrix;


matrix** matricesA;
matrix** matricesB;
matrix** matricesC;



double timeLimit;
bool individualWrite;


double timeDiff(clock_t start, clock_t end)
{
    return ((double)(start - end) / CLOCKS_PER_SEC);
}

int* getColumn(matrix* arr, FILE* file, int numberOfRows)
{
    rewind(file);
    int* column = (int*)calloc(arr->rows, sizeof(int));
    int i = 0;
    char* row = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    char* ptr = row;
    while(fgets(row, MAX_LINE_LEN, file) != NULL)
    {
        char* elem;
        for(int j = 0; j <= numberOfRows; j++)
        {
            elem = strsep(&row, " \n");
        }
        column[i++] = atoi(elem);
    }
    free(ptr);
    return column;
}




int* getRow(matrix* arr, FILE* file, int numberOfRows) {

    rewind(file);
    int* row = (int*)calloc(arr->columns, sizeof(int));
    char* buff = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    for(int i = 0; i < numberOfRows; i++)
        fgets(buff, MAX_LINE_LEN, file);
    buff = fgets(buff, MAX_LINE_LEN, file);

    char* elem;

    for(int j = 0; j < arr->columns; j++)
    {
        elem = strsep(&buff, " \n");
        row[j] = atoi(elem);
    }

    return row;
}


void writeColumn(matrix* m, FILE* file, int* column, int numberOfRows)
{
    flock(fileno(file), LOCK_EX);
    for(int i = 0; i < m->rows; i++)
    {
        int pos = m->outColumns * (numberOfRows + i * m->columns) + i;
        fseek(file, pos, 0);
        char* str = (char*)calloc(m->outColumns, sizeof(char));
        sprintf(str, "%d", column[i]);
        int j = m->outColumns - 1;
        while(str[j] == 0)
        {
            str[j] = ' ';
            j--;
        }
        fwrite(str, 1, m->outColumns, file);
        free(str);
    }
    flock(fileno(file), LOCK_UN);
}

void writeColumnIndividual(matrix* m, FILE* file, int* column, int numberOfRows)
{
    for(int i = 0; i < m->rows; i++)
    {
        char* str = (char*)calloc(m->outColumns, sizeof(char));
        sprintf(str, "%d", column[i]);
        int j = m->outColumns - 1;
        while(str[j] == 0)
        {
            str[j] = ' ';
            j--;
        }
        fwrite(str, 1, m->outColumns, file);
        fwrite("\n", 1, 1, file);
        free(str);
    }
}

int matrixColumns(FILE* file)
{
    rewind(file);
    char buffer[MAX_LINE_LEN];
    fgets(buffer, MAX_LINE_LEN, file);
    int i = 1;
    strtok(buffer, " \n");
    while(strtok(NULL, " \n") != NULL) i++;
    return i;
}

int matrixRows(FILE* file)
{
    rewind(file);
    char buffer[MAX_LINE_LEN];
    int i = 0;
    while(fgets(buffer, MAX_LINE_LEN, file) != NULL) i++;
    return i;
}


void multiplyMatrix(int startCol, int endCol, int pairIn)
{
    matrix* A = matricesA[pairIn];
    matrix* B = matricesB[pairIn];
    matrix* C = matricesC[pairIn];
    FILE* fA = fopen(A->file, "r+");
    FILE* fB = fopen(B->file, "r+");
    FILE* fC;
    if(!individualWrite) fC = fopen(C->file, "r+");

    int** m_A = (int**)calloc(A->rows, sizeof(int*));
    for(int i = 0; i < A->rows; i++)
    {
        m_A[i] = getRow(A, fA, i);
    }

    for(int midCol = startCol; midCol <= endCol; midCol++)
    {
        int* m_B_col = getColumn(B, fB, midCol);
        int* m_C_col = (int*)calloc(C->rows, sizeof(int));

        for(int i = 0; i < C->rows; i++)
        {
            int val = 0;
            for(int j = 0; j < A->columns; j++)
            {
                val += m_A[i][j] * m_B_col[j];
            }
            m_C_col[i] = val;
        }

        if(individualWrite)
        {
            char* fileName = (char*)calloc(100, sizeof(char));
            strcpy(fileName, C->file);
            strcat(fileName, "_");
            sprintf(fileName + strlen(fileName), "%d", midCol);
            fC = fopen(fileName, "w+");
            free(fileName);
            writeColumnIndividual(C, fC, m_C_col, midCol);
        } else
            {
            writeColumn(C, fC, m_C_col, midCol);
        }

        free(m_B_col);
        free(m_C_col);
    }
    fclose(fA);
    fclose(fB);
    fclose(fC);
}

int multiplyMatrices(int procWork, int procNumber, int matricesPairCount, clock_t start_t)
{
    int n = 0;
    for(int i = 0; i < matricesPairCount; i++)
    {
        int colNumber = matricesB[i]->columns;
        if(procWork < colNumber)
        {
            n++;
            int startCol = procWork * (int) ceil((double) colNumber / procNumber);
            int endCol = min((procWork + 1) * (int) ceil((double) colNumber / procNumber) - 1, colNumber - 1);
            multiplyMatrix(startCol, endCol, i);
        }
        if(timeDiff(start_t, clock()) >= timeLimit)
        {
            return n;
        }
    }
    return n;
}


int main(int argc, char** argv)
{
    if(argc < 5)
        printf("Too few arguments\n");

    char* tests_list = argv[1];
    int procNumber = atoi(argv[2]);
    timeLimit = strtod(argv[3], NULL);
    individualWrite = (atoi(argv[4]) == 2);

    pid_t* childrenPids = calloc(procNumber, sizeof(pid_t));

    FILE* f = fopen(tests_list, "r");

    matricesA = calloc(MAX_INPUT_LINES, sizeof(matrix*));
    matricesB = calloc(MAX_INPUT_LINES, sizeof(matrix*));
    matricesC = calloc(MAX_INPUT_LINES, sizeof(matrix*));

    int i = 0;
    FILE* tmp;
    char buffer[100];
    while(fscanf(f, "%s", buffer) != EOF)
    {
        int in = i / 3;
        switch (i % 3)
        {
            case 0:
                matricesA[in] = malloc(sizeof(matrix));
                tmp = fopen(buffer, "r");
                matricesA[in]->file = (char*)calloc(100, sizeof(char));
                strcpy(matricesA[in]->file, buffer);
                matricesA[in]->columns = matrixColumns(tmp);
                matricesA[in]->rows = matrixRows(tmp);
                fclose(tmp);
                break;
            case 1:
                matricesB[in] = malloc(sizeof(matrix));
                tmp = fopen(buffer, "r");
                matricesB[in]->file = (char*)calloc(100, sizeof(char));
                strcpy(matricesB[in]->file, buffer);
                matricesB[in]->columns = matrixColumns(tmp);
                matricesB[in]->rows = matrixRows(tmp);
                fclose(tmp);
                break;
            case 2:
                matricesC[in] = malloc(sizeof(matrix));
                tmp = fopen(buffer, "w+");
                matricesC[in]->file = (char*)calloc(100, sizeof(char));
                strcpy(matricesC[in]->file, buffer);
                matricesC[in]->columns= matricesB[in]->columns;
                matricesC[in]->rows = matricesA[in]->rows;

                int columns = matricesC[in]->columns;
                int rows = matricesC[in]->rows;
                int outColumns = (int) ceil(log10(matricesA[in]->columns * MAX_ABS_VAL * MAX_ABS_VAL)) + 3;

                matricesC[in]->outColumns = outColumns;

                char* buf = (char*)calloc(rows * (columns * outColumns + 1) + 1, sizeof(char));
                strcpy(buf, "");
                for(int i = 0; i < rows; i++)
                {
                    for(int j = 0; j < columns * outColumns; j++)
                    {
                        strcat(buf, "@");
                    }
                    strcat(buf, "\n");
                }
                fwrite(buf, 1, (columns * outColumns + 1) * rows, tmp);
                free(buf);
                fclose(tmp);
                break;
        }
        i++;
    }

    int matricesPairCount = i / 3;

    fclose(f);

    for(int i = 0; i < procNumber; i++)
    {
        pid_t childPid = fork();
        if(childPid == 0) {
            int matricesMultiplied = multiplyMatrices(i, procNumber, matricesPairCount, clock());
            exit(matricesMultiplied);
        } else
            {
            childrenPids[i] = childPid;
        }
    }

    for(int i = 0; i < procNumber; i++)
    {
        int status;
        waitpid(childrenPids[i], &status, 0);
        printf("Number of multiplying operations: %d finished by process: %d\n", WEXITSTATUS(status), childrenPids[i]);
    }

    if(individualWrite)
    {
        for(int i = 0; i < matricesPairCount; i++)
        {
            pid_t childPid = fork();

            if(childPid == 0)
            {
                char** args = (char**)calloc(matricesC[i]->columns + 2, sizeof(char*));
                args[0] = (char*)calloc(6, sizeof(char));
                strcpy(args[0], "paste");
                for(int j = 0; j < matricesC[i]->columns; j++)
                {
                    args[j + 1] = (char*)calloc(100, sizeof(char));
                    strcpy(args[j + 1], matricesC[i]->file);
                    strcat(args[j + 1], "_");
                    sprintf(args[j + 1] + strlen(args[j + 1]), "%d", j);
                }
                args[matricesC[i]->columns + 1] = NULL;

                int fd = open(matricesC[i]->file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(fd, 1);
                close(fd);

                execvp(args[0], args);
            } else
                {
                waitpid(childPid, NULL, 0);
            }
        }
    }

    return 0;
}
