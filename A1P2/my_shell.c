#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

#include "job.h"
#include "mysignal.h"

#define ARG_LEN 1024

int saved_stdin, saved_stdout;
pid_t shell_pid;
FILE * fd;
int in;
int pfds[2];

extern struct Job_Container Shell_job_c;

int pipe_parse(char* line, char* args[], int* is_finished, int* is_pipe, int* cmd_num, int* is_last, char ** saveptr);
int parse(char* line, char* args[]);
int external_cmd(char* args[], int cmd_count, int is_pipe);
void print_args(char *args[], int cmd_count);
void print_prompt();
int cd(char* args[]);
void switch_cmd(char* args[], int cmd_count, int is_pipe, int cmd_num);
void init_shell();

int pipe_parse(char* line, char* args[], int* is_finished, int* is_pipe, int* cmd_num, int* is_last, char ** saveptr){
    
    int cmd_count = 0;
    char* one_line = NULL;
    char * tmp_ptr = *saveptr;
    // printf("is_finished: %d\n", *is_finished);
    if (*is_finished){

        fgets(line, 1024, stdin);
        if (feof(stdin))
            exit(0);
        *is_finished = 0;
        one_line = strtok_r(line, "|", saveptr);
        // tmp_ptr = *saveptr;
        *cmd_num = 0;
        // if (strtok_r(line, "|", &tmp_ptr) != NULL){
        //     *is_pipe = 1;
        // }
    }
    else{
        one_line = strtok_r(NULL, "|", saveptr);
    }

    if (one_line == NULL){
        // printf("one line is null\n");
        *is_finished = 1;
        *is_pipe = 0;

        return 0;
    }
    else{
        (*cmd_num)++;
    }
    cmd_count = parse(one_line, args);
    return cmd_count;
}

int parse(char* line, char* args[])
{
    char* tmp_line = line;
    int i=0;
    while (*tmp_line != '\0') {      
        if (*tmp_line == ' ' || *tmp_line == '\t' || *tmp_line == '\n')
            *tmp_line = '\0';
        tmp_line++;     
    }
    while(line != tmp_line){
        if (*line == '\0' && line != tmp_line) {
            line++;
            continue;
        }
        args[i++] = line;
          
        while (*line != '\0')
            line++;
    }
    args[i] = '\0';   
    return i;
}

int external_cmd(char* args[], int cmd_count, int is_pipe){

    pid_t pid, r_pid;
    int stat;
    int res;
    // char test[100];
    pid = fork();

    if (pid < 0)
        perror("error in fork");
    else if (pid == 0){
        if (in != 0){
            dup2(in, 0);
            close(in);
        }
        if (pfds[1] != 1){
            dup2(pfds[1], 1);
            close(pfds[1]);
        }
        if (strcmp(args[cmd_count-1], "&") == 0){
            setpgid(0, 0);
            args[cmd_count-1] = '\0';
        }
        execvp(*args, args);
    }
    else{
        // close(pfds[1]);
        add_job(pid, !!strcmp(args[cmd_count-1], "&"), args[0]);
        if (strcmp(args[cmd_count-1], "&") != 0){
            r_pid = waitpid(pid, &stat, WUNTRACED);

            if (!WIFSTOPPED(stat))
                remove_job(pid);   
        }
        return 0;
    }

}

void print_args(char *args[], int cmd_count){
    int i;
    for (i=0; i< cmd_count;i++){
        printf("print args: %s\n",args[i] );
    }
}

void print_prompt(){
    char cwd[2048],hostname[1024];
    struct passwd *pw;
    pw = getpwuid(getuid());
    if (!pw)
        perror("getpwuid() error");
    if (gethostname(hostname, 1024) != 0)
        perror("gethostname() error");
    if (getcwd(cwd, 2048) != NULL){
        printf("%s@%s:%s $", pw->pw_name, hostname, cwd);
    }
    else
        perror("getcwd() error");
}

int cd(char* args[]){
    char dir[1024];
    char cwd[1024];
    if (args[2] != '\0')
        perror("my_shell: Too many parameters. \n");
    struct stat s;
    if (stat(args[1], &s)==0 && S_ISDIR(s.st_mode)){
        // printf("Changing to %s\n", args[1]);
        chdir(args[1]);
    }
    else{
        fprintf(stderr, "my_shell: No such file or directory. \n");
    }
}

