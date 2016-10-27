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
pid_t getpid( void )
{
    return syscall( __NR_getpid );
}

int main()
{
    /*********************************************************************************************
        To make sure context-switching processes are located on the same processor :
        1. Bind a process to a particular processor using sched_setaffinity.    
        2. To get the maximum priority value (sched_get_priority_max) that can be used with 
           the scheduling algorithm identified by policy (SCHED_FIFO).** 
        **********************************************************************************************/

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

    /*****************************************************************************************************
        1. To create a pipe for a fork, the parent and child processes use pipe to read and write, 
           read and write string, using this for context switch.
        2. The parent process first to get the current timestamp (gettimeofday), then write to the pipe,. 
           Then the child should be read in from the back, 
           then the child process to write string, the parent process reads. 
           After the child process to get the current timestamp. 
           This is roughly the difference between two timestamps n * 2 times the context switch time.
    *******************************************************************************************************/

    int     ret=-1;
    int     firstpipe[2];
    int     secondpipe[2];
    int     timepipe[2];
    int     nbytes;
    char    string[] = "Hello, world!\n";
    char    temp[] = "Sumit Gemini!\n";
    char    readbuffer[80];
    char    tempbuffer[80];
    int     i=0;
    struct  timeval start,end;

    // Create an unnamed first pipe
    if (pipe(firstpipe) == -1) {
        fprintf(stderr, "parent: Failed to create pipe\n");
        return -1;
    }

    // create an unnamed Second pipe
    if (pipe(secondpipe) == -1) {
        fprintf(stderr, "parent: Failed to create second pipe\n");
        return -1;
    }

    // Create an unnamed time pipe which will share in order to show time spend between processes
    if (pipe(timepipe) == -1) {
        fprintf(stderr, "parent: Failed to create time pipe\n");
        return -1;
    }


    if((ret=fork())==-1)
        perror("fork");
    else if(ret==0){
        int n=-1;
        printf("Child  ----> %d\n",getpid());

        for(n=0;n<5;n++)
        {
            nbytes = read(firstpipe[0], readbuffer, sizeof(readbuffer));
            printf("Received string: %s", readbuffer);
            write(secondpipe[1], temp, strlen(temp)+1);
        }

        gettimeofday(&end,0);
        n = sizeof(struct timeval);

        if( write(timepipe[1],&end,sizeof(struct timeval)) != n )
        {
            fprintf(stderr, "child: Failed to write in time pipe\n");
            exit(EXIT_FAILURE);
        }

    }
    else{
        double switch_time;
        int n=-1;
        printf("Parent  ----> %d\n",getpid());
        gettimeofday(&start,0);
        /* Read in a string from the pipe */

        for(n=0;n<5;n++){
            write(firstpipe[1], string, strlen(string)+1);
            read(secondpipe[0], tempbuffer, sizeof(tempbuffer));
            printf("Received temp: %s", tempbuffer);
        }

        n = sizeof(struct timeval);
        if( read(timepipe[0],&end,sizeof(struct timeval)) != n ){
            fprintf(stderr, "Parent: Failed to read from time pipe\n");
            exit(EXIT_FAILURE);
        }

        wait(NULL);
        switch_time = ((end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec))/1000.0;
        printf("context switch between two processes: %0.6lfms\n",switch_time/(5*2));
    }   

    return 0;
}