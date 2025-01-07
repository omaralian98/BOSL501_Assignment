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

#define FORKS_COUNT 3
#define PHILOSOPHERS_COUNT 5
#define MAX_CRITICS 2
#define THINKING_CHANCE 50 
#define CRITICISM_CHANCE 100 
#define COMPLIMENTING_CHANCE 100
#define CRITICS_STOP_EATING_IMMEDIATELY TRUE

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
    "Worst than my ex-wife's cooking.",
    "I'd rather eat a shoe.",
    "What a waste of food.",
    "I should have worn my brown pants",
    "My whole life just flashed before my eyes, I gave it 1 star.",
};

sem_t* forks[FORKS_COUNT];
int critics[PHILOSOPHERS_COUNT];
int speed = 60;

int *health;
pthread_mutex_t health_mutex;
key_t health_shm_key = 1234;

int *stopped_eating;
pthread_mutex_t stopped_eating_mutex;
key_t stopped_eating_shm_key = 125;

int Digits_Count(double number) {
    return (number == 0) ? 1 : (int)log10(number) + 1;
}
void Philosopher(int id);
void Eat(int id);
void Think(int id);
void Signal_Handler(int sig);
void Initialize_Semaphores();
void Destroy_Semaphores();
void Cleanup_Shared_Memory();
void Initialize_Shared_Memory();
void* Health_Decrease_Thread(void *arg);
int Get_Lowest_Health();
int Reset_Health(int index);
int Should_Eat(int index);

int main(int argc, char **argv) {
    int given_speed = 0; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-speed") == 0 && i + 1 < argc) {
            given_speed = atoi(argv[i + 1]);
            break;  
        }
    }

    if (given_speed > 0 && given_speed <= 100) {
        printf("Set %d as speed\n", given_speed);
        speed = given_speed;
    } 

    signal(SIGINT, Signal_Handler);
    srand(time(NULL) + 21);

    Initialize_Shared_Memory();
    pthread_t health_thread;
    pthread_create(&health_thread, NULL, Health_Decrease_Thread, NULL);

    Initialize_Semaphores();

    // At least one philosopher will criticize the food
    int critic_count = Random(0, MAX_CRITICS - 1) + 1;
    for (int i = 0; i < critic_count; i++) {
        int critic_id = rand() % PHILOSOPHERS_COUNT;
        critics[critic_id] = 1;
        printf("Philosopher #%d hates the food\n", critic_id + 1);
    }

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

    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        wait(NULL);
    }

    Destroy_Semaphores();
    Cleanup_Shared_Memory();
    return 0;
}


void Philosopher(int id) {
    while (1) {
        srand(time(NULL) + id + Random(1, 20));
        Think(id);
        Eat(id);
    }
}

void Eat(int id) {
    if (Should_Eat(id - 1) == FALSE) {
        return;
    } 

    int taken_forks_count = 0;
    int forks_taken[2] = {-1, -1};

    for (int i = 0; i < FORKS_COUNT && taken_forks_count < 2; i++) {
        if (sem_trywait(forks[i]) == 0) {
            forks_taken[taken_forks_count++] = i;
        }
    }
    int should_exit = 0;
    
    if (taken_forks_count == 2) {
        printf("Philosopher #%d took fork %d\n", id, forks_taken[0]);
        printf("Philosopher #%d took fork %d\n", id, forks_taken[1]);
        printf("Philosopher #%d is eating.\n", id);
        pthread_mutex_lock(&health_mutex);
        for (int i = 0; i < PHILOSOPHERS_COUNT; i++)
        {
            printf("Philosopher #%d has %d health\n", i + 1, health[i]);
        }
        pthread_mutex_unlock(&health_mutex);

        int sleep_time = Random(speed, 1000) / speed;
        sleep(sleep_time);
        printf("Philosopher #%d finished eating.\n", id);

        // Critic or compliment
        if (critics[id - 1]) {
            if (rand() % 100 < CRITICISM_CHANCE) {
                int index_of_random_criticism = rand() % (sizeof(criticism) / sizeof(criticism[0]));
                printf("Philosopher #%d says: %s\n", id, criticism[index_of_random_criticism]);
                should_exit = CRITICS_STOP_EATING_IMMEDIATELY;
            }
        } 
        else {
            int index_of_random_compliments = rand() % (sizeof(compliments) / sizeof(compliments[0]));
            printf("Philosopher #%d says: %s\n", id, compliments[index_of_random_compliments]);
        }
        printf("Philosopher #%d put down fork %d\n", id, forks_taken[0]);
        printf("Philosopher #%d put down fork %d\n", id, forks_taken[1]);
        Reset_Health(id - 1);
    }
    else {
        printf("Philosopher #%d didn't find enough forks to start eating\n", id);
    }

    for (int i = 0; i < taken_forks_count; i++)
    {
        sem_post(forks[forks_taken[i]]);
    }

    if (should_exit) {
        pthread_mutex_lock(&stopped_eating_mutex);
        stopped_eating[id - 1] = TRUE;
        pthread_mutex_unlock(&stopped_eating_mutex);
        exit(1);
    }
}

void Think(int id) {
    printf("Philosopher #%d is thinking.\n", id);

    if (rand() % 100 < THINKING_CHANCE) {
        printf("Philosopher #%d says: %s\n", id, quotes[rand() % (sizeof(quotes) / sizeof(quotes[0]))]);
    }

    int sleep_time = Random(speed, 1000) / speed;
    sleep(sleep_time);
}


void Signal_Handler(int sig) {
    Destroy_Semaphores();
    Cleanup_Shared_Memory();
    exit(1);
}

void Initialize_Semaphores() {
    for (int i = 0; i < FORKS_COUNT; i++) {
        int digits_count = Digits_Count(i);
        char fork_name[4 + digits_count];
        sprintf(fork_name, "fork%d", i);
        forks[i] = sem_open(fork_name, O_CREAT, 0666, 1);
    }
}

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

void* Health_Decrease_Thread(void *arg) {
    while (TRUE) {
        int sleep_time = Random(speed, 100) / speed;
        sleep(sleep_time);
        pthread_mutex_lock(&health_mutex);
        for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
            if (health[i] > 0) {
                health[i]--;  // Decrease health by 1
            }
        }
        pthread_mutex_unlock(&health_mutex);
    }
    return NULL;
}

int Get_Lowest_Health() {
    pthread_mutex_lock(&health_mutex);
    pthread_mutex_lock(&stopped_eating_mutex);
    int min_health = 101;
    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        if (health[i] < min_health && stopped_eating[i] == FALSE) {
            min_health = health[i];
        }
    }
    pthread_mutex_unlock(&health_mutex);
    pthread_mutex_unlock(&stopped_eating_mutex);
    return min_health;
}

int Reset_Health(int index) {
    pthread_mutex_lock(&health_mutex);
    health[index] = 100;
    pthread_mutex_unlock(&health_mutex);
}

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