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
#include <pthread.h>

#define MAX_ITER 100000
void thread1_func(){

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
    int i,num = 0;
    pthread_t tid; /* the thread identifier */
    pthread_attr_t attr;

    int test = 0;

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

    pthread_attr_init(&attr);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    
    printf("CLOCK_MONOTONIC Measured: %llu\n",result/(2*MAX_ITER));

    return 0;
}