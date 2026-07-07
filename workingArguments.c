#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


char **argumentsInit(char **arguments, char *progName)
{
    arguments = (char **)malloc(sizeof(char *));
    if(arguments == NULL) {
        return (NULL);
    }

    arguments[0] = (char *)malloc(sizeof(char) * (7));
    if(arguments == NULL) {
        return (NULL);
    }

    strcpy(arguments[0], "./");

    strcat(arguments[0], progName);

    return (arguments);
}

void argumentsArrayClear(char **argumentsArray)
{
    int i;

    for(i = 0; argumentsArray[i] != NULL; i++) {
        free(argumentsArray[i]);
    }
    free(argumentsArray);
}

//Takes as an argument, the arguments array with the first word already being progName
//NULL if file doesn't exist or read failed or realloc failed
char **argumentsArrayCreation(char **arguments, char *filename)
{
    int fd, result, i;
    int size = 1;
    char *buffer = NULL;
    int check = 0, check1 = 1;

    fd = open(filename, O_RDONLY, 0);
    if(fd < 0) {
        return (NULL);
    }

    while(1) {
        for(i = 0; ; i++) {
            buffer = (char *) realloc(buffer, sizeof(char) * (i + 1));
            if(buffer == NULL) {
                return (NULL);
            }
            result = read(fd, &buffer[i], sizeof(char));
            if(result < 0) {
                return (NULL);
            }
            if(buffer[i] == ' ') {
                break;
            }
            if((buffer[i] == '\n') || (result == 0)) {
                check = 1;
                break;
            }
            check1 = 0;
        }
        buffer[i] = '\0';

        if(check1 == 0) {
            arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
            if(arguments == NULL) {
                return (NULL);
            }
            arguments[size] = strdup(buffer);
            free(buffer);
            buffer = NULL;
            size++;
        }

        if(check == 1) {
            arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
            if(arguments == NULL) {
                return (NULL);
            }
            arguments[size] = NULL;
            free(buffer);
            return (arguments);
        }

    }
    
    close(fd);

    return (arguments);
}

int main(int argc, char *argv[])
{
    char **arguments = NULL;
    int i;

    if(argc != 2) {
        return (1);
    }

    arguments = argumentsInit(arguments, "add");

    arguments = argumentsArrayCreation(arguments, argv[1]);

    for(i = 0; arguments[i] != NULL; i++) {
        printf("%s\n", arguments[i]);
    }

    argumentsArrayClear(arguments);

    return (0);
}