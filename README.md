# simple-pipe-shell
A simple shell program that supports piping, primarily used as a learning exercise for multiprocess programming.

# Running the program
To run the project, compile using the provided `Makefile` and run the `shell` executable.
An `exit` command will terminate the shell.
The shell will print out a status after each command.

# Note
This is a very primitive shell that only supports piping,
you may find the feature set rather limited. For example,
the following are not implemented.
- quoting
- input/output redirection
- aliasing
  

# Example usage
```
> ./shell
simple-pipe-shell$ echo helloWorld
helloWorld
simple-pipe-shell status: 0
simple-pipe-shell$ echo helloWorld | grep oWo 
helloWorld
simple-pipe-shell status: 0
simple-pipe-shell$ exit
```
