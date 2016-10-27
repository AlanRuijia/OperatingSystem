#include "mysignal.h"

void sig_int(){
    pid_t pid;
    if (Shell_job_c.current_job == NULL)
        return; 
    pid = Shell_job_c.current_job -> pid;
    Shell_job_c.current_job -> is_stop = 3;
    signal(SIGINT, SIG_DFL);
    kill(pid, SIGINT);
    signal(SIGINT, &sig_int);
    Shell_job_c.current_job = NULL;
}

void sig_stp(){
    pid_t pid;
    struct Job * tmp;
    if (Shell_job_c.current_job == NULL)
        return;
    pid = Shell_job_c.current_job -> pid;
    // printf("Entering!\n");
    Shell_job_c.current_job -> is_stop = 1;
    signal(SIGTSTP, SIG_DFL);
    kill(pid, SIGTSTP);
    signal(SIGTSTP, &sig_stp);
    tmp = find_job_by_pid(pid);
    print_job(tmp,find_job_no(tmp));
    Shell_job_c.current_job = NULL;
}

void sig_quit(){
    pid_t pid;
    struct Job * tmp;
    if (Shell_job_c.current_job == NULL)
        return;
    pid = Shell_job_c.current_job -> pid;
    bg_job(pid);
    Shell_job_c.current_job -> is_stop = 4;
    signal(SIGQUIT, SIG_DFL);
    kill(pid, SIGQUIT);
    signal(SIGQUIT, &sig_quit);
    tmp = find_job_by_pid(pid);
    print_job(tmp,find_job_no(tmp));
    Shell_job_c.current_job = NULL;
}