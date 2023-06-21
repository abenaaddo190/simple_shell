#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    char input[MAX_LINE];
    int status;
    pid_t pid;

    while (should_run) {
        printf("simple_shell> ");
        fflush(stdout);

        fgets(input, MAX_LINE, stdin);
        input[strlen(input) - 1] = '\0';

        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "Command not found\n");
            }
            exit(EXIT_FAILURE);
        } else {
            wait(&status);
        }
    }

    return 0;
}