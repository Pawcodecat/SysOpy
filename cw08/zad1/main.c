#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

int W;
int H;
int **image;

int threads;
int **histogram;

struct arg_struct
{
    int arg1;
    char* mode;
};

void error_and_exit(char *msg)
{
    printf("Error: %s\n", msg);
    printf("Errno: %d\n", errno);
    exit(EXIT_FAILURE);
}

void set_W_and_H(char* buffer)
{
    char *w;
    char *h;
    char *rest = buffer;

    w = strtok_r(rest, " \t\r\n", &rest);
    h = strtok_r(rest, " \t\r\n", &rest);
    W = atoi(w);
    H = atoi(h);
}

void load_row_to_array(char* line, int r)
{
    char *i;
    char *rest = line;
    for(int col = 0; col < W; col++)
    {
        i = strtok_r(rest, " \t\r\n", &rest);
        image[r][col] = atoi(i);
    }
}

void load_image_to_array(char *filename)
{
    FILE *file = fopen(filename,"r");
    if(file == NULL)
        error_and_exit("Cannot open file");

    int MAX_LINE_LENGTH = 128;
    char buffer[MAX_LINE_LENGTH + 1];

    fgets(buffer, MAX_LINE_LENGTH, file);
    do
    {
        fgets(buffer, MAX_LINE_LENGTH, file);
    } while (buffer[0] == '#');
    set_W_and_H(buffer);
    
    fgets(buffer, MAX_LINE_LENGTH, file);
 

    image = calloc(H, sizeof(int*));
    for(int i = 0; i < H; i++)
        image[i] = calloc(W, sizeof(int));
    
    char *line = NULL;
    size_t len = 0;
    
    for(int row = 0; row < H; row++)
    {
        getline(&line, &len, file);
        load_row_to_array(line, row);
    }

    fclose(file);
}

void *sign(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255/ threads;
    for(int r = 0; r < H; r++)
    {
        for(int c = 0; c < W; c++)
        {
            if(image[r][c] / size == index)
            {
                histogram[index][image[r][c]]++;
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 10e6 + (end.tv_nsec - start.tv_nsec) * 10e-3;
    return (void*)time;
}

void *block(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = W / threads;
    for(int c = index * size; c < (index + 1) * size; c++)
    {
        for(int r = 0; r < H; r++)
        {
            histogram[index][image[r][c]]++;
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 10e6 + (end.tv_nsec - start.tv_nsec) * 10e-3;
    return (void*)time;

}

void *interleaved(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
     for(int c = index ; c < W; c += threads)
    {
        for(int r = 0; r < H; r++)
        {
            histogram[index][image[r][c]]++;
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 10e6 + (end.tv_nsec - start.tv_nsec) * 10e-3;
    return (void*)time;

}

void *write_to_histogram(void* arg)
{
    struct arg_struct *args = arg;
    if(strcmp(args->mode, "sign") == 0)
    {
        return sign(args->arg1);
    }
    else if(strcmp(args->mode, "block") == 0)
    {
        return block(args->arg1);
    }
    else if(strcmp(args->mode, "interleaved") == 0)
    {
        return interleaved(args->arg1);
    }
    else
    {
        error_and_exit("Wrong command");
    }
    
}

void save_histogram(char* output_file)
{
    FILE *file = fopen(output_file, "w");
    for(int i = 0; i < 256; i++)
    {
        int sum = 0;
        for(int j = 0; j < threads; j++)
        {
            sum += histogram[j][i];
        }
        fprintf(file, "Color: %d \tSum:%d\n", i, sum);
        // printf("Color: %d \tSum:%d\n", i, sum);
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    threads = atoi(argv[1]);
    char *mode = argv[2];
    char *pgm_file = argv[3];
    char *output_file = argv[4];
    char buffer[256];
    load_image_to_array(pgm_file);

    FILE *times = fopen("Times.txt", "a");
    fprintf(times, "Mode: %s\tThreads: %d\n", mode, threads);
    printf("Mode: %s\tThreads: %d\n", mode, threads);

    histogram = calloc(threads, sizeof(int*));
    for(int i = 0; i < threads; i++)
        histogram[i] = calloc(256, sizeof(int));
    
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(threads, sizeof(pthread_t));
    struct arg_struct *args = calloc(threads, sizeof(struct arg_struct));
    for(int i = 0; i < threads; i++)
    {
        struct arg_struct arg;
        arg.arg1 = i;
        arg.mode = mode;

        args[i] = arg;

        pthread_create(&thread_ids[i], NULL, write_to_histogram, (void*)&args[i]);
    }

    for(int i = 0; i < threads; i++)
    {
        double *time2;
        pthread_join(thread_ids[i], (void *)&time2);
        fprintf(times, "Thread %d\t %lf microseconds\n", i, *time2);
        printf("Thread %d\t %lf microseconds\n", i, *time2);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 10e6 + (end.tv_nsec - start.tv_nsec) * 10e-3;
    fprintf(times, "TOTAL TIME:\t %lf\n\n", time);
    printf("TOTAL TIME:\t %lf\n\n", time);

    save_histogram(output_file);

    for(int i = 0; i < H; i++)
        free(image[i]);
    free(image);

    for(int i = 0; i < threads; i++)
        free(histogram[i]);
    free(histogram);
    
    fclose(times);
    
    return 0;
}