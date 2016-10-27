#ifndef JOB_H
#define JOB_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct Job {
    pid_t pid;
    pid_t pgid;
    unsigned long long job_no;
    int is_fg;
    int is_stop;
    struct Job* next;
    char arg[1024];
};

struct Job_Container{
    int total;
    struct Job* head_job;
    struct Job* current_job;
    struct Job* tail_job; 
} Shell_job_c;

int add_job(pid_t pid, int current, char * argument);
int remove_job(pid_t pid);
void wait_job();
void jobs();
void bg_job(pid_t pid);
void fg_job(pid_t pid);
struct Job * find_job_by_pid(pid_t pid);
struct Job * find_job_by_no(int i);
int find_job_no(struct Job * tmp);
void print_job(struct Job* tmp, int i);

#endif