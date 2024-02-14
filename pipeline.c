/**
 * pipeline.c
 * 
 * Implements logic related to creating a pipelines and executing programs
 * within that pipeline.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

const int JSH_EXIT_FAILURE = 127;
const char ERROR_MSG[] = "jsh error: ";

/**
 * Calls close n times on descriptors[start] up to but not including
 * descriptors[stop]. Assumes the target descriptors are still valid and open.
 * 
 * This is called post-fork.
 *  
 * @param descriptors: The array of file descriptors.
 * @param start: The starting index.
 * @param stop: The stopping index.
*/
void rangedClose(int *descriptors, int start, int stop) {
    for (int i = start; i < stop; i++) {
        if (close(descriptors[i]) == -1) {
            fprintf(stderr, "%s(close) %s\n", ERROR_MSG, strerror(errno));
            exit(JSH_EXIT_FAILURE);
        }
    }
}


/**
 * Replace this (child) process's stdin and stdout file descriptors with the 
 * correct file descriptors, given an array of pipe descriptors and that this
 * process is the ith process.
 * 
 * To be called post-fork.
 * 
 * @param descriptors: The array of file descriptors.
 * @param i: The index of this process in a 0-indexed pipeline.
*/
void pipeIthChildProcess(int *descriptors, int i, int numPipes) {
    if (i > 0) {
        int read_i = (i - 1) * 2;
        rangedClose(descriptors, 0, read_i);
        rangedClose(descriptors, read_i + 1, read_i + 2);

        if (dup2(descriptors[read_i], STDIN_FILENO) == -1) {
            fprintf(stderr, "%s(dup2) %s\n", ERROR_MSG, strerror(errno));
            exit(JSH_EXIT_FAILURE);
        }
    }
    
    if (i < numPipes) {
        int write_i = (2 * i) + 1;
        rangedClose(descriptors, write_i - 1, write_i);
        rangedClose(descriptors, write_i + 1, numPipes * 2);

        if (dup2(descriptors[write_i], STDOUT_FILENO) == -1) {
            fprintf(stderr, "%s(dup2) %s\n", ERROR_MSG, strerror(errno));
            exit(JSH_EXIT_FAILURE);
        }
    }
}


/**
 * Call pipe to write file descriptors n successive times.
 * 
 * Assumes that descriptorSpace has space of at least sizeof(int) * 2n, and
 * exits with status 127 if there is a pipe syscall error.
 *
 * @param n: The number of pipes to create. 
 * @param descriptorSpace: The memory address at which to write descriptors.
*/
void nPipe(int n, int *descriptorSpace) {
    for (int i = 0; i < n * 2; i = i + 2) {
        if (pipe(&(descriptorSpace[i])) == -1) {
            fprintf(stderr, "%s(pipe) %s\n", ERROR_MSG, strerror(errno));
            exit(JSH_EXIT_FAILURE);
        }
    }
}


/**
 * Returns the number of elements in the first dimension of a 3D char array. In
 * other words, return the number of processes in a pipelined command.
 * 
 * @param command: The 3D char array.
*/
int processCount(char ***command) {

    int i = 0;

    while (command[i] != NULL) {
        i++;
    }

    return i;
}


/**
 * Clean up memory and file descriptors before exiting.
 * 
 * In the case that a the pipeline itself fails, this function will clear all
 * dynamically allocated memory associated with the pipeline, and perform other
 * cleanup operations such as closing file descriptors of the pipeline.
 * 
 * @param fileDescriptors: The pipeline's array of file descriptors.
 * @param numDescriptors: The length of fileDescriptors.
 * @returns 0 upon success.
*/
int abortPipeline(int *fileDescriptors, int numDescriptors) {
    for (int i = 0; i < numDescriptors; i++) {
        if (fcntl(fileDescriptors[i], F_GETFD) != -1) {
            if (close(fileDescriptors[i]) == -1) {
                fprintf(stderr, "%s(close) %s\n", ERROR_MSG, strerror(errno));
                return -1;
            }
        }
    }
    free(fileDescriptors);
    return 0;
}


/**
 * Given an array of PIDs, and the last PID, return the status of the status of
 * the pipeline (which is the status of the last process).
 * 
 * @param processPids: The array of PIDs.
 * @param lastPid: The last process's PID.
 * @param numProcesses: The number of PIDs.
 * @returns The status of the last process in the pipeline.
*/
int pipelineStatus(int processPids[], int lastPid, int numPids) {

    int pipelineStatus = 0;

    for (int i = 0; i < numPids; i++) {
        int tempStatus;

        if (waitpid(processPids[i], &tempStatus, 0) == - 1) {
            fprintf(stderr,"%s(wait) %s\n", ERROR_MSG, strerror(errno));
            return JSH_EXIT_FAILURE;
        }

        if (tempStatus != 0 && processPids[i] == lastPid)
            pipelineStatus = WEXITSTATUS(tempStatus);
    }
    
    return pipelineStatus;
}

/**
 * Sets up and executes a pipeline of processes, given a 3D char array that
 * represents a command (a series of processes to run in a pipeline).
 * 
 * @param command: The 3D char array.
 * @returns The return status of the pipeline.
*/
int pipeline(char ***command) {

    int numProcesses = processCount(command);
    int numPipes = numProcesses - 1;
    int *fileDescriptors = malloc(sizeof(int) * numPipes * 2); 

    if (fileDescriptors == NULL) {
        fprintf(stderr, "%smalloc failure\n", ERROR_MSG);
        return JSH_EXIT_FAILURE;
    }

    if (numPipes > 0)
        nPipe(numPipes, fileDescriptors);

    int processPids[numProcesses];
    int lastPid;

    for (int i = 0; i < numProcesses; i++) {
        int pid = fork();
        
        if (pid == -1) {
            // Parent process (fork failed)
            fprintf(stderr, "%sfork failure\n", ERROR_MSG);
            abortPipeline(fileDescriptors, numPipes * 2);
            return JSH_EXIT_FAILURE;

        } else if (pid == 0) {
            // Child process
            pipeIthChildProcess(fileDescriptors, i, numPipes);
            
            if (execvp(command[i][0], command[i]) == -1) {
                fprintf(stderr,"%s(execvp) %s\n", ERROR_MSG, strerror(errno));
                abortPipeline(fileDescriptors, numPipes * 2);
                exit(JSH_EXIT_FAILURE);
            }
        }

        processPids[i] = pid;

        if (i == numProcesses - 1)
            lastPid = pid;
    }

    rangedClose(fileDescriptors, 0, numPipes * 2);
    free(fileDescriptors);

    return pipelineStatus(processPids, lastPid, numProcesses);
}
