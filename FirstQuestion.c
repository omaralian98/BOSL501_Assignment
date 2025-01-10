#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sched.h>
#include<wait.h>
#include<signal.h>

#define TRUE 1
#define ARRAY_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define Random(min, max) (rand() % (int)((max - min)) + (min))
#define SIGNALS_COUNT 5

const int NUMBER_OF_THREADS = 5;
const int SIGNALS_TO_SEND[SIGNALS_COUNT] = { SIGINT, SIGTRAP, SIGUSR1, SIGKILL, SIGCHLD };

pid_t child_pid;

void* Thread(void* p);
void Signal_Handler(int signal);

int main() {
    //Seed the Random.
    srand(time(NULL) + getpid() + 21);

    //Create a child process.
    child_pid = fork();

    //If the child wasn't created.
    if (child_pid < 0) {
        printf("Unexpected error has occurred while trying to create a new process\n");
        return -1;
    }


    if (child_pid == 0) {
        //If this is the child process.
        printf("I'm the Child my pid: %d\n", getpid());
        pthread_t threads_ids[NUMBER_OF_THREADS];
        int threads_params[NUMBER_OF_THREADS];

        //Create NUMBER_OF_THREADS threads.
        for (int i = 0; i < NUMBER_OF_THREADS; i++) {
            threads_params[i] = i + 1;
            pthread_create(&threads_ids[i], NULL, &Thread, (void*)&threads_params[i]);
        }

        //Wait for them to finish.
        for (int i = 0; i < NUMBER_OF_THREADS; i++) {
            pthread_join(threads_ids[i], NULL);
        }
    }
    else {
        //If it's the parent process.
        printf("I'm the parent my pid: %d\n", getpid());

        //Register the signal handlers.
        for (int i = 0; i < SIGNALS_COUNT; i++) {
            printf("Signal: %d was registered\n", SIGNALS_TO_SEND[i]);
            signal(SIGNALS_TO_SEND[i], &Signal_Handler);
        }
        
        //Wait for the child to finish.
        printf("I'm waiting for the child to finish\n");
        int child_exit_code;
        wait(&child_exit_code);
        //Report how was the child process exited.
        printf("Child has finished with code: %d\n", child_exit_code >> 8);
    }
    return 0;
}

void* Thread(void* p) {
    //Get the Id
    int thread_id = *(int*)p;
    printf("Thread number: #%d started working\n", thread_id);
    sleep(2);
    pid_t parent_id = getppid();
    while (TRUE) {
        //Select a random signal.
        int random_signal = SIGNALS_TO_SEND[Random(0, SIGNALS_COUNT)];
        printf("Thread #%d Selected signal: %d to send to %d\n", thread_id, random_signal, parent_id);
        
        //If the selected signal is SIGKILL, send it to the parent but as negative.
        //Sending to a negative process will kill the process and all of its children
        //Without this the parent will die and the child would become an orphan, after
        //that the OS would adopt it and re-parent it, and it will keep sending signals 
        //and crashes the system.
        if (random_signal == SIGKILL) {
            kill(-parent_id, SIGKILL);
        }
        kill(parent_id, random_signal);

        //Give other process the chance to use the cpu.
        sched_yield();
    }
    
    return 0;
}

void Signal_Handler(int signal) {
    //If the signal is SIGKILL send SIGCHILD to the child.
    if (signal == SIGKILL) {
        printf("Parent process received SIGKILL signal.\nSending SIGKILL to child.\n");
        kill(child_pid, SIGCHLD);
        sched_yield();
    }
    printf("Parent process received %d from %d.\n", signal, child_pid);
}