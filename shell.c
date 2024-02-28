/**
 * shell.c
 * 
 * Implements the high level logic of the shell program, which prompts a user
 * for commands to run (supporting pipes) until an 'exit' command is provided,
 * at which point, the program will exit.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "tokenizer.h"
#include "pipeline.h"

const char PROMPT[] = "simple-pipe-shell$ ";
const char STATUS_MSG[] = "simple-pipe-shell status: ";
const char BAD_INPUT_MSG[] = "simple-pipe-shell: Please provide a valid command.";
const char EXIT_CMD[] = "exit";
const int MAX_CHARS = 1024;

/**
 * Validate that the user input doesn't start with a pipe, end with a pipe, or
 * have (ignoring whitespace) consecutive pipes.
 *
 * @param inputBuffer: The '\n' terminated input buffer.
 * @returns 1 if the input buffer is valid otherwise 0.
*/
int validInputBuffer(char *inputBuffer) {
    int canBePipe = 0;
    int endsWithPipe = 0;
    int i = 0;

    if (strcmp(inputBuffer, "\n") == 0) {
        return 0;
    }

    while (inputBuffer[i] != '\n') {

        // Ignore whitespace
        if (inputBuffer[i] == ' ' || 
            inputBuffer[i] == '\t' ||
            inputBuffer[i] == '\n') {
            i++;
            continue;
        }

        // Check that there aren't consecutive pipes
        if (!canBePipe && (inputBuffer[i] == '|')) {
            return 0;
        } else if (canBePipe && (inputBuffer[i] == '|')) {
            canBePipe = 0;
            endsWithPipe = 1;
        } else if (!canBePipe && (inputBuffer[i] != '|')) {
            canBePipe = 1;
            endsWithPipe = 0;
        }
        i++;
    }

    // The command cannot end with a pipe 
    if (!endsWithPipe)
        return 1;
    
    return 0;
}


/**
 * Calls fgets and exits with an error message if it returns NULL.
 *  
 * @param inputBuffer: The input buffer (which is passed to fgets).
 * @returns Nothing.
*/
void readInput(char* inputBuffer) {

    if (fgets(inputBuffer, MAX_CHARS, stdin) == NULL) {
        fprintf(stderr,"%s(fgets) %s\n", ERROR_MSG, strerror(errno));
        exit(JSH_EXIT_FAILURE);
    }
    return;
}


/**
 * Prompts for user input, then tokenizes and allocated a dynamic 3D char array
 * that stores the input, tokenized by '|' in the first dimension then by 
 * whitespace in the second dimension.
 * 
 * @param command: The base memory address at which to store the 3D array.
 * @param input_buffer: The base address of memory to use for the input buffer.
 * @returns Nothing.
*/
void promptUser(char ****command, char *inputBuffer) {
    
    if (command != NULL) { 
        clearCommand(*command);
    }

    // Prompt the user until a valid command is provided.
    printf("%s", PROMPT);
    readInput(inputBuffer);
    
    while (!validInputBuffer(inputBuffer)) {
        printf("%s\n", BAD_INPUT_MSG);
        printf("%s", PROMPT);
        readInput(inputBuffer);
    }
    *command = tokenizeCommand(inputBuffer);
}


/**
 * Entry point of the shell.
 * 
 * @returns 0 on success.
*/
int main(void) {
    char ***command = NULL;
    char inputBuffer[MAX_CHARS];
    promptUser(&command, inputBuffer);

    // Prompt the user and execute pipelines until an exit command is provided.
    while (strcmp(command[0][0], EXIT_CMD)) {
        printf("%s%d\n", STATUS_MSG, pipeline(command));
        promptUser(&command, inputBuffer);
    }
    clearCommand(command);
    return 0;
}
