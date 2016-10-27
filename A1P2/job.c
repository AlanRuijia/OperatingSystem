#include "job.h"

int add_job(pid_t pid, int current, char * argument){

    struct Job * new_job = (struct Job *) malloc(sizeof(struct Job)/sizeof(char));
    new_job -> pid = pid;
    Shell_job_c.total++;
    new_job -> is_fg = 0;
    new_job -> is_stop = 0;
    new_job -> next = NULL;
    strncpy(new_job->arg, argument, 1024);
    if (Shell_job_c.head_job == NULL){
        Shell_job_c.head_job = new_job;
        Shell_job_c.tail_job = new_job;
        // printf("head job is null, noe added. pid : %d\n", Shell_job_c.head_job->pid);
    }
    else {
        Shell_job_c.tail_job -> next = new_job;
        Shell_job_c.tail_job = new_job;
    }
    if (current)
        Shell_job_c.current_job = new_job;
}

int remove_job(pid_t pid){

    struct Job * tmp = Shell_job_c.head_job;
    struct Job * pre = Shell_job_c.head_job;
    pid_t current_pid;

    if (tmp == NULL){
        printf("Remove job: No jobs\n");
        return -1;
    }
    
    if (tmp -> pid == pid){
            Shell_job_c.total--;
            Shell_job_c.head_job = tmp->next;
            if (tmp == Shell_job_c.tail_job)
                Shell_job_c.tail_job = NULL;
            if (tmp == Shell_job_c.current_job)
                Shell_job_c.current_job = NULL;
            free(tmp);
            // printf("Job removed\n");
            return 0;
    }
    tmp = tmp -> next;

    while (tmp!=NULL){
        current_pid = tmp -> pid;
        if (current_pid != pid){
            pre = tmp;
            tmp = tmp-> next;
            continue;
        }

        Shell_job_c.total--;
        pre->next = tmp->next;

        if (tmp == Shell_job_c.tail_job)
            Shell_job_c.tail_job = NULL;

        if (tmp == Shell_job_c.current_job)
            Shell_job_c.current_job = NULL;

        free(tmp);
        // printf("Job removed\n");
        return 0;
    }
    printf("Remove Job: No such job\n");
    return -1;

}

void wait_job(){

    pid_t pid, current_pid;
    int stat,i;
    struct Job * tmp = Shell_job_c.head_job;

    if (tmp == NULL){
        // printf("jobs: No jobs\n");
        return;
    }

    while (tmp!=NULL){
        current_pid = tmp -> pid;
        pid = waitpid(current_pid, &stat, WNOHANG|WUNTRACED);
        if (pid == -1){
            perror("wait pid: waitpid error");
            return;
        }
        tmp = tmp -> next;
        if (WIFEXITED(stat))
            remove_job(current_pid);
    }
    
}

void print_job(struct Job* tmp, int i){
    char status[5][100] = {"running", "suspended", "continued", "done", "quit (core dumped)"};
    printf("[%d] %d %s\t%s\n", i, tmp->pid, status[tmp->is_stop], tmp->arg );
}

void jobs(){

    struct Job * tmp = Shell_job_c.head_job;
    int i =1;
    while (tmp != NULL){
        print_job(tmp, i);
        tmp = tmp -> next;
        i++;
    }
}

int find_job_no(struct Job * tmp){
    struct Job * tmp1 = NULL;
    int i = 1;
    if (tmp == NULL)
        return -1;
    tmp1 = Shell_job_c.head_job;
    while (tmp1 != NULL){
        if (tmp1 == tmp)
            return i;
        i++;
        tmp1 = tmp1->next;
    }
    return -1;

}
struct Job * find_job_by_pid(pid_t pid){
    struct Job * tmp = NULL;
    // printf("Wntered\n");
    if (Shell_job_c.total == 0)
        return NULL;
    tmp = Shell_job_c.head_job;
    while (tmp != NULL){
        // printf("Job Found3\n");
        if (tmp -> pid == pid)
            return tmp;
        tmp = tmp -> next;
    }
    return NULL;

}

struct Job * find_job_by_no(int boundary){
    struct Job * tmp = NULL;
    int i;
    if (Shell_job_c.total == 0){
        printf("find_job_by_no: total is 0\n");
        return NULL;
    }
    tmp = Shell_job_c.head_job;
    i = 1;
    while (i < boundary && i < Shell_job_c.total){
        tmp = tmp -> next;
        i++;
    }
    if (i > Shell_job_c.total){
        printf("find_job_by_no: i > total\n");
        return NULL;
    }
    return tmp;

}


void bg_job(pid_t pid){

    struct Job * tmp = find_job_by_pid(pid);
    
    if (tmp != NULL){
        kill(pid, SIGCONT);
        tmp -> is_stop = 2;
        print_job(tmp, find_job_no(tmp));
        return;
    }
    else {
        perror("bg_job error: find no job with pid ");
        return;
    }

}

void fg_job(pid_t pid){
    
    int stat;
    struct Job * tmp = find_job_by_pid(pid);

    if (tmp != NULL){

        Shell_job_c.current_job = tmp;
        waitpid(tmp->pid, &stat, WUNTRACED);
        if (!WIFSTOPPED(stat))
            remove_job(tmp->pid);
    }
    else {
        perror("fg_job error: find no job with pid ");
        return;
    }

}