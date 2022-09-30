/*******************************************************************************
 *
 *  Filter a large array based on the values in a second array.
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <omp.h>
#include <math.h>
#include <assert.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

/* Example filter sizes */
#define DATA_LEN 512 * 512 * 256
#define FILTER_LEN 256

char HEADER[] = "\033[95m";
char OKBLUE[] = "\033[94m";
// OKCYAN = '\033[96m'
// OKGREEN = '\033[92m'
char WARNING[] = "\033[93m";
char FAIL[] = "\033[91m";
char ENDC[] = "\033[0m";
// BOLD = '\033[1m'
// UNDERLINE = '\033[4m'

/* Subtract the `struct timeval' values X and Y,
    storing the result in RESULT.
    Return 1 if the difference is negative, otherwise 0. */
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

/* Function to apply the filter with the filter list in the outside loop */
float serialFilterFirst(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    /* for all elements in the filter */
    for (int y = 0; y < filter_len; y++)
    {
        /* for all elements in the data */
        for (int x = 0; x < data_len; x++)
        {
            /* it the data element matches the filter */
            if (input_array[x] == filter_list[y])
            {
                /* include it in the output */
                output_array[x] = input_array[x];
            }
        }
    }

    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    float total_time = (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000;
    printf("Serial filter first took %f seconds \n", total_time);

    return total_time;
}

/* Function to apply the filter with the filter list in the outside loop */
float serialDataFirst(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    /* for all elements in the data */
    for (int x = 0; x < data_len; x++)
    {
        /* for all elements in the filter */
        for (int y = 0; y < filter_len; y++)
        {
            /* it the data element matches the filter */
            if (input_array[x] == filter_list[y])
            {
                /* include it in the output */
                output_array[x] = input_array[x];
            }
        }
    }

    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    float total_time = (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000;
    printf("Serial data first took %f seconds \n", total_time);
    return total_time;
}

/* Function to apply the filter with the filter list in the outside loop */
float parallelFilterFirst(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list, int threads = -1)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    //********************
    // Your code here.
    //********************
    if (threads != -1)
    {
        omp_set_num_threads(threads);
    }

#pragma omp parallel for
    for (int y = 0; y < filter_len; y++)
    {
        /* for all elements in the data */
        for (int x = 0; x < data_len; x++)
        {
            /* it the data element matches the filter */
            if (input_array[x] == filter_list[y])
            {
                /* include it in the output */
                output_array[x] = input_array[x];
            }
        }
    }

    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    float total_time = (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000;
    printf("Parallel filter first took %f seconds \n", total_time);
    return total_time;
}

/* Function to apply the filter with the filter list in the inside loop */
float parallelDataFirst(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list, int threads = 1)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    //********************
    // Your code here.
    //********************
    if (threads != -1)
    {
        omp_set_num_threads(threads);
    }
#pragma omp parallel for
    for (int x = 0; x < data_len; x++)
    {
        /* for all elements in the filter */
        for (int y = 0; y < filter_len; y++)
        {
            /* it the data element matches the filter */
            if (input_array[x] == filter_list[y])
            {
                /* include it in the output */
                output_array[x] = input_array[x];
            }
        }
    }
    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    float total_time = (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000;
    printf("Parallel data first took %f seconds \n", total_time);
    return total_time;
}

/* version of parallelDataFirst that unrolls the outer loop 8 times */
void parallelDataFirstUnrolledData(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list, int threads = -1)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    // Your code here.
    if (threads != -1)
    {
        omp_set_num_threads(threads);
    }
#pragma omp parallel for
    for (int x = 0; x < data_len; x = x + 8)
    {
        /* for all elements in the filter */
        for (int y = 0; y < filter_len; y++)
        {
            // Unrolling
            if (input_array[x + 0] == filter_list[y])
            {
                output_array[x + 0] = input_array[x + 0];
            }
            if (input_array[x + 1] == filter_list[y])
            {
                output_array[x + 1] = input_array[x + 1];
            }
            if (input_array[x + 2] == filter_list[y])
            {
                output_array[x + 2] = input_array[x + 2];
            }
            if (input_array[x + 3] == filter_list[y])
            {
                output_array[x + 3] = input_array[x + 3];
            }
            if (input_array[x + 4] == filter_list[y])
            {
                output_array[x + 4] = input_array[x + 4];
            }
            if (input_array[x + 5] == filter_list[y])
            {
                output_array[x + 5] = input_array[x + 5];
            }
            if (input_array[x + 6] == filter_list[y])
            {
                output_array[x + 6] = input_array[x + 6];
            }
            if (input_array[x + 7] == filter_list[y])
            {
                output_array[x + 7] = input_array[x + 7];
            }
        }
    }
    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);
    printf("DFUD data first unrolled data took %f seconds \n", (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000);
    return;
}

/* version of parallelFilterFirst that unrolls the outer loop 8 times */
void parallelFilterFirstUnrolledFilter(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list, int threads = -1)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    // Your code here.
    if (threads != -1)
    {
        omp_set_num_threads(threads);
    }
#pragma omp parallel for
    for (int y = 0; y < filter_len; y = y + 8)
    {
        /* for all elements in the data */
        for (int x = 0; x < data_len; x++)
        {
            // Unrolling
            if (input_array[x] == filter_list[y + 0])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 1])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 2])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 3])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 4])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 5])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 6])
            {
                output_array[x] = input_array[x];
            }
            if (input_array[x] == filter_list[y + 7])
            {
                output_array[x] = input_array[x];
            }
        }
    }
    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    printf("FFUF filter first unrolled filter took %f seconds \n", (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000);
    return;
}

