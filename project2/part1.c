#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
// #include "part1.h"
#include "string_parser.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Invalid use: incorrect number of parameters\n");
        return EXIT_FAILURE;
    }
    // open input file
    FILE *file = fopen(argv[2], "r");

    if (file == NULL) {
        perror("couldn't open file");
        return EXIT_FAILURE;
    }
    int read;
    char *line = NULL;
    size_t length = 0;
    int num_lines = 0;

    // count how many lines are in file
    while ((read = getline(&line, &length, file)) != -1) {
        num_lines += 1;

    }
    fclose(file);

    // reopen file and pull line by line
    file = fopen(argv[2], "r");
    if (file == NULL) {
        perror("couldn't open file");
        return EXIT_FAILURE;
    }
    // for string parsing
    command_line token_buffer;
    pid_t *pid_arr = (pid_t *)malloc(sizeof(pid_t) * num_lines);

    // pull lines one at a time, start up a new process for each one
    int index = 0;
    while ((read = getline(&line, &length, file)) != -1) {
        
        pid_t cpid = fork();
        pid_arr[index] = cpid;
        
        // thread error
        if (pid_arr[index] < 0) {
            perror("error on opening new thread");
            return EXIT_FAILURE;
        }

        // child thread
        if (pid_arr[index] == 0) {
            // line parsing
            token_buffer = str_filler(line, " ");
    
            // array to hold command and associated args
            // array should be free when execpv replaces current image process
            char **command_args = (char **)malloc(sizeof(char *) * (token_buffer.num_token + 1)); // +1 for NULL
            int i;
            for (i = 0; i < token_buffer.num_token; i++) {
                command_args[i] = (char *)malloc(sizeof(char) * 100);
                strcpy(command_args[i], token_buffer.command_list[i]); 
            }
            command_args[i] = (char *)0;
    
            // use system call execvp to run command and args
            // child should die after this runs, no thread bomb
            if (execvp(command_args[0], command_args) < 0) {
                // printf("invalid command: %s\nKilling child\n", command_args[0]);
                perror("Execvp");
                exit(-1);
            }
            for (int i = 0; i < token_buffer.num_token; i++) {
                free(command_args[i]);
            }
            free(command_args);
            exit(-1); // stopping thread bombe
        }
        index++;
    }
    // this makes parent process wait for all children process to die
    for (int i = 0; i < num_lines; i++) {
        int status;
        // waitpid halts execution until process with PID == pid
        // has exited
        // parent should only call
        waitpid(pid_arr[i], &status, 0);
        // waitpid halts execution until process with PID == pid has exited
    }

    free(line);
    fclose(file);
    free(pid_arr);
}
