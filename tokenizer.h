/* tokenizer.h
 *
 * Exposes functionality to enable string tokenization into a 3D char array,
 * which in other words, is a dynamically allocated array of arrays of strings.
 * Also exposes a function to properly clean up memory after the array is used.
*/
#ifndef TOKENIZER_H
#define TOKENIZER_H
void clearCommand(char ***arr);
char ***tokenizeCommand(char *command);
extern const int MAX_ARGS;
extern const int MAX_PROGRAMS;
extern const char SEP[];
#endif
