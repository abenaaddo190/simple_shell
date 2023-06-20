#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

ssize_t _getline(char **lineptr, size_t *n, FILE *stream)
{
    static char buffer[BUFFER_SIZE]; /* declare a static buffer */
    static int index = 0; /* declare a static index */
    static int bytes_read = 0; /* declare a static bytes_read */
    int i;
    char *line;

    if (lineptr == NULL || n == NULL || stream == NULL) /* check for invalid arguments */
        return (-1);
    if (*lineptr == NULL) /* allocate memory for line if not allocated */
    {
        *lineptr = malloc(BUFFER_SIZE);
        if (*lineptr == NULL) /* check for allocation error */
            return (-1);
        *n = BUFFER_SIZE;
    }
    line = *lineptr; /* assign line to lineptr */
    i = 0; /* initialize i to 0 */
    while (1)
    {
        if (index == bytes_read) /* if index reaches bytes_read, read more bytes from stream */
        {
            bytes_read = read(fileno(stream), buffer, BUFFER_SIZE); /* read bytes into buffer */
            if (bytes_read == -1) /* check for read error */
                return (-1);
            if (bytes_read == 0) /* check for end of file */
            {
                if (i == 0) /* if no characters were read, return -1 */
                    return (-1);
                line[i] = '\0'; /* append null terminator to line */
                return (i); /* return number of characters read */
            }
            index = 0; /* reset index to 0 */
        }
        if (i == *n - 1) /* if line is full, reallocate more memory */
        {
            *n += BUFFER_SIZE; /* increase n by BUFFER_SIZE */
            line = realloc(line, *n); /* reallocate memory for line */
            if (line == NULL) /* check for allocation error */
                return (-1);
            *lineptr = line; /* assign lineptr to line */
        }
        line[i] = buffer[index]; /* copy character from buffer to line */
        i++; /* increment i */
        index++; /* increment index */
        if (buffer[index - 1] == '\n') /* check for newline character */
        {
            line[i] = '\0'; /* append null terminator to line */
            return (i); /* return number of characters read */
        }
    }
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
        if (strcmp(buffer, "exit") == 0) /* exit built-in */ 
            break; 
        if (strcmp(buffer, "env") == 0) // This is the added code
        {
            print_env(); // This is the added code
            continue; // This is the added code
        }
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
            if (execve(full_path, argv, environ) == -1) // This is the modified code
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
