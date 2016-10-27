#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>

#define ARG_LEN 1024

void print_args(char *args[], int cmd_count){
    int i;
    for (i=0; i< cmd_count;i++){
        printf("print args: %s\n",args[i] );
    }
}

void parse(char* line, char* args[])
{
    char* tmp_line = line;
    int i=0;
    printf("line: %s", line);
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
        // args[i] = strcpy(args[i], line);
        // i++;
        args[i++] = line;
          
        while (*line != '\0')
            line++;
    }
    printf("%d\n",i);
    args[i] = '\0';   
    print_args(args, 2);
}

void get_cmd(char* cmd, char* args[]){
    
    int cmd_count = 0;
    fgets(cmd, 1024, stdin);
    printf("getcmd: %s\n", cmd);
    parse(cmd, args);
}

int main(){
    char args[20][ARG_LEN];
    char cmd[1024];
    while (1){
        // print_prompt();
        get_cmd(cmd, args);
        // print_args(args, 2);
        // switch_cmd(args);
    }
    
}

