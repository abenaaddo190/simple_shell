#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 1024 // maximum length of a command line
#define MAXPATH 256 // maximum length of a path

// function prototypes
void execute(char *cmd); // execute a command
void change_dir(char *dir); // change directory

int main(void)
{
    char line[MAXLINE]; // buffer for the command line
    char *prompt = "simple_shell> "; // prompt string

    while (1) // main loop
    {
        printf("%s", prompt); // print the prompt
        if (fgets(line, MAXLINE, stdin) == NULL) // read a line
        {
            break; // exit on end of file
        }
        line[strcspn(line, "\n")] = '\0'; // remove the newline
        execute(line); // execute the command
    }
    return 0;
}

// execute a command
void execute(char *cmd)
{
    char *args[2]; // array for the command and its argument
    pid_t pid; // process id
    int status; // exit status

    args[0] = cmd; // the command is the first element
    args[1] = NULL; // the argument is NULL for simplicity

    if (strcmp(cmd, "cd") == 0) // if the command is cd
    {
        change_dir(NULL); // change to the home directory
        return; // do not fork a child process
    }
    else if (strncmp(cmd, "cd ", 3) == 0) // if the command is cd with an argument
    {
        change_dir(cmd + 3); // change to the given directory
        return; // do not fork a child process
    }

    pid = fork(); // fork a child process

    if (pid < 0) // error in forking
    {
        perror("fork error"); // print an error message
        exit(1); // terminate with an error code
    }
    else if (pid == 0) // child process
    {
        if (execvp(cmd, args) < 0) // execute the command
        {
            perror("exec error"); // print an error message if not found
            exit(1); // terminate with an error code
        }
    }
    else // parent process
    {
        wait(&status); // wait for the child to finish
    }
}

// change directory
void change_dir(char *dir)
{
    char cwd[MAXPATH]; // buffer for the current working directory
    char oldpwd[MAXPATH]; // buffer for the old working directory

    if (dir == NULL || strcmp(dir, "~") == 0) // if no argument or ~ is given
    {
        dir = getenv("HOME"); // use the home directory as the argument
    }
    else if (strcmp(dir, "-") == 0) // if - is given as the argument
    {
        dir = getenv("OLDPWD"); // use the old working directory as the argument
        printf("%s\n", dir); // print the old working directory
    }

    if (getcwd(oldpwd, MAXPATH) == NULL) // get the old working directory
    {
        perror("getcwd error"); // print an error message if failed
        return; 
    }

    if (chdir(dir) < 0) // change to the new directory
    {
        perror("chdir error"); // print an error message if failed
        return;
    }

    if (getcwd(cwd, MAXPATH) == NULL) // get the new working directory
    {
        perror("getcwd error"); // print an error message if failed
        return;
    }

    if (setenv("PWD", cwd, 1) < 0) // update the environment variable PWD 
    {
        perror("setenv error"); // print an error message if failed 
        return;
    }

    if (setenv("OLDPWD", oldpwd, 1) < 0) // update the environment variable OLDPWD 
    {
        perror("setenv error"); // print an error message if failed 
        return;
    }
}