__kernel void odd_even_sort(__global int *array, __global int *arr_size_ptr, __global int *mart_ptr) {

    // Get the index of the current element to be processed
    int i = get_global_id(0);
    int len = *arr_size_ptr;
    int mark = *mart_ptr;

    // Sort elements of even-odd or odd_even indexed pairs
    if (i % 2 == mark)
    {
        if (i <= len-2 && array[i] > array[i+1])
        {
            int tmp = array[i] ;
            array[i] = array[i+1] ;
            array[i+1] = tmp ;
        }
    }
}
