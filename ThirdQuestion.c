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

#define TRUE 1
#define Random(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))


#define FORKS_COUNT 3
#define PHILOSOPHERS_COUNT 5
#define MAX_CRITICS 2
#define THINKING_CHANCE 10 
#define CRITICISM_CHANCE 30 
#define COMPLIMENTING_CHANCE 30
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
int speed = 20;

int Digits_Count(double number) {
    return (number == 0) ? 1 : (int)log10(number) + 1;
}
void Philosopher(int id);
void Eat(int id);
void Think(int id);
void Signal_Handler(int sig);
void Initialize_Semaphores();
void Destroy_Semaphores();

int main() {
    signal(SIGINT, Signal_Handler);
    srand(time(NULL) + 21);

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
            perror("Fork failed");
            exit(1);
        }
        if (pid == 0) {
            Philosopher(i + 1);
            exit(0);
        }
    }

    for (int i = 0; i < PHILOSOPHERS_COUNT; i++) {
        wait(NULL);
    }

    Destroy_Semaphores();
    return 0;
}


void Philosopher(int id) {
    while (1) {
        srand(time(NULL) + id);
        Think(id);

        int taken_forks_count = 0;
        int forks_taken[2] = {-1, -1};

        for (int i = 0; i < FORKS_COUNT && taken_forks_count < 2; i++) {
            if (sem_trywait(forks[i]) == 0) {
                forks_taken[taken_forks_count++] = i;
            }
        }

        if (taken_forks_count == 2) {
            printf("Philosopher #%d took fork %d\n", id, forks_taken[0]);
            printf("Philosopher #%d took fork %d\n", id, forks_taken[1]);
            Eat(id);
            printf("Philosopher #%d put down fork %d\n", id, forks_taken[0]);
            printf("Philosopher #%d put down fork %d\n", id, forks_taken[1]);
        }
        else {
            printf("Philosopher #%d didn't find enough forks to start eating\n", id);
        }
        for (int i = 0; i < taken_forks_count; i++)
        {
            sem_post(forks[forks_taken[i]]);
        }
        
    }
}

void Eat(int id) {
    printf("Philosopher #%d is eating.\n", id);
    int sleep_time = Random(0, 1000) / speed;
    sleep(sleep_time);
    printf("Philosopher #%d finished eating.\n", id);

    // Critic or compliment
    if (critics[id - 1]) {
        if (rand() % 100 < CRITICISM_CHANCE) {
            int index_of_random_criticism = rand() % (sizeof(criticism) / sizeof(criticism[0]));
            printf("Philosopher #%d says: %s\n", id, criticism[index_of_random_criticism]);
            if (CRITICS_STOP_EATING_IMMEDIATELY) {
                exit(1);
            }
        }
    } 
    else {
        int index_of_random_compliments = rand() % (sizeof(compliments) / sizeof(compliments[0]));
        printf("Philosopher #%d says: %s\n", id, compliments[index_of_random_compliments]);
    }
}

void Think(int id) {
    printf("Philosopher #%d is thinking.\n", id);

    if (rand() % 100 < THINKING_CHANCE) {
        printf("Philosopher #%d says: %s\n", id, quotes[rand() % (sizeof(quotes) / sizeof(quotes[0]))]);
    }

    int sleep_time = Random(0, 1000) / speed;
    sleep(sleep_time);
}


void Signal_Handler(int sig) {
    Destroy_Semaphores();
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