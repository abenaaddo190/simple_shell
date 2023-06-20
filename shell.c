#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        printf("$ ");
        fflush(stdout);  // Flush the output buffer

        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\nShell exited.\n");
            break;
        }

        // Remove the trailing newline character from the command
        command[strcspn(command, "\n")] = '\0';

        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to create child process");
            continue;
        }

        if (pid == 0) {
            // Child process
            execlp(command, command, (char *)NULL);  // Execute the command

            // If execlp returns, it means the command was not found
            fprintf(stderr, "Command not found: %s\n", command);
            exit(1);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);  // Wait for the child process to finish

            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status != 0) {
                    fprintf(stderr, "Command failed with exit code %d\n", exit_status);
                }
            }
        }
    }

    return 0;
}
