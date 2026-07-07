/* This program calculates how similar a file is to the input given from STDIN,
 * based on their byte contents.
 * Returns the corresponding percentage
 *
 * Author: Stefanos Manos 
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define READ_BYTES 64

/* specificByteRead(fd, buffer)
 *
 * Reads READ_BYTES number of bytes at most from a file
 * The only time READ_BYTES aren't read is when EOF has been reached
 * 
 * Parameters:
 *      fd: Represents the file from which the bytes will be read
 *      
 *      buffer: Where the read data is stored
 * 
 * Returns: The number of bytes read
 *          "-1" if read failed      
 */
int specificByteRead(int fd, char *buffer)
{
    int result;
    int position = 0;
    int bytes = READ_BYTES;
    
    do {
        result = read(fd, &(buffer[position]), bytes);
        if(result < 0) {
            return (-1);
        }
        bytes -= result;
        position += result;
    } while((result != 0) && (bytes > 0));

    return (position);
}

/* maxInteger(firstNum, secondNum)
 *
 * Compares two numbers
 * 
 * Parameters:
 *      The two integers about to be compared
 * 
 * Returns: The biggest number of the two
 */
int maxInteger(int firstNumber, int secondNumber)
{
    if(firstNumber > secondNumber) {
        return (firstNumber);
    }
    return (secondNumber);
}

/* minInteger(firstNum, secondNum)
 *
 * Compares two numbers
 * 
 * Parameters:
 *      The two integers about to be compared
 * 
 * Returns: The smallest number of the two
 */
int minInteger(int firstNumber, int secondNumber)
{
    if(firstNumber < secondNumber) {
        return (firstNumber);
    }
    return (secondNumber);
}

/* clearBuffer(buffer)
 *
 * Clears a string of size READ_BYTES (sets all contents to '\0')
 *
 * Parameters:
 *      buffer: The string cleared
 * 
 * Returns: Nothing(void function)
 */
void clearBuffer(char *buffer)
{
    int i;

    for(i = 0; i < READ_BYTES; i++) {
        buffer[i] = '\0';
    }
}

int main(int argc, char *argv[])
{
    int fd, readResult, scanResult, i;
    char fileBuffer[READ_BYTES];
    char inputBuffer[READ_BYTES];
    int numExpectedBytes = 0;
    int numCreatedBytes = 0;
    int similarBytes = 0;
    int result;
    int score;

    if(argc != 2) {
        return(255);
    }

    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
        return (255);
    }

    while(1) {
        clearBuffer(fileBuffer);
        clearBuffer(inputBuffer);
        
        readResult = specificByteRead(fd, fileBuffer);
        if(readResult == -1) {
            close(fd);
            return (255);
        }

        scanResult = specificByteRead(STDIN_FILENO, inputBuffer);
        if(scanResult == -1) {
            close(fd);
            return (255);
        }
        
        numExpectedBytes += scanResult;
        numCreatedBytes += readResult;

        if((readResult == 0) && (scanResult == 0)) {
            break;
        }

        result = minInteger(readResult, scanResult);

        for(i = 0; i < result; i++) {
            if(fileBuffer[i] == inputBuffer[i]) {
                similarBytes++;
            }
        }
    }
    if((numExpectedBytes == 0) && (numCreatedBytes == 0)) {
        score = 100;
    }
    else {
        score = similarBytes * 100 / maxInteger(numExpectedBytes, numCreatedBytes);
    }
    return (score);
}