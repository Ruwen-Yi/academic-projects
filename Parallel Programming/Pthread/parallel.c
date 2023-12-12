#include "main.h"
#include "parallel.h"

#include <stdlib.h>
#include <stdio.h>

// pthread_mutex_t mutex;

static void* thread_routine(void* rank)
{
    long my_rank = (long)rank;
    
    // *** YOUR CODE FOR THE THREAD ROUTINE GOES HERE ***
    
    // assign roughly the same amount of work to each thread
    long my_workload = matrix_size / num_threads;
    long my_first_task = my_rank * my_workload;
    long my_last_task = my_first_task + my_workload - 1;
    long local_sum = 0;

    //each thread compute its local sum
    for (long x = my_first_task; x <= my_last_task; x++)
    {
    	for (long y = 0; y <= matrix_size - 1; y++)
    	{
            local_sum += matrix_x[x][y] * matrix_y[y][x];
    	}
    }

    // if matrix_size is not divisible by the number of threads
    // assign the remainder tasks to threads
    long remainder = matrix_size % num_threads;
    if (remainder != 0)
    {
    	if (my_rank < remainder)
    	{
    		long first_remained = my_workload * num_threads;
    		long my_task = (first_remained + my_rank);
    		long x = my_task;

    		for (long y = 0; y <= matrix_size - 1; y++)
	    	{
	            local_sum += matrix_x[x][y] * matrix_y[y][x];
	    	}
    	}
    }

    // critial section using a mutex
    // threads update the global sum one by one
    pthread_mutex_lock(&mutex);
    sum += local_sum ;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void parallel()
{
    // *** YOUR CODE FOR THE THREAD GENERATION GOES HERE ***
	long thread;
	pthread_t* thread_handles;

	// allocate memory for pthread_t object
	thread_handles = malloc(num_threads*sizeof(pthread_t));

	// initialize mutex
	pthread_mutex_init(&mutex, 0);

    // create threads
    for (thread = 0; thread < num_threads; thread++)
    {
    	pthread_create(&thread_handles[thread], NULL,
    		thread_routine, (void*) thread);
    }

    // join all the threads
    for (thread = 0; thread < num_threads; thread++)
    {
    	pthread_join(thread_handles[thread], NULL);
    }

    // destory mutex
    pthread_mutex_destroy(&mutex);

    // free memory of pthread_t object
    free(thread_handles);
}
