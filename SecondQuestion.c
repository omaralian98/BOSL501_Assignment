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
void Write(const char* buffer);
void Read(char* buffer, size_t buffer_size);

int main() {
    pthread_t client1, client2;

    mkfifo(PIPE_NAME, 0666);

    pthread_create(&client1, NULL, &Thread_Client1, NULL);
    pthread_create(&client2, NULL, &Thread_Client2, NULL);

    pthread_join(client1, NULL);
    pthread_join(client2, NULL);

    return 0;
}

void* Thread_Client1(void* p) {
    char buffer[256];

    while (TRUE) {
        fgets(buffer, sizeof(buffer), stdin);
        Write(buffer);

        Read(buffer, sizeof(buffer));
        printf("Client2: %s\n", buffer);
        sched_yield();
    }
    return 0;
}

void* Thread_Client2(void* p) {
    char buffer[256];

    while (TRUE) {
        Read(buffer, sizeof(buffer));
        printf("Client1: %s\n", buffer);

        fgets(buffer, sizeof(buffer), stdin);
        Write(buffer);
        sched_yield();
    }
    return 0;
}

void Write(const char* buffer) {
    int fd = open(PIPE_NAME, O_WRONLY);
    write(fd, buffer, strlen(buffer) + 1);
    close(fd);
}

void Read(char* buffer, size_t buffer_size) {
    int fd = open(PIPE_NAME, O_RDONLY);
    read(fd, buffer, buffer_size);
    close(fd);
}