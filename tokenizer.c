/**
 * tokenizer.c
 * 
 * Implements the logic related to tokenizing user commands for the shell.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int MAX_ARGS = 1024;
const int MAX_PROGRAMS = 1024;
const char SEP[] = "|";

/**
 * Frees memory of a NULL terminated, dynamically allocated 3D charr array.
 * 
 * @param arr: The NULL terminated, dynamically allocated 3D char array.
 * @returns Nothing.
*/
void clearCommand(char ***arr) {
    
    if (arr == NULL) {
        return;
    }
    int i = 0;

    while (arr[i] != NULL) {
        int j = 0;
        
        if (arr[i][j] == NULL) {
            continue;
        }
        
        while (arr[i][j] != NULL) {
            free(arr[i][j]);
            arr[i][j] = NULL;
            j++;
        }
        free(arr[i]);
        arr[i] = NULL;
        i++;
    }

    free(arr);
    arr = NULL;
    return;
}


/**
 * Takes a string and returns an array of strings, tokenized by whitespace.
 *
 * @param program: The input string.
 * @returns An array of strings that is the tokenized input string.
*/
char **tokenizeArgs(char *program) {
    char **args = (char**)malloc(MAX_ARGS * sizeof(char*));

    if (args == NULL) {
        fprintf(stderr, "malloc failed\n");
    } 
    char *argToken = strtok(program, " \t\n");
    int argCount = 0;

    while (argToken != NULL) {
        args[argCount] = strdup(argToken);

        if (args[argCount] == NULL) {
            fprintf(stderr, "strdup failed\n");
            exit(EXIT_FAILURE);
        }
        argToken = strtok(NULL, " \t\n");
        argCount++;
    }
    args[argCount] = NULL;

    return args;
}


/**
 * Given a string, tokenizes by SEP to make string arrays, then by whitespace
 * to create char arrays, resulting in a 3D dynamically allocated char array.
 * 
 * @param command: The string that is the input comnmand.
 * @returns The 3D char array (rows of arrays of strings).
*/
char ***tokenizeCommand(char *command) {
    char ***programs = (char***)malloc(MAX_PROGRAMS * sizeof(char**));
    
    if (programs == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    char *tokenPtr;
    char *programToken = strtok_r(command, SEP, &tokenPtr);
    int programCount = 0;

    while (programToken != NULL) {
        programs[programCount] = tokenizeArgs(programToken);
        
        if (programs[programCount] == NULL) {
            fprintf(stderr, "strdup failed\n");
            exit(EXIT_FAILURE);
        }
        programToken = strtok_r(NULL, SEP, &tokenPtr);
        programCount++;
    }
    programs[programCount] = NULL;
    
    return programs;
}
