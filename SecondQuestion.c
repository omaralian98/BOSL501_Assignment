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

int main() {
    pthread_t client1, client2;
    mkfifo(PIPE_NAME, 0666);
    mkfifo(PIPE_NAME, 0666);

    pthread_create(&client1, NULL, &Thread_Client1, NULL);
    pthread_create(&client2, NULL, &Thread_Client2, NULL);

    pthread_join(client1, NULL);
    pthread_join(client2, NULL);

    return 0;
}

void* Thread_Client1(void* p) {
    int fd;
    char buffer[256];

    while (TRUE) {
        fd = open(PIPE_NAME, O_WRONLY);
        fgets(buffer, sizeof(buffer), stdin);
        write(fd, buffer, strlen(buffer) + 1);
        close(fd);

        fd = open(PIPE_NAME, O_RDONLY);
        read(fd, buffer, sizeof(buffer));
        close(fd);
        printf("Client2: %s\n", buffer);
        sched_yield();
    }
    return 0;
}

void* Thread_Client2(void* p) {
    int fd;
    char buffer[256];

    while (TRUE) {
        fd = open(PIPE_NAME, O_RDONLY);
        read(fd, buffer, sizeof(buffer));
        close(fd);
        printf("Client1: %s\n", buffer);

        fd = open(PIPE_NAME, O_WRONLY);
        fgets(buffer, sizeof(buffer), stdin);
        write(fd, buffer, strlen(buffer) + 1);
        close(fd);
        sched_yield();
    }
    return 0;
}