#ifndef _MAIN_H
#define _MAIN_H

#include <pthread.h>

// Define global variables accessible to all threads
long matrix_size;
long num_threads;
char** matrix_x;
char** matrix_y;
pthread_mutex_t mutex;
long sum;

#endif // _MAIN_H
