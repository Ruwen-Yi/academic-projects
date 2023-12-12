#include "main.h"
#include "serial.h"
#include "parallel.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    struct timespec timer_begin, timer_end;

    // Check that 2 parameters are given, exiting with error otherwise
    if (argc!=3)
    {
        printf("2 parameters required\n");
        exit(0);
    }

    // Read from command line: matrix_size, num_threads
    matrix_size = strtol(argv[1], NULL, 10);
    num_threads = strtol(argv[2], NULL, 10);

    // Check that matrix_size is positive, exiting with error otherwise
    if (matrix_size<1)
    {
        printf("Matrix size must a be positive number\n");
        exit(0);
    }

    // Check that num_threads is non-negative, exiting with error otherwise
    if (num_threads<0)
    {
        printf("Number of threads must be non-negative\n");
        exit(0);
    }

    // Generate matrices x and y pseudorandomly filled with 1s and -1s
    // using the xorshift pseudorandom number generator = much faster than rand()
    u_int32_t randomstate=0xC0DEF00D;
    matrix_x = (char**) malloc(matrix_size * sizeof(char*));
    matrix_y = (char**) malloc(matrix_size * sizeof(char*));
    for (long x=0; x<matrix_size; x++)
    {
        matrix_x[x] = (char*) malloc(matrix_size * sizeof(char));
        matrix_y[x] = (char*) malloc(matrix_size * sizeof(char));
        for (long y=0; y<matrix_size; y++)
        {
            matrix_x[x][y] = (randomstate & 1) * 2 - 1;
            matrix_y[x][y] = (randomstate >> 1 & 1) * 2 - 1;
            randomstate ^= randomstate << 13;
            randomstate ^= randomstate >> 17;
            randomstate ^= randomstate << 5;
        }
    }

    clock_gettime(CLOCK_REALTIME, &timer_begin);

    // Calculate sum of diagonal of product of matrices x and y using either serial or parallel algorithm
    sum = 0;
    if (num_threads==0)
        serial();
    else
        parallel();

    // calculate elapsed time
    clock_gettime(CLOCK_REALTIME, &timer_end);
    long seconds = timer_end.tv_sec - timer_begin.tv_sec;
    long nanoseconds = timer_end.tv_nsec - timer_begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    // Output final sum, matrix size, number of threads used, and time elapsed
    printf("%ld, %ld, %ld, %f\n", sum, matrix_size, num_threads, elapsed);

    return 0;
}