int str_to_num(char * args[]){
    int i;
    int job_no;
    for (i = 1; i < strlen(args[1]);i++){
        if (args[1][i] <= '9' && args[1][i] >= '0'){
            job_no += job_no*10 + args[1][i] - '0';
        }
        else {
            printf("No such job %s\n", args[1]);
            return -1;
        }
    }
    if (job_no > Shell_job_c.total){
        printf("Job Number exceeds boundary.\n");
        return -1;
    }
    // printf("job num %d", job_no);
    return job_no;
}


int bg(char* args[]){

    int job_no = 0;
    struct Job * tmp = NULL;

    if (args[2] != '\0')
        perror("my_shell: Too many parameters. \n");
    if (args[1][0] != '%')
        printf("Job not found : %s", args[1]);
    job_no = str_to_num(args);
    // printf("bg job no : %d\n", job_no);
    tmp = find_job_by_no(job_no);
    // printf("job founded %d\n", tmp -> pid);
    bg_job(tmp -> pid);

}

int fg(char* args[]){

    int job_no = 0;
    struct Job * tmp = NULL;

    if (args[2] != '\0'){
        perror("my_shell: Too many parameters. \n");
        return -1;
    }
    if (args[1][0] != '%'){
        printf("No such job %s\n", args[1]);
        return -1;
    }

    job_no = str_to_num(args);
    tmp = find_job_by_no(job_no);

    fg_job(tmp -> pid);
}

void switch_cmd(char* args[], int cmd_count, int is_pipe, int cmd_num){

    if (strcmp(args[0], "exit") == 0)
        exit(0);
    else if (strcmp(args[0],"cd") == 0)
        cd(args);
    else if (strcmp(args[0], "bg") == 0)
        bg(args);
    else if (strcmp(args[0], "fg") == 0)
        fg(args);
    else if (strcmp(args[0], "jobs") == 0)
        jobs();
    else if (strcmp(args[0], "kill") == 0)
        ;
    else{ 
        external_cmd(args, cmd_count, is_pipe);
    }
}

void init_shell(){
    Shell_job_c.total = 0;
    Shell_job_c.current_job = NULL;
    Shell_job_c.head_job = NULL;
    Shell_job_c.tail_job = NULL;

    signal (SIGINT, &sig_int); // ctrl c
    signal (SIGTSTP, &sig_stp); // ctrl z
    signal (SIGQUIT, &sig_quit); // ctrl /
    shell_pid = getpid();

    saved_stdout = dup(1);
    saved_stdin = dup(0);
    // setpgid(shell_pgid, shell_pgid);
    // tcsetpgrp(shell_in, shell_pgid);
    // tcgetattr(shell_in, &shell_termios);
}

int main(){
    char args[20][ARG_LEN];
    char cmd[1024];
    int cmd_count = 0;
    int is_pipe = 1;
    int is_finished = 1;
    char* saveptr = NULL;
    int cmd_num = 0;
    int is_last = 0;
    char test[100];
    
    init_shell();
    fd = fopen("debug.txt", "w");
    
	while (1){
        if (is_finished)
            print_prompt();
        cmd_count = pipe_parse(cmd, args, &is_finished, &is_pipe, &cmd_num, &is_last,  &saveptr);
        if (is_finished){
            dup2(saved_stdout, pfds[0]);
        }

        if (cmd_count == -1){
            close(saved_stdout);
            close(saved_stdin);
            exit(0);
        }
        if (cmd_count == 0){
            continue;
        }
        // if (is_pipe){
            

            // if (!is_last){
                
                // // if (cmd_num == 1){
                // //     dup2(in, 0);
                // // }
                // // else {
                // //     dup2(in, pfds[0]);
                // // }
        pipe(pfds);
        dup2(in, 0);
        dup2(pfds[1], 1);
        //     }
        //     else{
        //         // write(saved_1, "1" , 2);
        //         // dup2(saved_stdout, pfds[0]);
        //     }
        // }
        // else{
        //     // printf("exit");
        //     // write(saved_stdout, "2" , 2);
        //     // dup2(saved_stdout, 1);
        //     // dup2(saved_stdin, 0);
        // }
        // write(saved_stdin, "1" , 2);

        

        switch_cmd(args, cmd_count, is_pipe, cmd_num);
        wait_job();
        in = pfds[0];
        
	}
    fclose(fd);
}