#include "main.h"
#include "serial.h"

#include <stdlib.h>

void serial()
{
    for (long x=0; x<matrix_size; x++)
        for (long y=0; y<matrix_size; y++)
            sum += matrix_x[x][y] * matrix_y[y][x];
}
