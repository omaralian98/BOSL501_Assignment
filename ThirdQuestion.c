/*
    This is more like a game rather than a solution for a problem.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
#include<wait.h>
#include<fcntl.h>
#include<string.h>
#include<math.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define TRUE 1
#define FALSE 0
#define Random(min, max) (rand() % (int)((max - min)) + (min))
#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b

#define FORKS_COUNT 3
#define PHILOSOPHERS_COUNT 5


const char* quotes[] = {
    "I think, therefore I am.",
    "Whats the point of nice days when stupid people are allowed to enjoy them too?",
    "What if there ain’t nothing after we die? What if all we have of our loved ones is our memories, and after we’re gone…. there’s nothing.",
    "I am not a vegetarian because I love animals. I am a vegetarian because I hate plants.",
    "I have made this quote longer than usual because I lack the time to make it shorter.",
    "I have not failed. I've just found 10,000 ways that won't work",
    "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.",
    "The opposite of love is not hate, it's indifference. The opposite of art is not ugliness, it's indifference. The opposite of faith is not heresy, it's indifference. And the opposite of life is not death, it's indifference.",
    "Courage is not the absence of fear, courage is acting in spite of fear.",
    "Weak people revenge. Strong people forgive. Intelligent people ignore.",
    "The strongest man in the world is he who stands most alone.",
    "Patience is the strength of the weak, impatience is the weakness of the strong.",
    "People cry, not because they’re weak. It’s because they’ve been strong for too long.",
    "You talk when you cease to be at peace with your thoughts.",
    "The biggest adventure you can take is to live the life of your dreams.",
    "The biggest risk is not taking any risk.",
    "The biggest lie is telling the truth.",
    "The biggest fool is the one who thinks he is wise.",
    "The biggest enemy is not the one who is against you, but the one who is within.",
    "The biggest challenge is not to change the world, but to change yourself.",
    "The biggest reward is not what you get, but what you become.",
    "The biggest regret is not what you did, but what you didn’t do.",
    "The biggest fear is not failure, but success.",
    "The biggest problem is not the problem. The problem is your reaction to the problem.",
    "The biggest lie is that you can’t do something.",
    "Life doesn’t reward the naturally clever or strong but those who can learn to fight and work hard and never quit.",
    "Everyone wants to be strong and self sufficient, but few are willing to put in the work necessary to achieve worthy goals.",
    "Man is the only creature who refuses to be what he is.",
    "There is nothing either good or bad, but thinking makes it so.",
    "It is hard enough to remember my opinions, without also remembering my reasons for them!",
    "The only true wisdom is in knowing you know nothing.",
    "A woman has to live her life, or live to repent not having lived it.",
    "When we are tired, we are attacked by ideas we conquered long ago.",
    "You're not to be so blind with patriotism that you can't face reality. Wrong is wrong, no matter who does it or says it.",
    "If you ever find yourself in the wrong story, leave.",
    "The past has no power over the present moment.",
    "Time is an illusion. Lunchtime doubly so.",
    "The only thing necessary for the triumph of evil is for good men to do nothing.",
    "I cannot teach anybody anything. I can only make them think",
    "A truth that's told with bad intent Beats all the lies you can invent.",
    "Happiness can be found, even in the darkest of times, if one only remembers to turn on the light.",
    "We are addicted to our thoughts. We cannot change anything if we cannot change our thinking.",
    "Tiger got to hunt, bird got to fly; Man got to sit and wonder 'why, why, why?' Tiger got to sleep, bird got to land; Man got to tell himself he understand.",
    "If you win, you need not have to explain...If you lose, you should not be there to explain!",
    "All you need is love. But a little chocolate now and then doesn't hurt.",
    "If more of us valued food and cheer and song above hoarded gold, it would be a merrier world.",
    "One cannot think well, love well, sleep well, if one has not dined well.",
    "The best way to get started is to quit talking and begin doing.",
    "The best and most beautiful things in the world can't be seen or even touched. They must be felt with the heart.",
    "The best way to predict the future is to invent it.",
    "The best revenge is massive success.",
    "The best way to get good ideas is to get a lot of ideas.",
    "Ask not what you can do for your country. Ask what’s for lunch.",
    "The best way to make your dreams come true is to wake up.",
    "The best way to make your life better is to make someone else's life better.",
    "If you only read the books that everyone else is reading, you can only think what everyone else is thinking.",
    "Pain is inevitable. Suffering is optional.",
    "Listen up - there's no war that will end all wars.",
    "After a good dinner one can forgive anybody, even one's own relations.",
    "Pull up a chair. Take a taste. Come join us. Life is so endlessly delicious.",
    "There is no love sincerer than the love of food.",
    "Let food be thy medicine and medicine be thy food.",
    "When I give food to the poor, they call me a saint. When I ask why the poor have no food, they call me a communist.",
    "The best way to get a good meal is to cook it yourself.",
    "The best way to make a good first impression is to improve your last impression.",
    "The best way to make a good friend is to be a good friend.",
    "The best way to make a good enemy is to be a good enemy.",
    "We must have a pie. Stress cannot exist in the presence of a pie.",
}; 

const char* compliments[] = {
    "What a delicious meal!",
    "Who cooked this?",
    "I'm so full!",
    "I wish my wife could cook like this.",
    "Never had such a meal.",
    "What's this dish called?",
    "My honest rating: ★★★★☆."
};

const char* criticism[] = {
    "This is a disaster!",
    "I'm so disappointed.",
    "Unbelievably disgusting!",
    "You call this a meal?",
    "You should never cook again.",
    "Worst than my wife's cooking.",
    "I'd rather eat a shoe.",
    "What a waste of food.",
    "I should have worn my brown pants",
    "My whole life just flashed before my eyes, I gave it 1 star.",
};

int change_chances_chance = 50;

int speed = 40;
int max_critics = 2;
int should_log = TRUE;
int thinking_chance = 15;
int criticism_chance = 15;
int complimenting_chance = 15;
int critics_stop_eating_immediately = FALSE;


sem_t* forks[FORKS_COUNT];
int critics[PHILOSOPHERS_COUNT];

int *health;
pthread_mutex_t health_mutex;
key_t health_shm_key = 1234;

int *stopped_eating;
pthread_mutex_t stopped_eating_mutex;
key_t stopped_eating_shm_key = 125;

int Digits_Count(double number) {
    return (number == 0) ? 1 : (int)log10(number) + 1;
}
void Shuffle(int *array, int size);
void Philosopher(int id);
void Think(int id, int ate);
int Eat(int id);
void Signal_Handler(int sig);
void Finished_Eating_Handler(int sig);
void Initialize_Semaphores();
void Destroy_Semaphores();
void Initialize_Shared_Memory();
void Cleanup_Shared_Memory();
void* Health_Decrease_Thread(void *arg);
int Get_Lowest_Health();
int Set_Health(int index, int newhealth);
int Add_Health(int index, int healthToAdd);
int Should_Eat(int index);
void Set_Parameters(int argc, char** argv); 
void Print_Parameters();


int main(int argc, char** argv) {
    //Seed the Random.
    srand(time(NULL) + getpid() * 21);

    //Set the parameters and print them.
    Set_Parameters(argc, argv);
    Print_Parameters();

    //Set up the signal handlers To properly clean up the resources after terminating the program.
    signal(SIGINT, Signal_Handler);
    signal(SIGUSR1, Finished_Eating_Handler);

    //Initialize the shared memory and semaphores.
    Initialize_Shared_Memory();
    Initialize_Semaphores();

    pthread_t health_thread;
    pthread_create(&health_thread, NULL, Health_Decrease_Thread, NULL);

    //Create a new process for each philosopher.
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            return -1;
        }
        if (pid == 0) {
            Philosopher(i + 1);
            return 0;
        }
    }

    //Wait for the philosophers to finish.
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        wait(NULL);
    }

    //Clean up the resources.
    Cleanup_Shared_Memory();
    Destroy_Semaphores();
    return 0;
}

void Shuffle(int *array, int size) {
    //Fisher–Yates shuffle a simple shuffling algorithm
    for (int i = 0; i < size; i++) {
        int j = Random(0, size);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void Philosopher(int id) {
    //Set a signal handler. this handler is used to override the default behaviour.
    signal(SIGUSR1, Finished_Eating_Handler);

    //Store whether the philosopher has eaten this cycle or not.
    int ate = 1;

    while (TRUE)
    {
        //Seed the random and using the pid since it's always unique.
        srand(time(NULL) + id + Random(1, 20) * getpid());

        //Starts thinking.
        Think(id, ate);

        //Tries to eat.
        ate = Eat(id);

        //If the philosopher want to stop eating. 
        if (ate == -1) {
            //Mark this philosopher as finished eating.
            pthread_mutex_lock(&stopped_eating_mutex);
            stopped_eating[id - 1] = TRUE;
            pthread_mutex_unlock(&stopped_eating_mutex);
            printf("Philosopher #%d doesn't want to eat anymore because the didn't like the food\n", id);
            break;
        }
    }
}

void Think(int id, int ate) {
    //Picking a random thinking time from this range [1, 500] seconds.
    int sleep_time = Random(speed, 500) / speed;

    //If the philosopher hasn't eaten this cycle then don't print.
    if (ate) {
        printf("Philosopher #%d is thinking for %ds\n", id, sleep_time);
    }

    //Randomly decide whether the philosopher will throw a random quote.
    if (Random(1, 101) < thinking_chance) {
        printf("Philosopher #%d says: %s\n", id, quotes[Random(0, (sizeof(quotes) / sizeof(quotes[0])))]);
    }

    //Get the interrupted seconds for logging purposes.
    int interrupted = sleep(sleep_time);
    if (should_log && interrupted != 0) {
        printf("Philosopher #%d 's sleep was interrupted, %ds left to sleep\n",id,  interrupted);
    }
}

int Eat(int id) {
    //Determine if this philosopher should eat or not.
    if (Should_Eat(id - 1) == FALSE) {
        return FALSE;
    } 

    int taken_forks_count = 0;
    int forks_taken[2] = {-1, -1};

    //This section is created for no reason.
    int forks_indices[FORKS_COUNT];
    for (int i = 0; i < FORKS_COUNT; i++) {
        forks_indices[i] = i;
    }

    Shuffle(forks_indices, FORKS_COUNT);

    //Attempt to acquire two forks.
    for (int i = 0; i < FORKS_COUNT && taken_forks_count < 2; i++) {
        int fork_index = forks_indices[i];
        if (sem_trywait(forks[fork_index]) == 0) {
            forks_taken[taken_forks_count++] = fork_index;
        }
    }

    int should_exit = 0;
    
    //If we have acquired two forks.
    if (taken_forks_count == 2) {
        printf("Philosopher #%d took fork %d\n", id, forks_taken[0]);
        printf("Philosopher #%d took fork %d\n", id, forks_taken[1]);

        //Picking a random eating time from this range [1, 500] seconds.
        int sleep_time = Random(speed, 500) / speed;
        printf("Philosopher #%d is eating for %ds\n",id, sleep_time);

        //Log the health for each philosopher
        if (should_log) {
            pthread_mutex_lock(&health_mutex);
            for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
                if (stopped_eating[i]){
                    printf("\e[9m");
                    printf("Philosopher #%d has %d health", i + 1, health[i]);
                    printf("\e[m\n");
                }
                else {
                    printf("Philosopher #%d has %d health\n", i + 1, health[i]);
                }
            }
            pthread_mutex_unlock(&health_mutex);
        }

        sleep(sleep_time);
        printf("Philosopher #%d finished eating.\n", id);
        //Reset the health to 100 after finishing eating.
        Set_Health(id - 1, 100);

        // Critic or compliment
        if (critics[id - 1] && Random(1, 101) < criticism_chance) {
            int index_of_random_criticism = Random(0, (sizeof(criticism) / sizeof(criticism[0])));
            printf("Philosopher #%d says: %s\n", id, criticism[index_of_random_criticism]);

            //Determine if the philosopher wants to exit or not.
            should_exit = critics_stop_eating_immediately;

            //Pick a random penalty for criticizing the food and remove it form 
            //the philosopher's health to make he in need of eating much often.
            int penalty = Random(1, speed / 4);
            Add_Health(id - 1, -penalty);
            if (should_log) {
                printf("Philosopher #%d lost \033[31m%d\e[0m health due to criticism\n", id, penalty);
            }

            //Randomly decide whether to reduce the criticism chance of this philosopher
            if (Random(0, 101) <= change_chances_chance) {
                criticism_chance -= penalty / 2;
                if (should_log && change_chances_chance > 0) {
                    printf("Philosopher #%d 's criticism chance increased by %d\n", id, penalty / 2);
                }
            }
        } 
        else if (Random(1, 101) < complimenting_chance) {
            int index_of_random_compliments = Random(0, (sizeof(compliments) / sizeof(compliments[0])));
            printf("Philosopher #%d says: %s\n", id, compliments[index_of_random_compliments]);

            //Pick a random reward to add it to the philosopher's health.
            int reward = Random(1, speed / 2);
            Add_Health(id - 1, reward);
            if (should_log) {
                printf("Philosopher #%d gained \033[32m%d\e[0m health due to complimenting\n", id, reward);
            }

            if (Random(0, 101) <= change_chances_chance) {
                complimenting_chance += reward / 2;
                if (should_log && complimenting_chance > 0) {
                    printf("Philosopher #%d 's complimenting chance increased by %d\n", id, reward / 2);
                }
            }
        }

        printf("Philosopher #%d put down fork %d\n", id, forks_taken[0]);
        printf("Philosopher #%d put down fork %d\n", id, forks_taken[1]);
    }
    else {
        printf("Philosopher #%d wants to eat but he didn't find enough forks to start eating\n", id);
    }

    //Release any acquired forks whether.
    for (int i = 0; i < taken_forks_count; i++) {
        sem_post(forks[forks_taken[i]]);
    }

    //If the philosopher wants to stop eating.
    if (should_exit) {
        return -1;
    }

    //If the philosopher has finished eating.
    if (taken_forks_count == 2) {
        //Signal to the other philosophers that this philosopher has finished eating.
        //So that they stop thinking and try to eat.
        kill(-getppid(), SIGUSR1);
        return TRUE;
    }
    return FALSE;
}

void Signal_Handler(int sig) {
    //When the user send SIGINIT clean up the recourses.
    Destroy_Semaphores();
    Cleanup_Shared_Memory();
    exit(1);
}

//A signal handler just to override the default behaviour of SIGUSER1
void Finished_Eating_Handler(int sig) {}

//Initialize the semaphores using named semaphores.
void Initialize_Semaphores() {
    for (int i = 0; i < FORKS_COUNT; i++) {
        int digits_count = Digits_Count(i);
        char fork_name[4 + digits_count];
        sprintf(fork_name, "fork%d", i);
        forks[i] = sem_open(fork_name, O_CREAT, 0666, 1);
    }
}

//Destroy the named semaphores.
void Destroy_Semaphores() {
    for (int i = 0; i < FORKS_COUNT; i++) {
        int digits_count = Digits_Count(i);
        char fork_name[4 + digits_count]; 
        sprintf(fork_name, "fork%d", i);
        sem_unlink(fork_name);
    }
}


void Initialize_Shared_Memory() {
    // Create shared memory
    int shm_id = shmget(health_shm_key, PHILOSOPHERS_COUNT * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        printf("Shared memory creation failed\n");
        exit(1);
    }
    health = (int *)shmat(shm_id, NULL, 0);
    if (health == (void *)-1) {
        printf("Failed to attach shared memory\n");
        exit(1);
    }

    //Initialize mutex for health
    pthread_mutex_init(&health_mutex, NULL);

    // Initialize health to 100 for all philosophers
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        health[i] = 100;
    }

    shm_id = shmget(stopped_eating_shm_key, PHILOSOPHERS_COUNT * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        printf("Shared memory creation failed\n");
        exit(1);
    }
    stopped_eating = (int *)shmat(shm_id, NULL, 0);
    if (stopped_eating == (void *)-1) {
        printf("Failed to attach shared memory\n");
        exit(1);
    }

    pthread_mutex_init(&stopped_eating_mutex, NULL);

    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        stopped_eating[i] = FALSE;
    }
}

void Cleanup_Shared_Memory() {
    //Detach and destroy shared memory
    shmdt(health);
    shmdt(stopped_eating);
    pthread_mutex_destroy(&health_mutex);
    pthread_mutex_destroy(&stopped_eating_mutex);
}

//Create this thread to keep decreasing the health of all the philosophers.
void* Health_Decrease_Thread(void *arg) {
    while (TRUE) {
        int sleep_time = Random(speed, 101) / speed;
        sleep(sleep_time);
        pthread_mutex_lock(&health_mutex);
        for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
            if (health[i] > 0) {
                health[i]--;
            }
        }
        pthread_mutex_unlock(&health_mutex);
    }
    return NULL;
}

//This function get's the lowest health among all the philosopher who want to eat.
int Get_Lowest_Health() {
    pthread_mutex_lock(&health_mutex);
    pthread_mutex_lock(&stopped_eating_mutex);
    int min_health = __INT_MAX__;
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        if (health[i] < min_health && stopped_eating[i] == FALSE) {
            min_health = health[i];
        }
    }
    pthread_mutex_unlock(&health_mutex);
    pthread_mutex_unlock(&stopped_eating_mutex);
    return min_health;
}

int Set_Health(int index, int newhealth) {
    pthread_mutex_lock(&health_mutex);
    health[index] = newhealth;
    pthread_mutex_unlock(&health_mutex);
}

int Add_Health(int index, int healthToAdd) {
    pthread_mutex_lock(&health_mutex);
    health[index] += healthToAdd;
    pthread_mutex_unlock(&health_mutex);
}

//Determine whether the philosopher should eat.
//A philosopher should eat when he has the lowest health among the others.
int Should_Eat(int index) {
    int min_health = Get_Lowest_Health();

    pthread_mutex_lock(&health_mutex);
    // Check if the philosopher has the lowest health
    if (health[index] != min_health) {
        pthread_mutex_unlock(&health_mutex);
        return FALSE;
    } 
    pthread_mutex_unlock(&health_mutex);
    return TRUE;
}

//Set the given parameters.
void Set_Parameters(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-speed") == 0 && i + 1 < argc) {
            int given_speed = atoi(argv[i + 1]);
            if (given_speed > 0) {
                speed = given_speed;
            }
            else {
                printf("\033[31mInvalid speed value. Default speed will be used.\e[0m\n");
            }
        }
        else if (strcmp(argv[i], "-log") == 0 && i + 1 < argc) {
            int log = atoi(argv[i + 1]);
            if (log == TRUE || log == FALSE) {
                should_log = log;
            }
            else {
                printf("\033[31mInvalid log value. Default logging will be used.\e[0m\n");
            }
        }        
        else if (strcmp(argv[i], "-mc") == 0 && i + 1 < argc) {
            int max_cr = atoi(argv[i + 1]);
            if (max_cr >= 0 && max_cr <= PHILOSOPHERS_COUNT) {
                max_critics = max_cr;
            } 
            else {
                printf("\033[31mInvalid max critics value. Default max critics will be used.\e[0m\n");
            }
        }
        else if (strcmp(argv[i], "-thinking") == 0 && i + 1 < argc) {
            int thinking_cha = atoi(argv[i + 1]);
            if (thinking_cha >= 0) {
                thinking_chance = MIN(thinking_cha, 100);
            } 
            else {
                printf("\033[31mInvalid thinking chance value. Default thinking chance will be used.\e[0m\n");
            }
        }        
        else if (strcmp(argv[i], "-criticism") == 0 && i + 1 < argc) {
            int criticism_cha = atoi(argv[i + 1]);
            if (criticism_cha >= 0) {
                criticism_chance = MIN(criticism_cha, 100);
            } 
            else {
                printf("\033[31mInvalid criticism chance value. Default criticism chance will be used.\e[0m\n");
            }
        }        
        else if (strcmp(argv[i], "-complimenting") == 0 && i + 1 < argc) {
            int complimenting_cha = atoi(argv[i + 1]);
            if (complimenting_cha >= 0) {
                complimenting_chance = MIN(complimenting_cha, 100);
            }
            else {
                printf("\033[31mInvalid complimenting chance value. Default complimenting chance will be used.\e[0m\n");
            }
        }        
        else if (strcmp(argv[i], "-criticsstop") == 0 && i + 1 < argc) {
            int critics_stop = atoi(argv[i + 1]);
            if (critics_stop == TRUE || critics_stop == FALSE) {
                critics_stop_eating_immediately = critics_stop;
            } 
            else {
                printf("\033[31mInvalid critics stop value. Default critics stop will be used.\e[0m\n");
            }
        }        
    }
}

//Print the acquired parameters or the default ones.
//And make some philosophers hate the food.
void Print_Parameters() {
    printf("\033[36m====================================================\n");
    printf("speed: %d%%\n", speed);
    printf("should_log: %s\n", (should_log ? "true" : "false"));
    printf("max_critics: %d\n", max_critics);
    printf("thinking_chance: %d%%\n", thinking_chance);
    printf("criticism_chance: %d%%\n", criticism_chance);
    printf("complimenting_chance: %d%%\n", complimenting_chance);
    printf("critics_stop_eating_immediately: %s\n", (critics_stop_eating_immediately ? "true" : "false"));

    // At least one philosopher will criticize the food
    int critic_count = Random(0, max_critics) + 1;
    while (critic_count) {
        int critic_index = Random(0, PHILOSOPHERS_COUNT);
        if (critics[critic_index] == 0) {
            critics[critic_index] = 1;
            printf("Philosopher #%d hates the food\n", critic_index + 1);
            critic_count--;
        }
    }
    
    printf("====================================================\e[0m\n\n");
}