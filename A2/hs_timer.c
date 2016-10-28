#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main()
{
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer

    clock_gettime(CLOCK_REALTIME, &start);
    sleep(1);
    clock_gettime(CLOCK_REALTIME, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_REALTIME Measured: %llu\n",result);

    clock_gettime(CLOCK_MONOTONIC, &start);
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_MONOTONIC Measured: %llu\n",result);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    sleep(1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    sleep(1);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

    result=timespecDiff(&stop,&start);

    printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);


}