#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_LEN 100

char *find_path(char *command)
{
    char *path;
    char *token;
    char *full_path;

    path = getenv("PATH"); /* get the PATH variable */
    if (path == NULL) /* error getting PATH */
        return (NULL);
    path = strdup(path); /* make a copy of PATH */
    if (path == NULL) /* error duplicating PATH */
        return (NULL);
    token = strtok(path, ":"); /* split PATH by : */
    while (token != NULL) /* loop through directories */
    {
        full_path = malloc(strlen(token) + strlen(command) + 2); /* allocate memory for full path */
        if (full_path == NULL) /* error allocating memory */
            return (NULL);
        strcpy(full_path, token); /* copy directory */
        strcat(full_path, "/"); /* append / */
        strcat(full_path, command); /* append command */
        if (access(full_path, X_OK) == 0) /* check if executable exists and is executable */
        {
            free(path); /* free the copy of PATH */
            return (full_path); /* return the full path */
        }
        free(full_path); /* free the full path */
        token = strtok(NULL, ":"); /* get next directory */
    }
    free(path); /* free the copy of PATH */
    return (NULL); /* executable not found in PATH */
}

int main(void)
{
    char buffer[MAX_LEN];
    char *argv[2];
    int status;
    char *full_path;

    while (1)
    {
        printf("$ "); /* display prompt */
        if (fgets(buffer, MAX_LEN, stdin) == NULL) /* read command */
        {
            printf("\n"); /* handle end of file */
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; /* remove newline */
        if (strcmp(buffer, "exit") == 0) /* exit built-in */ // This is the added code
            break; // This is the added code
        argv[0] = buffer; /* set arguments for execve */
        argv[1] = NULL;
        status = fork(); /* create child process */
        if (status == -1) /* fork error */
        {
            perror("fork");
            exit(1);
        }
        if (status == 0) /* child process */
        {
            full_path = find_path(buffer); /* find the full path of the executable */
            if (full_path == NULL) /* executable not found in PATH */
            {
                perror(buffer);
                exit(1);
            }
            if (execve(full_path, argv, NULL) == -1) /* execute command with full path */
            {
                perror(buffer);
                exit(1);
            }
            free(full_path); /* free the full path */
        }
        else /* parent process */
        {
            wait(NULL); /* wait for child to finish */
        }
    }
    return (0);
}