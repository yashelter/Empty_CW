#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h> 

#define N 15
#define LEFT ((i + N - 1) % N)
#define RIGHT ((i + 1) % N )


#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t s[N];
int state[N];

sem_t mutex;

// если вилки стали свободны, говорим что их можно взять
void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        sem_post(&s[i]); // можно брать
    }
}

void take_forks(int i)
{
    sem_wait(&mutex);

    state[i] = HUNGRY;
    test(i); 
    sem_post(&mutex);

    sem_wait(&s[i]);
}


void put_forks(int i)
{
    sem_wait(&mutex);

    state[i] = THINKING;

    test(LEFT);
    test(RIGHT);

    sem_post(&mutex);
}

void *philosopher(void *id)
{
    int i = *((int*)id);
    while (true)
    {           
        printf("Философ %d думает.\n", i+1); // think();
        sleep(rand() % 3); // Философ думает

        printf("Философ %d голоден и хочет взять вилки.\n", i+1); // take_forks(i);
        take_forks(i);

        printf("Философ %d взял левую вилку %d и правую вилку %d и начал есть.\n", i+1, i+1, RIGHT+1);
        sleep(rand() % 3); // Философ ест
        
        printf("Философ %d закончил есть и кладет вилки %d и %d на стол.\n", i+1, i+1, RIGHT+1);
        put_forks(i);
    }
}



int main() {
    srand(time(NULL));

    if (sem_init(&mutex, 0, 1) == -1) {
        perror("Ошибка инициализации семафора");
        return 1;
    }

    pthread_t tid[N];

    for (int i = 0; i < N; i++) {
        sem_init(&s[i], 0, 0); // чтобы сломать тут можно поставить 1
    }

    for (int i = 0; i < N; i++) {
        state[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &state[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }

    sem_destroy(&mutex);

    for (int i = 0; i < N; i++) 
    {
        sem_destroy(&s[i]);
    }

    return 0;
}