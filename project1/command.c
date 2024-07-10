#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include "command.h"

/*
TODO:
    memory errors in these functions
    printing strderr
*/

void listDir() {
    // previous working unsorted ls
    DIR *d = opendir(".");
    struct dirent *curr;
    while ((curr = readdir(d)) != NULL) {
        // printf("%d\n", strlen(curr->d_name));
        write(1, curr->d_name, strlen(curr->d_name));
        write(1, " ", 1);
    }
    write(1, "\n", strlen("\n"));
    closedir(d);
} /*for the ls command*/

void showCurrentDir() {
    char pwd[1024];
    getcwd(pwd, sizeof(pwd));
    write(1, pwd, strlen(pwd));
    write(1, "\n", strlen("\n"));
}; /*for the pwd command*/

void makeDir(char *dirName) {
    int newDir = mkdir(dirName, 0777);
    if (newDir == -1) {
        char *err = "Directory already exists!\n";
        write(1, err, strlen(err));
    }
}; /*for the mkdir command*/

void changeDir(char *dirName) {
    int cdir = chdir(dirName);
    if (cdir != 0) {
        perror("chdir");
    }
}; /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath) {
    // if dstpath is a file or a dir
    struct stat stat_struct;
    char dstPath[1024];
    strcpy(dstPath, destinationPath);
    // system call, stores info about file or dir into stat_struct
    if (stat(destinationPath, &stat_struct) == 0) {
        // if dst is a dir, copy src file name
        if (S_ISDIR(stat_struct.st_mode)) {
            strcat(dstPath, "/");
            strcat(dstPath, basename(sourcePath));
        }
    }
    // open src file
    int origFile = open(sourcePath, O_RDONLY);
    if (origFile == -1) {
        write(1, "bash: cp: \n", strlen("bash: cp: "));
        write(1, sourcePath, strlen(sourcePath));
        write(1, ": ", strlen(": "));
        write(1, "No such file\n", strlen("No such file\n"));
    }

    // create new copied file
    // O_RDWR | O_CREATE creates a new file and returns it as rdwr
    int cpyFile = open(dstPath, O_RDWR | O_CREAT, 0777);
    char buf[1024];
    int readBytes;
    while ((readBytes = read(origFile, buf, 1024)) > 0) {
        write(cpyFile, buf, readBytes);
    }
    close(origFile);
    close(cpyFile);
}; /*for the cp command*/


// done
void moveFile(char *sourcePath, char *destinationPath) {
    // if dstpath is a file or a dir
    struct stat stat_struct;
    char dstPath[1024];
    strcpy(dstPath, destinationPath);
    // system call, stores info about file or dir into stat_struct
    if (stat(destinationPath, &stat_struct) == 0) {
        // if dst is a dir, copy src file name
        if (S_ISDIR(stat_struct.st_mode)) {
            strcat(dstPath, "/");
            strcat(dstPath, basename(sourcePath));
        }
    }
    // open src file
    int origFile = open(sourcePath, O_RDONLY);
    if (origFile == -1) {
        write(1, "bash: mv: \n", strlen("bash: cp: "));
        write(1, sourcePath, strlen(sourcePath));
        write(1, ": ", strlen(": "));
        write(1, "No such file\n", strlen("No such file\n"));
    }

    // create new copied file
    // O_RDWR | O_CREATE creates a new file and returns it as rdwr
    int cpyFile = open(dstPath, O_RDWR | O_CREAT, 0777);
    char buf[1024];
    int readBytes;
    while ((readBytes = read(origFile, buf, 1024)) > 0) {
        write(cpyFile, buf, readBytes);
    }
    remove(sourcePath);
    close(origFile);
    close(cpyFile);
}; /*for the mv command*/

// delete
void deleteFile(char *filename) {
    remove(filename);
}; /*for the rm command*/

void displayFile(char *filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        write(1, "cat: ", strlen("cat: "));
        write(1, filename, strlen(filename));
        write(1, ": ", strlen(": "));
        write(1, "No such file or directory\n", strlen("No such file or directory\n"));
    }
    char buf[1024];
    int readBytes;
    while ((readBytes = read(file, buf, 1024)) > 0) {
        write(1, buf, readBytes);
    }
    write(1, "\n", strlen("\n"));
    close(file);
}; /*for the cat command*/
