#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"
// for open system call
#include <fcntl.h>
#include <unistd.h>
// commands
#include "command.h"

// initialize extern fileMode to 1 (stdout)
// int fileMode = 1;

int main(int argc, char *argv[]) {
    char *line = NULL; // initialized line buffer
    size_t length = 0; // initialize buffer size
    ssize_t read; // number of chars read, ssize_t is of type signed size_t
    FILE *file;
    int outFile;

    if (argc > 1) {
        file = fopen(argv[2], "r");
        // create new file output.txt
        outFile = open("output.txt", O_RDWR | O_CREAT | O_EXCL | O_APPEND, 0777);
        // external variable
        FILE *fp = freopen("output.txt", "w", stdout);
        // fileMode = outFile; // change to out file if file mode
    }
    
    while (1) {

        // case 1: interactive mode
        if (argc == 1) { // just file name, no -f flag
            // printf(">>> ");
            write(1, ">>> ", strlen(">>> "));
            read = getline(&line, &length, stdin);
            if (read == -1) {
                break;
            }
            if (strcmp(line, "exit\n") == 0) {
                break;
            }
        }
        // case 1: file mode
        else {
            read = getline(&line, &length, file);
            if (read == -1) {
                break;
            }
        }
        // printf("read %zu chars: %s", read, line);

        command_line large_token_buffer;
        command_line small_token_buffer;
        //tokenize line buffer
		//large token is seperated by ";"
		large_token_buffer = str_filler (line, ";");
		//iterate through each large token
		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
		{
			// printf ("\tLine segment %d:\n", i + 1);

			//tokenize large buffer
			//smaller token is seperated by " "(space bar)
			small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");

            // TODO: error handling
            char errPrint[1024] = "Error! Unsupported parameters for command: ";
            char **com_list = small_token_buffer.command_list;
            if (strcmp(com_list[0], "ls") == 0) {
                if (small_token_buffer.num_token != 1) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                listDir();
            }
            else if (strcmp(com_list[0], "pwd") == 0) {
                if (small_token_buffer.num_token != 1) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                showCurrentDir();
            }
            else if (strcmp(com_list[0], "mkdir") == 0) {
                if (small_token_buffer.num_token != 2) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                makeDir(com_list[1]);
            }
            else if (strcmp(com_list[0], "cd") == 0) {
                if (small_token_buffer.num_token != 2) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                changeDir(com_list[1]);
            }
            else if (strcmp(com_list[0], "cp") == 0) {
                if (small_token_buffer.num_token != 3) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                copyFile(com_list[1], com_list[2]);
            }
            else if (strcmp(com_list[0], "mv") == 0) {
                if (small_token_buffer.num_token != 3) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                moveFile(com_list[1], com_list[2]);
            }
            else if (strcmp(com_list[0], "rm") == 0) {
                if (small_token_buffer.num_token != 2) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                deleteFile(com_list[1]);
            }
            else if (strcmp(com_list[0], "cat") == 0) {
                if (small_token_buffer.num_token != 2) {
                    strcat(errPrint, com_list[0]);
                    strcat(errPrint, "\n");
                    write(1, errPrint, strlen(errPrint));
                    break;
                }
                displayFile(com_list[1]);
            }
            else {
                char unrec[200] = "Error! Unrecognized command: ";
                strcat(unrec, com_list[0]);
                strcat(unrec, "\n");
                write(1, unrec, strlen(unrec));
            }

			//free smaller tokens and reset variable
			free_command_line(&small_token_buffer);
			memset (&small_token_buffer, 0, 0);
		}
		//free smaller tokens and reset variable
		free_command_line (&large_token_buffer);
		memset (&large_token_buffer, 0, 0);
    }
    // if we're in file mode, close it.
    if (argc > 1) {
        fclose(file);
        close(outFile);
    }
    free(line);
}