/* version of parallelDataFirst that unrolls the inner loop 8 times */
void parallelDataFirstUnrolledFilter(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    // Your code here.

    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);
    printf("DFUF data first unrolled filter took %f seconds \n", (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000);
    return;
}

/* version of parallelFilterFirst that unrolls the inner loop 8 times */
void parallelFilterFirstUnrolledData(int data_len, unsigned int *input_array, unsigned int *output_array, int filter_len, unsigned int *filter_list, int threads = -1)
{
    /* Variables for timing */
    struct timeval ta, tb, tresult;

    /* get initial time */
    gettimeofday(&ta, NULL);

    // Your code here.
    if (threads != -1)
    {
        omp_set_num_threads(threads);
    }
#pragma omp parallel for
    for (int y = 0; y < filter_len; y++)
    {
        /* for all elements in the data */
        for (int x = 0; x < data_len; x = x + 8)
        {
            // Unrolling
            if (input_array[x + 0] == filter_list[y])
            {
                output_array[x + 0] = input_array[x + 0];
            }
            if (input_array[x + 1] == filter_list[y])
            {
                output_array[x + 1] = input_array[x + 1];
            }
            if (input_array[x + 2] == filter_list[y])
            {
                output_array[x + 2] = input_array[x + 2];
            }
            if (input_array[x + 3] == filter_list[y])
            {
                output_array[x + 3] = input_array[x + 3];
            }
            if (input_array[x + 4] == filter_list[y])
            {
                output_array[x + 4] = input_array[x + 4];
            }
            if (input_array[x + 5] == filter_list[y])
            {
                output_array[x + 5] = input_array[x + 5];
            }
            if (input_array[x + 6] == filter_list[y])
            {
                output_array[x + 6] = input_array[x + 6];
            }
            if (input_array[x + 7] == filter_list[y])
            {
                output_array[x + 7] = input_array[x + 7];
            }
        }
    }
    /* get initial time */
    gettimeofday(&tb, NULL);

    timeval_subtract(&tresult, &tb, &ta);

    printf("FFUD filter first unrolled data took %f seconds \n", (double)tresult.tv_sec + (double)tresult.tv_usec / 1000000);
    return;
}

void checkData(unsigned int *serialarray, unsigned int *parallelarray)
{
    for (int i = 0; i < DATA_LEN; i++)
    {
        if (serialarray[i] != parallelarray[i])
        {
            printf("%sData check failed offset %d%s\n", FAIL, i, ENDC);
            printf("%sSerial data %d. Parallel data %d%s\n", FAIL, serialarray[i], parallelarray[i], ENDC);
            return;
        }
    }
}

