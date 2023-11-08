#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void check_err(cl_int err, char* err_output) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error %i: %s\n", (int)err, err_output);
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    // Check that 2 parameters are given, exiting with error otherwise
    if (argc!=3)
    {
        printf("2 parameters required\n");
        exit(0);
    }

    // Get information from input
    const int LIST_SIZE = (int)(strtol(argv[1], NULL, 10));
    int user_local_size = (int)(strtol(argv[2], NULL, 10));

    // Create the array
    int *array = (int*)malloc(sizeof(int)*LIST_SIZE);

    // Get length of the array
    int arr_size = LIST_SIZE;
    int *arr_size_ptr = &arr_size;

    // Create a mark to indicate an odd-even or even-odd sort
    int mark = 0;
    int* mark_ptr = &mark;

    // Fill in the array
    int j = arr_size;
    for(int i = 0; i < arr_size; i++) {
        array[i] =  j % 10;
        j--;
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // variable used for OpenCL error handling
    cl_int err;

    // Get the platform ID
    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    err = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    check_err(err, "clGetPlatformIDs");

    // Get the first GPU device associated with the platform
    cl_device_id device_id;
    cl_uint ret_num_devices;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    check_err(err, "clGetPlatformIDs");

    // Get maximum work group size
    size_t max_work_group_size;
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
    check_err(err, "clGetDeviceInfo");

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    check_err(err, "clCreateContext");

    // Create Queue with Profiling enabled
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
    check_err(err, "clCreateCommandQueue");

    // Create memory buffers on the device for array, array size and mark
    cl_mem arr_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, LIST_SIZE * sizeof(int), NULL, &err);
    check_err(err, "clCreateBuffer a");
    cl_mem size_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int*), NULL, &err);
    check_err(err, "clCreateBuffer b");
    cl_mem mark_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int*), NULL, &err);
    check_err(err, "clCreateBuffer c");

    // Copy array, arr_size_ptr and mark_ptr to their respective memory buffers
    err = clEnqueueWriteBuffer(command_queue, arr_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), array, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer a");
    err = clEnqueueWriteBuffer(command_queue, size_mem_obj, CL_TRUE, 0, sizeof(int*), arr_size_ptr, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer b");
    err = clEnqueueWriteBuffer(command_queue, mark_mem_obj, CL_TRUE, 0, sizeof(int*), mark_ptr, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer c");

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    check_err(err, "clCreateProgramWithSource");

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    check_err(err, "clBuildProgram");

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "odd_even_sort", &err);
    check_err(err, "clCreateKernel");

    // Set the arguments of the kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arr_mem_obj);
    check_err(err, "clSetKernelArg a");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&size_mem_obj);
    check_err(err, "clSetKernelArg b");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mark_mem_obj);
    check_err(err, "clSetKernelArg c");

    // Ensure to have executed all enqueued tasks
    err = clFinish(command_queue);
    check_err(err, "clFinish");

    // Decide local size and global size
    size_t local_work_size = ((size_t)user_local_size > max_work_group_size)? max_work_group_size : (size_t)user_local_size;
    size_t num_work_groups = (LIST_SIZE + local_work_size - 1) / local_work_size; // divide rounding up
    size_t global_work_size = num_work_groups * local_work_size;

    // Launch Kernel linked to the event and get the execution time
    double total_time = 0.0;

    for (int j = 1; j <= arr_size; j++)
    {
        cl_event event, write_event;

        // Write to the "mark" memory buffer to indicate an odd-even or even-odd sort
        mark = (j % 2 == 0)? 0 : 1;
        err = clEnqueueWriteBuffer(command_queue, mark_mem_obj, CL_TRUE, 0, sizeof(int*), mark_ptr, 0, NULL, &write_event);
        check_err(err, "clEnqueueWriteBuffer c");

        // Ensure writing to buffer is finished
        err = clWaitForEvents(1, &write_event);
        check_err(err, "clWaitForEvents");

        // Execute kernel function
        err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &event);
        check_err(err, "clEnqueueNDRangeKernel");

        // Ensure kernel execution is finished
        err = clWaitForEvents(1, &event);
        check_err(err, "clWaitForEvents");

        // Get the profiling data
        cl_ulong time_start, time_end;

        err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        check_err(err, "clGetEventProfilingInfo Start");
        err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        check_err(err, "clGetEventProfilingInfo End");
        total_time += time_end - time_start;
    }

    // Read the memory from "array" memory buffer
    int *sorted_array = (int*)malloc(sizeof(int)*LIST_SIZE);
    err = clEnqueueReadBuffer(command_queue, arr_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), sorted_array, 0, NULL, NULL);
    check_err(err, "clEnqueueReadBuffer a");

    // Display GPU execution time and array size
    printf("%d %0.3f\n", arr_size, (total_time / 1000000000.0));

    // Clean up
    err = clFlush(command_queue);
    err = clFinish(command_queue);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(arr_mem_obj);
    err = clReleaseMemObject(size_mem_obj);
    err = clReleaseMemObject(mark_mem_obj);
    err = clReleaseCommandQueue(command_queue);
    err = clReleaseContext(context);
    free(array);
    free(sorted_array);
    return 0;
}
