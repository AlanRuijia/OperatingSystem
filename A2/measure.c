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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <linux/futex.h>

#define MAX_ITER 100000

pthread_mutex_t lock;
pthread_cond_t cond_zero;
pthread_cond_t cond_one;
int count;
struct timespec start;
struct timespec stop;

void dummy_func(){

}

void* zero(void* unused)
{
    pthread_mutex_lock(&lock);
    while (count == 0){
        // printf("waiting\n");
        pthread_cond_wait(&cond_zero, &lock);
    }
    // printf("in zero\n");
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    count = 0;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

void* one(void* unused)
{
    pthread_mutex_lock(&lock);
    // printf("in one\n");
    count = 1;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    pthread_cond_signal(&cond_zero);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

int main()
{
    
    unsigned long long result; //64 bit integer
    double final_result;
    int i,num = 0;
    pthread_t tid[2]; /* the thread identifier */
    pthread_attr_t attr[2];

    int test = 0;

    int first_pipe[2];
    int second_pipe[2];
    
    char in_str[2] = "A", in_str2[2] = "B";
    char read_content[10];

    cpu_set_t set;

    pid_t pid;
    int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
    int* futex = shmat(shm_id, NULL, 0);
    *futex = 0xA;

    CPU_ZERO( &set );
    CPU_SET( 0, &set );

    if (sched_setaffinity(getpid(), sizeof( cpu_set_t ), &set )){
        perror( "sched_setaffinity" );
        exit(EXIT_FAILURE);
    }

    //------------------Function call--------------------------------
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
    printf("Function call CLOCK_PROCESS_CPUTIME_ID Measured: %lf\n",final_result);

    //--------------------System call------------------------------
    result = 0;
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++){
        getpid();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    result+=timespecDiff(&stop,&start);
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0;i < MAX_ITER;i++);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    
    result-=timespecDiff(&stop,&start); 
    final_result = 1.0*result/MAX_ITER;
    printf("System call CLOCK_PROCESS_CPUTIME_ID Measured: %lf\n",final_result);

    //-------------------Process context swtich-------------------------------
    result = 0;
    if ((pid = fork()) == -1)
        perror("Error: fork().");
    else if (pid == 0){
        //child process
        for (i = 0; i < MAX_ITER;i++){
            sched_yield();
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                sched_yield();
            }
            *futex = 0xB;
            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                sched_yield();
            }
        }
        return 0;
    }else{
        //parent process
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (i = 0; i < MAX_ITER;i++){
            *futex = 0xA;
            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                sched_yield();
            }
            sched_yield();
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
                sched_yield();
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
        result = timespecDiff(&stop, &start);

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (i = 0;i < MAX_ITER;i++);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
        result-=timespecDiff(&stop,&start); 
        
        printf("Process context swtich CLOCK_MONOTONIC Measured: %llu\n",result/(4*MAX_ITER));
    }

    //--------------------------------------------------
    result = 0;
    for (i = 0; i < MAX_ITER; i++){
        pthread_create(&tid[0], NULL, zero, NULL);
        pthread_create(&tid[1], NULL, one, NULL);
        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
        result += timespecDiff(&stop, &start); 
    }
    for (i = 0; i < MAX_ITER;i++){
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
        result -= timespecDiff(&stop, &start); 
    }
    printf("Thread switch time CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result/(MAX_ITER));

    return 0;
}