#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <time.h>

#define MAX_ITER 100000000
void dummy_func(){

}

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
    double final_result;
    int i;

    cpu_set_t set;
    struct sched_param prio_param;
    int prio_max;

    CPU_ZERO( &set );
    CPU_SET( 0, &set );
    memset(&prio_param,0,sizeof(struct sched_param));

    if (sched_setaffinity( getpid(), sizeof( cpu_set_t ), &set )){
        perror( "sched_setaffinity" );
        exit(EXIT_FAILURE);
    }

    if( (prio_max = sched_get_priority_max(SCHED_FIFO)) < 0 ){
        perror("sched_get_priority_max");
    }

    prio_param.sched_priority = prio_max;
    if( sched_setscheduler(getpid(),SCHED_FIFO,&prio_param) < 0 )
    {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }

    // result = 0;
    // for (i = 0;i < MAX_ITER;i++){
    //     clock_gettime(CLOCK_REALTIME, &start);   
    //     dummy_func();
    //     clock_gettime(CLOCK_REALTIME, &stop);
    //     result+=timespecDiff(&stop,&start);
    // }
    // result/=MAX_ITER;
    // printf("CLOCK_REALTIME Measured: %llu\n",result);
    
    // result = 0;
    // for (i = 0;i < MAX_ITER;i++){
    //     clock_gettime(CLOCK_MONOTONIC, &start);
    //     dummy_func();
    //     clock_gettime(CLOCK_MONOTONIC, &stop);
    //     result+=timespecDiff(&stop,&start);
    // }
    // result/=MAX_ITER;
    // printf("CLOCK_MONOTONIC Measured: %llu\n",result);
    
    // result = 0;
    
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // for (i = 0;i < MAX_ITER;i++){
    //     dummy_func();
    // }
    // clock_gettime(CLOCK_MONOTONIC, &stop);
    // result+=timespecDiff(&stop,&start);
    
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // for (i = 0;i < MAX_ITER;i++);
    // clock_gettime(CLOCK_MONOTONIC, &stop);
    // result-=timespecDiff(&stop,&start); 

    // final_result = 1.0*result/MAX_ITER;
    // printf("CLOCK_MONOTONIC Measured: %lf\n",final_result);

    result = 0;
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++){
        dummy_func();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    result+=timespecDiff(&stop,&start);
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    
    result-=timespecDiff(&stop,&start); 
    final_result = 1.0*result/MAX_ITER;
    printf("CLOCK_PROCESS_CPUTIME_ID Measured: %lf\n",final_result);

    result = 0;
    
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++){
        dummy_func();
    }
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
    result+=timespecDiff(&stop,&start);
    
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
    
    result-=timespecDiff(&stop,&start); 
    final_result = 1.0*result/MAX_ITER;
    printf("CLOCK_THREAD_CPUTIME_ID Measured: %lf\n",final_result);
    
    // result = 0;
    // for (i = 0;i < MAX_ITER;i++){
    //     clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    //     dummy_func();
    //     clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
    //     result+=timespecDiff(&stop,&start);
    // }
    // // result/=MAX_ITER;
    // printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);

    return 0;
}