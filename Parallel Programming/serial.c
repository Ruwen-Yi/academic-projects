#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// This is serial algorithm
int main(int argc, char* argv[])
{
    // Check that 1 parameters are given, exiting with error otherwise
    if (argc!=2)
    {
        printf("1 parameters required\n");
        exit(0);
    }

    int arr_size = (int)(strtol(argv[1], NULL, 10));

    // Create the array
    int *array = (int*)malloc(sizeof(int)*arr_size);
    int j = arr_size;

    // Fill in the array
    for(int i = 0; i < arr_size; i++)
    {
        array[i] =  j % 10;
        j--;
    }

    // Get execution time
    struct timespec timer_begin, timer_end;

    clock_gettime(CLOCK_REALTIME, &timer_begin);
    for (int j = 1; j <= arr_size; j++)
    {
        if (j % 2 == 0)
        {
            // Sort elements of each even-odd indexed pair
            for (int i = 0; i <= arr_size-2; i = i+2)
            {
                if (array[i] > array[i+1])
                {
                    int tmp = array[i] ;
                    array[i] = array[i+1] ;
                    array[i+1] = tmp ;
                }
            }
        }
        else
        {
            // Sort elements of each odd-even indexed pair
            for (int i = 1; i <= arr_size-2; i = i+2)
            {
                if (array[i] > array[i+1])
                {
                    int tmp = array[i] ;
                    array[i] = array[i+1] ;
                    array[i+1] = tmp ;
                }
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &timer_end);

    // calculate elapsed time for serial program
    long seconds = timer_end.tv_sec - timer_begin.tv_sec;
    long nanoseconds = timer_end.tv_nsec - timer_begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    // Print out the time and array size
    printf("%d %f\n", arr_size ,elapsed);

    free(array);
    return 0;
}