void experiment1(int total_iterations, unsigned int *input_array, unsigned int *output_array, unsigned int *serial_array, unsigned int *filter_list)
{
    //*********************************************************
    // Experiment 1: Execute at a variety of filter lengths
    //*********************************************************
    float exec_time;
    char data[100];

    int total_runs = total_iterations;
    std::string header = "run,time,filter_size,type,exec_type,threads\n";
    std::ofstream results_file("./results/experiment1.csv");
    results_file << header;
    printf("Experiment 1 headers: %s\n", header.c_str());

    for (int run = 0; run < total_runs; run++)
    {
        std::cout << "Executing run " << run << std::endl;

        for (int filter_len = 8; filter_len <= 256; filter_len *= 2)
        {
            printf("%sCollecting timing data for filter of length %d%s\n", OKBLUE, filter_len, ENDC);

            exec_time = serialDataFirst(DATA_LEN, input_array, serial_array, filter_len, filter_list);

            sprintf(data, "%d,%0.6f,%d,datafirst,serial,1", run, exec_time, filter_len);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);

            exec_time = serialFilterFirst(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);

            sprintf(data, "%d,%0.6f,%d,filterfirst,serial,1", run, exec_time, filter_len);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);

            //********************
            // Parallel execution
            exec_time = parallelDataFirst(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);

            sprintf(data, "%d,%0.6f,%d,datafirst,parallel,12", run, exec_time, filter_len);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);

            exec_time = parallelFilterFirst(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);

            sprintf(data, "%d,%0.6f,%d,filterfirst,parallel,12", run, exec_time, filter_len);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);

            //********************
            // Unrolled for loops

            //    parallelDataFirstUnrolledFilter ( DATA_LEN, input_array, output_array, filter_len, filter_list );
            //    checkData ( serial_array, output_array );
            //    memset ( output_array, 0, DATA_LEN );

            parallelFilterFirstUnrolledFilter(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);
            memset(output_array, 0, DATA_LEN);

            parallelDataFirstUnrolledData(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);
            memset(output_array, 0, DATA_LEN);

            parallelFilterFirstUnrolledData(DATA_LEN, input_array, output_array, filter_len, filter_list);
            checkData(serial_array, output_array);
            memset(output_array, 0, DATA_LEN);

            //********************
            // Reset serial array
            memset(serial_array, 0, DATA_LEN);
        }
    }
    results_file.close();
}

void experiment2(int total_iterations, unsigned int *input_array, unsigned int *output_array, unsigned int *serial_array, unsigned int *filter_list)
{
    //*********************************************************
    // Experiment 2: Loop parallel execution
    //*********************************************************
    float exec_time;
    char data[100];

    int total_runs = total_iterations;
    std::string header = "run,time,filter_size,type,exec_type,threads\n";
    std::ofstream results_file("./results/experiment2.csv");
    results_file << header;
    printf("Experiment 2 headers: %s\n", header.c_str());

    int thread_numbers[] = {1, 2, 4, 8, 12, 16, 20};
    // int thread_numbers[] = {12, 16};
    int filter_size = 256;
    // int run = 1;

    // Serial execution
    // exec_time = serialDataFirst(DATA_LEN, input_array, serial_array, filter_size, filter_list);
    // memset(data, 0, sizeof(data));
    // memset(output_array, 0, DATA_LEN);

    for (int run = 0; run < total_runs; run++)
    {
        printf("%sIteration %d%s\n", OKBLUE, run, ENDC);

        for (const int th : thread_numbers)
        {
            std::cout << " Run code with " << th << " threads" << std::endl;

            // Parallel execution
            exec_time = parallelDataFirst(DATA_LEN, input_array, output_array, filter_size, filter_list, th);

            sprintf(data, "%d,%0.6f,%d,datafirst,parallel,%d", run, exec_time, filter_size, th);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);

            exec_time = parallelFilterFirst(DATA_LEN, input_array, output_array, filter_size, filter_list, th);

            sprintf(data, "%d,%0.6f,%d,filterfirst,parallel,%d", run, exec_time, filter_size, th);
            results_file << data << std::endl;
            memset(data, 0, sizeof(data));
            memset(output_array, 0, DATA_LEN);
        }
    }
}

int main(int argc, char **argv)
{
    /* loop variables */
    int x, y;

    /* Create matrixes */
    unsigned int *input_array;
    unsigned int *serial_array;
    unsigned int *output_array;
    unsigned int *filter_list;

    /* Initialize the data. Values don't matter much. */
    posix_memalign((void **)&input_array, 4096, DATA_LEN * sizeof(unsigned int));
    for (x = 0; x < DATA_LEN; x++)
    {
        input_array[x] = x % 2048;
    }

    /* start with an empty *all zeros* output array */
    posix_memalign((void **)&output_array, 4096, DATA_LEN * sizeof(unsigned int));
    memset(output_array, 0, DATA_LEN);
    posix_memalign((void **)&serial_array, 4096, DATA_LEN * sizeof(unsigned int));
    memset(serial_array, 0, DATA_LEN);

    /* Initialize the filter. Values don't matter much. */
    filter_list = (unsigned int *)malloc(FILTER_LEN * sizeof(unsigned int));
    for (y = 0; y < FILTER_LEN; y++)
    {
        filter_list[y] = y;
    }

    //*********************************************************
    // Experiment 1: Execute at a variety of filter lengths
    //*********************************************************
    experiment1(1, input_array, output_array, serial_array, filter_list);

    //*********************************************************
    // Experiment 2: Loop parallel execution
    //*********************************************************
    // experiment2(10, input_array, output_array, serial_array, filter_list);

    //*********************************************************
    // Experiment 2: Loop unrolling
    //*********************************************************
}
