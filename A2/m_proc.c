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

#define MAX_ITER 10
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
    int i,num;
    
    int first_pipe[2];
    int second_pipe[2];

    int test = 0;
    
    char in_str[2] = "A";
    char read_content[100];

    cpu_set_t set;
    struct sched_param prio_param;
    int prio_max;

    int shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    pid_t pid;

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
    result = 0;
    for (i = 0; i < MAX_ITER;i++){
        if (pipe(first_pipe) == -1)
            perror("Error: first pipe.");
        if (pipe(second_pipe) == -1)
            perror("Error: second pipe.");

        if ((pid = fork()) == -1)
            perror("Error: fork().");
        else if (pid == 0){
            //Child process
            close(first_pipe[1]);
            read(first_pipe[0], read_content, sizeof(read_content));
            // printf("reading from fisrt pipe: %s\n", read_content);
            clock_gettime(CLOCK_MONOTONIC, &stop);

            close(second_pipe[0]);
            write(second_pipe[1], &stop, sizeof(stop));

            _exit(0);
        }else{
            //parent process
            close(first_pipe[0]);
            clock_gettime(CLOCK_MONOTONIC, &start);
            write(first_pipe[1], in_str, strlen(in_str)+1);

            wait(NULL);
            
            close(second_pipe[1]);
            read(second_pipe[0], &stop, sizeof(stop));
            result+=timespecDiff(&stop,&start);
            // printf("reading from second pipe: %s\n", read_content);
            
            close(second_pipe[0]);
            close(first_pipe[1]);
        }
    }
    for (i = 0; i < MAX_ITER;i++){
        clock_gettime(CLOCK_MONOTONIC, &start);
        clock_gettime(CLOCK_MONOTONIC, &stop);
        result -= timespecDiff(&stop, &start); 
    }
    printf("CLOCK_MONOTONIC Measured: %llu\n",result/(MAX_ITER));

    return 0;
}