#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

// The code basically measures sleep(1) using different counters using function int clock_gettime(clockid_t clk_id, struct timespec *tp).
// And the function timeSpecDiff() measures the difference between two strcut timespec and results returnning in nanoseconds.

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    // a function that take two strcut timespec inputs and output the difference of theses two timespec in nanosecond

    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
    // tv_sec is second and tv_nsec is nanosecond
}   



int main()
{
    struct timespec start;
    struct timespec stop;
    // define two timespec, namely start and stop, to record the start time and stop time of a counter
    unsigned long long result; //64 bit integer

    clock_gettime(CLOCK_REALTIME, &start);
    // record the time before calling sleep(1) using counter CLOCK_REALTIME

    // CLOCK_REALTIME: System-wide realtime clock. 
    // It measures the real time spent to implement the sleep(1), if we change the system time during sleep(1), 
    // it will also change. And it also includes time when the process is actually suspended by CPU.

    sleep(1);
    clock_gettime(CLOCK_REALTIME, &stop);
    // Record the time after calling sleep(1) using counter CLOCK_REALTIME

    result=timespecDiff(&stop,&start);
    // Get the result: result = stop - start

    printf("CLOCK_REALTIME Measured: %llu\n",result);
    // Print the result 

    clock_gettime(CLOCK_MONOTONIC, &start);
    // record the time before calling sleep(1) using counter CLOCK_MONOTONIC

    //CLOCK_MONOTONIC: Clock that cannot be set and represents monotonic time since some unspecified starting point.
    // In the code it measures the monotonic real time spent to implement the sleep(1). 
    // It is similar the the first clock type except it won't change even if we change the system time. 
    // And it also includes time when the process is actually suspended by CPU. 
    // It could be used to measure the time for multi-process program.

    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_MONOTONIC Measured: %llu\n",result);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    // record the time before calling sleep(1) using counter CLOCK_PROCESS_CPUTIME_ID

    // CLOCK_PROCESS_CPUTIME_ID: High-resolution per-process timer from the CPU. 
    // This counter measures the time a specific process spent on CPU, which means that if the process is suspended by CPU, the counter won't increase.
    // It is used to measure time spent in the same process. 

    sleep(1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    // record the time before calling sleep(1) using counter CLOCK_THREAD_CPUTIME_ID

    // CLOCK_THREAD_CPUTIME_ID: Thread-specific CPU-time clock.
    // This counter measures the time a specific thread spent on CPU, which means that if the thread is suspended by CPU, the counter won't increase.
    // It is used to measure time spent in the same thread. 
    sleep(1);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);
}