/* 
Notes:
    - For this assignment, we only need one pipe since there will never be simultaneous writing to it.
      Client1 and Client2 will alternate their roles: Client1 will write while Client2 reads, and vice versa.

    - When a client reads from the pipe, the read operation will block until the other client writes to the pipe,
      utilizing the built-in blocking mechanizm of the read() function.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define TRUE 1
#define PIPE_NAME "/tmp/secondquestion"

void* Thread_Client1(void* p);
void* Thread_Client2(void* p);
void Write_To_Pipe(const char* buffer);
void Read_From_Pipe(char* buffer, size_t buffer_size);

int main() {
    pthread_t client1, client2;

    //Initializes the pipe
    mkfifo(PIPE_NAME, 0666);

    //Creates the threads
    pthread_create(&client1, NULL, &Thread_Client1, NULL);
    pthread_create(&client2, NULL, &Thread_Client2, NULL);

    //Waits for the thread to finish
    pthread_join(client1, NULL);
    pthread_join(client2, NULL);

    return 0;
}

void* Thread_Client1(void* p) {
    //Declares the buffer which the thread will use to write to the pipe and read from it
    char buffer[256];

    while (TRUE) {
        //Gets input from the user
        fgets(buffer, sizeof(buffer), stdin);
        Write_To_Pipe(buffer);

        Read_From_Pipe(buffer, sizeof(buffer));
        printf("Client2: %s\n", buffer);

        //Gives chance to other process to execute
        sched_yield();
    }
    return 0;
}

void* Thread_Client2(void* p) {
    char buffer[256];

    while (TRUE) {
        Read_From_Pipe(buffer, sizeof(buffer));
        printf("Client1: %s\n", buffer);

        fgets(buffer, sizeof(buffer), stdin);
        Write_To_Pipe(buffer);
        sched_yield();
    }
    return 0;
}

void Write_To_Pipe(const char* buffer) {
    //Opens file to write only
    int fd = open(PIPE_NAME, O_WRONLY);
    //Writes the given readonly-buffer to the pipe
    write(fd, buffer, strlen(buffer) + 1);
    //Closes the file
    close(fd);
}

void Read_From_Pipe(char* buffer, size_t buffer_size) {
    //Opens file to read only
    int fd = open(PIPE_NAME, O_RDONLY);
    //Reads from the file and stores it in the given buffer
    read(fd, buffer, buffer_size);
    //Closes the file
    close(fd);
}