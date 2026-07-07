/* This program tests a C code file and the program it creates.
 * Taking into account compilation errors and runtime errors.
 * It also compares its output to a file representing the expected output.
 * Hence a score is generated and printed at the end
 * 
 * Author: Stefanos Manos
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef struct {
    int compilation;
    int termination;
    int output;
    int memory;
} scoresT;

/* progErrorNameCreation(progOut, size)
 *
 * Creates a new string containing the program name followed by ".err"
 *
 * Parameters:
 *      progOut: A string containing the program name followed by ".out",
 *               needed for the error string creation
 *              (Must have enough size to fit the program name, ".err" and a null byte)
 * 
 *      size: the size of the progOut string
 * 
 * Returns: A pointer to the error string
 *          (NULL) if strdup failed
 */
char *progErrorNameCreation(char *progOut, int size)
{
    char *progErrorName;

    progErrorName = strdup(progOut);
    if(progErrorName == NULL) {
        return (NULL);
    }

    progErrorName[size - 3] = 'e';
    progErrorName[size - 2] = 'r';
    progErrorName[size - 1] = 'r';

    return(progErrorName);
}

/* progNameCreation(progOut, size)
 *
 * Creates a new string containing "./" followed by the program name
 *
 * Parameters:
 *      progOut: A string containing the program name followed by ".out",
 *               needed for the string creation
 *              (Must have enough size to fit the program name, "./" and a null byte)
 * 
 *      size: the size of the progOut string
 * 
 * Returns: A pointer to the string
 *          (NULL) if strdup or realloc failed and a message is printed in STDERR
 */
char *progNameCreation(char *progOut, int size)
{
    char *progName, *temporary;

    progName = strdup(progOut);
    if(progOut == NULL) {
        perror("strdup error\n");
        return (NULL);
    }
    progName[size - 4] = '\0';

    temporary = realloc(progName, sizeof(char *) * (size - 4));
    if(temporary == NULL) {
        perror("realloc failed\n");
        free(progName);
        return(NULL);
    }
    progName = temporary;
    return (progName);
}

/* compilationErrorCheck(filename)
 *
 * Looks for the strings "error" and "warning" inside a file
 * 
 * Parameters:
 *      filename: The file in which the words will be searched
 * 
 * Returns: The number of times "warning" was found in the file OR
 *          "-1" if "error" was found
 *          "-2" if the file doesn't exist
 */
int compilationErrorCheck(char *filename)
{
    int wordFound = 0;
    int fd, result;
    char buffer[8] = {0};
    char errorWord[] = "error";
    char warningWord[] = "warning";

    fd = open(filename, O_RDONLY, 0);
    if(fd < 0) {
        perror("File doesn't exist\n");
        return (-2);
    }

    while(1) {
        result = read(fd, &buffer[0], 1);
        if(result == 0) {
            break;
        }

        if(buffer[0] == errorWord[0]) {
            result = read(fd, &buffer[1], strlen(errorWord) - 1);
            if(result == 0) {
                break;
            }
            buffer[strlen(errorWord)] = '\0';
            if(strcmp(errorWord, buffer) == 0) {
                wordFound = -1;
                break;
            }
        }
        else if(buffer[0] == warningWord[0]) {
            result = read(fd, &buffer[1], strlen(warningWord) - 1);
            if(result == 0) {
                break;
            }
            if(strcmp(warningWord, buffer) == 0) {
                wordFound++;
            }
        }
    }
    return (wordFound);
}

/* argumentsInit(arguments, progName)
 *
 * Initializes an array of pointers to strings
 * The first pointer points to the string: "./" followed by the program name
 * The second points to NULL(End of the array) 
 * 
 * Parameters:
 *      arguments: The dynamically allocated array
 *      progName: The name of the program
 * 
 * Returns: Pointer to the array
 *          (NULL) if malloc failed
 */
char **argumentsInit(char **arguments, char *progName)
{
    arguments = (char **)malloc(sizeof(char *));
    if(arguments == NULL) {
        return (NULL);
    }

    arguments[0] = (char *)malloc(sizeof(char) * (strlen(progName) + 3));
    if(arguments == NULL) {
        return (NULL);
    }

    strcpy(arguments[0], "./");

    strcat(arguments[0], progName);

    return (arguments);
}

/* argumentsArrayClear(arguments)
 *
 * Clears all dynamically allocated memory for the array
 * 
 * Parameters:
 *      arguments: The dynamically allocated array
 * 
 * Returns: Nothing(void function)
 */
void argumentsArrayClear(char **arguments)
{
    int i;

    for(i = 0; arguments[i] != NULL; i++) {
        free(arguments[i]);
    }
    free(arguments);
}

