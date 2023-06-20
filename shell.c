#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFSIZE 1024
#define PROMPT "$ "

int main(void)
{
    char *line; /* the input line */
    char *cmd; /* the command to execute */
    size_t len = 0; /* the length of the input line */
    ssize_t read; /* the number of bytes read */
    pid_t pid; /* the process id */
    int status; /* the exit status */

    /* print the prompt and wait for input */
    printf(PROMPT);
    while ((read = getline(&line, &len, stdin)) != -1)
    {
        /* remove the newline character */
        line[read - 1] = '\0';

        /* fork a child process */
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) /* child process */
        {
            /* execute the command */
            cmd = line;
            if (execve(cmd, NULL, NULL) == -1)
            {
                perror(cmd);
                exit(EXIT_FAILURE);
            }
        }
        else /* parent process */
        {
            /* wait for the child to terminate */
            wait(&status);

            /* print the prompt and wait for input again */
            printf(PROMPT);
        }
    }
}
