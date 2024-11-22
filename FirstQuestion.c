#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sched.h>
#include<wait.h>
#include<signal.h>
#include<stdbool.h>

#define ARRAY_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define Rand(min, max) ((rand() % ((max) - (min) + 1)) + (min))

const bool SHOULD_LOG = false;
const int NUMBER_OF_THREADS = 5;
const int SIGNALS_TO_SEND[5] = { SIGINT, SIGTRAP, SIGKILL, SIGUSR1, SIGCHLD };
const int SIGNALS_COUNT = ARRAY_SIZE(SIGNALS_TO_SEND);

void* Thread(void* p);
void Signal_Handler(int signal);
int Child_Process_Body();

int main() {
    pid_t child_pid = fork();
    if (child_pid < 0) {
        printf("Unexpected error has occured while trying to create a child process\n");
        return -1;
    }
    else if (child_pid == 0) {
        Child_Process_Body();
    }
    else {
        printf("I'm the parent my pid: %d and I'm waiting for the child to finish\n", getpid());
        int child_exit_code;
        wait(&child_exit_code);
        printf("Child has finished with code: %d\n", child_exit_code >> 8);
    }
    return 0;
}

void* Thread(void* p) {
    int thread_id = *(int*)p;
    printf("thread number: #%d started working\n", thread_id);
    while (1)
    {
        int random_signal = SIGNALS_TO_SEND[Rand(0, SIGNALS_COUNT)];
            printf("Thread #%d Selected signal: %d to send\n", thread_id, random_signal);
        if (thread_id != 1 && random_signal == 9) {
            printf("Thread #%d skiped signal: %d\n", thread_id, random_signal);
            continue;
        }
        if (SHOULD_LOG) {
        }
        kill(getpid(), random_signal);
        sched_yield();
        sleep(3);
    }
    
    return 0;
}

void Signal_Handler(int signal) {
    printf("I caught: %d\n", signal);
}

int Child_Process_Body() {
    printf("I'm the Child my pid: %d\n", getpid());
    for (int i = 0; i < SIGNALS_COUNT; i++)
    {
        if (SIGNALS_TO_SEND[i] == SIGKILL) {
            printf("Signal: %d wasn't registerd since it's pointless to do so\n", SIGNALS_TO_SEND[i]);
            continue;
        }
        printf("Signal: %d was registerd\n", SIGNALS_TO_SEND[i]);
        signal(SIGNALS_TO_SEND[i], Signal_Handler);
        sleep(2);
    }
    

    pthread_t threads_ids[NUMBER_OF_THREADS];
    int threads_params[NUMBER_OF_THREADS];
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        threads_params[i] = i + 1;
        pthread_create(&threads_ids[i], NULL, &Thread, (void*)&threads_params[i]);
    }
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_join(threads_ids[i], NULL);
    }
}