/* argumentsArrayCreation(arguments, filename)
 *
 * Creates an array similar to argv, based on a file containing the arguments of the program
 * 
 * Parameters:
 *      arguments: The dynamically allocated array, 
 *                 already initialized with arguments[0] pointing
 *                 to the string: "./" followed by the program name
 *      filename: The name of the file containing the arguments
 * 
 * Returns: Pointer to the array
 *          (NULL) if read, realloc or strdup failed and a message is printed in STDERR
 */
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
        i = 0;
        while(1) {
            buffer = (char *) realloc(buffer, sizeof(char) * (i + 1));
            if(buffer == NULL) {
                perror("realloc failed\n");
                return (NULL);
            }
            result = read(fd, &buffer[i], sizeof(char));
            if(result < 0) {
                perror("read failed\n");
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
            i++;
        }
        buffer[i] = '\0';

        if(check1 == 0) {
            arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
            if(arguments == NULL) {
                perror("realloc failed\n");
                return (NULL);
            }
            arguments[size] = strdup(buffer);
            if(arguments[size] == NULL) {
                perror("dup failed\n");
                return (NULL);
            }
            free(buffer);
            buffer = NULL;
            size++;
        }

        if(check == 1) {
            arguments = (char **)realloc(arguments, sizeof(char *) * (size + 1));
            if(arguments == NULL) {
                perror("realloc failed\n");
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

/* printScores(score)
 *
 * Prints the score accumulated by the program being tested
 * 
 * Parameters:
 *      score: Struct containing all info about the program's score
 * 
 * Returns: Nothing(void function)
 */
void printScores(scoresT score)
{
    int total;
    
    printf("\nCompilation: %d\n", score.compilation);
    printf("\nTermination: %d\n", score.termination);
    printf("\nOutput: %d\n", score.output);
    printf("\nMemory access: %d\n", score.memory);

    total = score.compilation + score.termination + score.output + score.memory;

    if(total < 0) {
        total = 0;
    }

    printf("\nScore: %d\n", total);
}

/* alarmHandler
 *
 * A signal handler that does nothing,
 * but the signal being handled isn't ignored
 */
static void myHandler(int sig)
{

}

int main(int argc, char *argv[])
{
    int pid, fd, status, wordSearch;
    int pidA, pidB, result;
    int statusA, statusB;
    int fdArray[2];
    char *progName, *progErrorName;
    char **arguments = NULL;
    struct itimerval alarm = {{0}};
    struct sigaction action = {{0}};
    scoresT score = {0};

    if(argc != 6) {
        perror("Incorrect args\n");
        return (1);
    }

    progErrorName = progErrorNameCreation(argv[4], strlen(argv[4]));
    if(progErrorName == NULL) {
        perror("allocation error\n");
        return (255);
    }

    progName = progNameCreation(argv[4], strlen(argv[4]));
    if(progName == NULL) {
        return (255);
    }

    //Starts P1
    pid = fork();
    if(pid < 0) {
        perror("fork error\n");
        return (255);
    }
    if(pid == 0) {
        fd = open(progErrorName, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
        if(fd < 0) {
            exit(255);
        }
        result = dup2(fd, STDERR_FILENO);
        if(result < 0) {
            return (255);
        }
        close(fd);

        execlp("gcc", "gcc", "-Wall", argv[1], "-o", progName, NULL);

        //If exec fails
        exit(255);
    }
    result = waitpid(pid, &status, 0);
    if(result < 0) {
        perror("waitpid error\n");
        kill(pid, SIGKILL);
        free(progName);
        free(progErrorName);
        return (255);
    }

    if (!WIFEXITED(status)) {
        perror("P1 failed\n");
        free(progName);
        free(progErrorName);
        return (255);
    }
    else if(WEXITSTATUS(status) == 255) {
        perror("P1 failed\n");
        free(progName);
        free(progErrorName);
        return (255);
    }

    wordSearch = compilationErrorCheck(progErrorName);
    if(wordSearch == -2) {
        perror(".err file was not created\n");
        free(progName);
        free(progErrorName);
        return (255);
    }
    else if(wordSearch == -1) {
        //"error" was found in the file
        score.compilation = -100;

        printScores(score);

        free(progName);
        free(progErrorName);
        return (0);
    }
    else if(wordSearch > 0){
        //"warning" was found in the file "wordSearch" times
        score.compilation = -5 * wordSearch;
    }

    //Pipe creation
    result = pipe(fdArray);
	if (result < 0) {
		perror("pipe\n");
		return(1);
	}

    //Starts P2
    pidA = fork();
    if(pidA < 0) {
        perror("fork error\n");
        close(fdArray[0]);
        close(fdArray[1]);
        free(progName);
        free(progErrorName);
        return (255);
    }
    if(pidA == 0) {
        close(fdArray[0]);

        fd = open(argv[3], O_RDONLY, 0);
        if(fd < 0) {
            perror("open failed");
            close(fdArray[1]);
            free(progName);
            free(progErrorName);
            exit(255);
        }
        result = dup2(fd, STDIN_FILENO);
        if(result < 0) {
            perror("dup2 failed\n");
            close(fd);
            close(fdArray[1]);
            free(progName);
            free(progErrorName);
            return (255);
        }
        close(fd);

        result = dup2(fdArray[1], STDOUT_FILENO);
        if(result < 0) {
            perror("dup2 failed\n");
            close(fdArray[1]);
            free(progName);
            free(progErrorName);
            return (255);
        }
        close(fdArray[1]);

        //Arguments creation
        arguments = argumentsInit(arguments, progName);
        if(arguments == NULL) {
            perror("malloc failed\n");
            free(progName);
            free(progErrorName);
            return (255);
        }

        arguments = argumentsArrayCreation(arguments, argv[2]);
        if(arguments == NULL) {
            exit(255);
        }

        //Exexutes progname with progname.args
        execv(arguments[0], arguments);

        //If exec fails
        perror("exec failed\n");
        argumentsArrayClear(arguments);    
        free(progName);
        free(progErrorName);
        return (255);
    }
    close(fdArray[1]);

    //Starts P3
    pidB = fork();
    if(pidB < 0) {
        perror("fork error\n");
        close(fdArray[0]);
        free(progName);
        free(progErrorName);
        return (255);
    }

    if(pidB == 0) {
        result = dup2(fdArray[0], STDIN_FILENO);
        if(result < 0) {
            perror("dup2 failed\n");
            close(fdArray[0]);
            free(progName);
            free(progErrorName);
            return (255);
        }
        close(fdArray[0]);

        execl("./p4diff", "p4diff", argv[4], NULL);

        //If exec fails
        perror("exec failed.\n");
        free(progName);
        free(progErrorName);
        return (255);
    }
    close(fdArray[0]);

    //Creates an alarm for P2
    alarm.it_value.tv_sec = atoi(argv[5]);
	alarm.it_value.tv_usec = 0;
    alarm.it_interval.tv_sec = 0;
    alarm.it_interval.tv_usec = 0;

    result = setitimer(ITIMER_REAL,&alarm,NULL);
    if(result < 0) {
        perror("timer error\n");
        kill(pidA, SIGKILL);
        kill(pidB, SIGKILL);
        free(progName);
        free(progErrorName);
        return (255);
    }

    action.sa_handler = myHandler;
    result = sigaction(SIGALRM, &action, NULL);
    if(result < 0) {
        perror("sigaction error\n");
        kill(pidA, SIGKILL);
        kill(pidB, SIGKILL);
        free(progName);
        free(progErrorName);
        return (255);
    }

    result = waitpid(pidA, &statusA, 0);
    if(result < 0) {
        if((errno == EINTR)) {
            kill(pidA, SIGKILL);
            score.termination = score.termination -100;
        }
        else {
            perror("waitpid error\n");
            kill(pidA, SIGKILL);
            kill(pidB, SIGKILL);
            free(progName);
            free(progErrorName);
            return (255);
        }
    }


    //Stops alarm because P2 returned or SIGALRM was recieved
	alarm.it_value.tv_sec = 0;
	alarm.it_value.tv_usec = 0;
    result = setitimer(ITIMER_REAL,&alarm,NULL);
    if(result < 0) {
        perror("timer error\n");
        kill(pidA, SIGKILL);
        kill(pidB, SIGKILL);
        free(progName);
        free(progErrorName);
        return (255);
    }

    if((errno == EINTR) && (result < 0)) {
        result = waitpid(pidA, &statusA, 0);  
        if(result == -1)  {
            perror("waitpid error\n");
            kill(pidA, SIGKILL);
            kill(pidB, SIGKILL);
            free(progName);
            free(progErrorName);
            return (255);
        } 
    }

    if(WIFEXITED(statusA)) {
        if(WEXITSTATUS(statusA) == 255) {
            perror("P2 failed\n");
            kill(pidB, SIGKILL);
            free(progName);
            free(progErrorName);
            return (255);
        }        
    }
    if(WIFSIGNALED(statusA)){
        if ((WTERMSIG(statusA) == SIGSEGV) || (WTERMSIG(statusA) == SIGABRT) \
                                            || (WTERMSIG(statusA) == SIGBUS)) {
            score.memory -= 15;
        }
    }

    result = waitpid(pidB, &statusB, 0);
    if(result < 0) {
        perror("waitpid error\n");
        kill(pidB, SIGKILL);
        free(progName);
        free(progErrorName);
        return (255);
    }

    if(WIFEXITED(statusB)) {
        if(WEXITSTATUS(statusB) == 255) {
            perror("P3 failed\n");
            free(progName);
            free(progErrorName);
            return (255);
        }
        else {
            score.output = WEXITSTATUS(statusB);
        }
    }
    else {
        perror("P3 failed\n");
        free(progName);
        free(progErrorName);
        return (255);
    }

    printScores(score);

    free(progName);
    free(progErrorName);
    return (0);
}