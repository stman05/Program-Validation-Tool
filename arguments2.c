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
    char buffer;
    int inWord = 0;

    fd = open(filename, O_RDONLY, 0);
    if(fd < 0) {
        return (NULL);
    }

    i = 0;

    while(1) {
        result = read(fd, &buffer, sizeof(char));
        if(result < 0) {
            return (NULL);
        }
        if(result == 0) {
            if(inWord == 1) {
                arguments[size] = (char *)realloc(arguments[size], sizeof(char) * (i + 1));
                (arguments[size])[i] = '\0';
            }
            arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
            if(arguments == NULL) {
                return (NULL);
            }
            arguments[size] = NULL;
            close(fd);
            return (arguments);
        }
        else if((buffer == ' ') || (buffer == '\n')) {
            if(inWord == 1) {
                arguments[size] = (char *)realloc(arguments[size], sizeof(char) * (i + 1));
                (arguments[size])[i] = '\0';
                inWord = 0;
            }
        }
        else {
            if(inWord == 1) {
            arguments[size] = (char *)realloc(arguments[size], sizeof(char) * (i + 1));
            (arguments[size])[i] = buffer;
            i++;                    
            }
            else {
                arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
                if(arguments == NULL) {
                    return (NULL);
                }
                i = 1;
                arguments[size] = (char *)realloc(arguments[size], sizeof(char) * (i));
                if(arguments[size] == NULL) {
                    return (NULL);
                }
                (arguments[size])[i - 1] = buffer;
                inWord = 1;
                size++;
            }

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