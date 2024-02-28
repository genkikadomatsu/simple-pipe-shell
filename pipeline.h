/* pipeline.h
 *
 * Exposes functionality to enable the creation an execution of processes in 
 * pipelines for the simple-pipe-shell.
*/
#ifndef PIPELINE_H
#define PIPELINE_H
int pipeline(char ***command);
extern const int JSH_EXIT_FAILURE;
extern const char ERROR_MSG[];
#endif
