#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUFFER_SIZE 1024

extern char **environ; /* declare the environ variable */

char *strdup(const char *s) /* a custom strdup function */
{
    char *dup;
    size_t len;

    len = strlen(s) + 1; /* get the length of the string */
    dup = malloc(len); /* allocate memory for the duplicate */
    if (dup == NULL) /* check for allocation error */
        return (NULL);
    memcpy(dup, s, len); /* copy the string to the duplicate */
    return (dup); /* return the duplicate */
}

char *strchr(const char *s, int c) /* a custom strchr function */
{
    while (*s != '\0') /* loop through the string */
    {
        if (*s == c) /* check if character matches */
            return ((char *)s); /* return the pointer to the character */
        s++; /* increment the pointer */
    }
    if (c == '\0') /* check if character is null terminator */
        return ((char *)s); /* return the pointer to the null terminator */
    return (NULL); /* character not found */
}

char *_strtok(char *str, const char *delim) /* a custom strtok function */
{
    static char *saveptr; /* declare a static pointer to save the state */
    char *token;

    if (str == NULL) /* if str is NULL, use the saved pointer */
        str = saveptr;
    if (str == NULL) /* if both str and saveptr are NULL, return NULL */
        return (NULL);
    str += strspn(str, delim); /* skip any leading delimiters */
    if (*str == '\0') /* if str is empty, return NULL */
    {
        saveptr = NULL;
        return (NULL);
    }
    token = str; /* assign token to str */
    str = strchr(str, *delim); /* find the next delimiter in str */
    if (str == NULL) /* if no delimiter found, saveptr is NULL */
        saveptr = NULL;
    else
    {
        *str = '\0'; /* replace delimiter with null terminator */
        saveptr = str + 1; /* save the pointer to the next character */
    }
    return (token); /* return the token */
}

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
    token = _strtok(path, ":"); /* split PATH by : using _strtok function*/
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
        token = _strtok(NULL, ":"); /* get next directory using _strtok function*/
    }
    free(path); /* free the copy of PATH */
    return (NULL); /* executable not found in PATH */
}

void print_env(void)
{
    int i;

    for (i = 0; environ[i] != NULL; i++) /* loop through environ array */
    {
        printf("%s\n", environ[i]); /* print each variable */
    }
}

int _setenv(const char *name, const char *value)
{
   int i;
   int len;
   char *var;

   if (name == NULL || value == NULL || name[0] == '\0') /* check for invalid arguments */
       return (-1);
   len = strlen(name) + strlen(value) + 2; /* get the length of the variable */
   var = malloc(len); /* allocate memory for the variable */
   if (var == NULL) /* check for allocation error */
       return (-1);
   sprintf(var, "%s=%s", name, value); /* format the variable as name=value */
   for (i = 0; environ[i] != NULL; i++) /* loop through environ array */
   {
       if (strncmp(environ[i], name, strlen(name)) == 0) /* check if name matches */
       {
           environ[i] = var; /* replace the variable */
           return (0); /* return success */
       }
   }
   environ[i] = var; /* append the variable to the end of the array */
   return (0); /* return success */
}

int _unsetenv(const char *name)
{
    int i;
    int j;

    if (name == NULL || name[0] == '\0') /* check for invalid arguments */
        return (-1);
    for (i = 0; environ[i] != NULL; i++) /* loop through environ array */
    {
        if (strncmp(environ[i], name, strlen(name)) == 0) /* check if name matches */
        {
            free(environ[i]); /* free the variable */
            for (j = i; environ[j] != NULL; j++) /* loop from the matched index */
            {
                environ[j] = environ[j + 1]; /* shift the variables to the left */
            }
            i--; /* decrement i to check the next variable */
        }
    }
    return (0); /* return success */
}

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
    char *buffer;
    size_t n;
    ssize_t len;
    char **argv;
    int status;
    char *full_path;
    
     buffer = NULL; // initialize buffer to NULL
     n = 0; // initialize n to 0
     argv = malloc(sizeof(char *) * 3); // allocate memory for argv array
     if (argv == NULL) // check for allocation error
         return (1);

    while (1)
    {
        printf("$ "); /* display prompt */
        len = _getline(&buffer, &n, stdin); /* read command using _getline function */
        if (len == -1) /* handle end of file or error */
        {
            printf("\n");
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; /* remove newline */
        argv[0] = _strtok(buffer, " "); /* split command by space using _strtok function */
        argv[1] = _strtok(NULL, " "); /* get the second argument if any */
        argv[2] = NULL; /* set the last element to NULL */
        if (strcmp(argv[0], "exit") == 0) /* exit built-in */ 
        {
            if (argv[1] != NULL) /* if there is a second argument */
            {
                status = atoi(argv[1]); /* convert it to an integer and use it as exit status */
                free(buffer); // free the buffer allocated by _getline function
                free(argv); // free the argv array
                exit(status); // exit with the given status
            }
            break; 
        }
        if (strcmp(argv[0], "env") == 0) // This is the added code
        {
            print_env(); // This is the added code
            continue; // This is the added code
        }
        if (strcmp(argv[0], "setenv") == 0) // This is the added code
        {
            if (_setenv(argv[1], argv[2]) == -1) // This is the added code
                fprintf(stderr, "Error: setenv failed\n"); // This is the added code
            continue; // This is the added code
        }
        if (strcmp(argv[0], "unsetenv") == 0) // This is the added code
        {
            if (_unsetenv(argv[1]) == -1) // This is the added code
                fprintf(stderr, "Error: unsetenv failed\n"); // This is the added code
            continue; // This is the added code
        }
        
        status = fork(); /* create child process */
        if (status == -1) /* fork error */
        {
            perror("fork");
            exit(1);
        }
        if (status == 0) /* child process */
        {
            full_path = find_path(argv[0]); /* find the full path of the executable */
            if (full_path == NULL) /* executable not found in PATH */
            {
                perror(argv[0]);
                exit(1);
            }
            if (execve(full_path, argv, environ) == -1) // This is the modified code
            {
                perror(argv[0]);
                exit(1);
            }
            free(full_path); /* free the full path */
        }
        else /* parent process */
        {
            wait(NULL); /* wait for child to finish */
        }
    }
    free(buffer); /* free the buffer allocated by _getline function*/
    free(argv); // free the argv array
    return (0);
}
