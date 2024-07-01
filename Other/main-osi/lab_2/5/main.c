#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h> 

#define COUNT 7

#define WAITING 0
#define INSIDE 1
#define RELAX 2
#define ENTERING 3

bool inside_man = false;
bool inside_women = false;

int N = 0;
int MaxN = 0;

sem_t men[COUNT];
sem_t women[COUNT];

int state_men[COUNT];
int state_women[COUNT];

sem_t mutex;

void try_enter_man(int i)
{
    int n = 0;
    for (int i = 0; i < COUNT; i++)
    {
        if (state_women[i] == INSIDE)
        {
            return;
        }
    }
    //printf("Empty or only men\n");
    for (int i = 0; i < COUNT; i++)
    {
        if (state_men[i] == INSIDE)
        {
            n++;
        }
    }
    if (state_men[i] == WAITING && n < MaxN)
    {
        state_men[i] = INSIDE;
        sem_post(&men[i]);
    }
}



void try_enter_woman(int i)
{
    int n = 0;
    for (int i = 0; i < COUNT; i++)
    {
        if (state_men[i] == INSIDE)
        {
            return;
        }
    }
    //printf("Empty or only women\n");
    for (int i = 0; i < COUNT; i++)
    {
        if (state_women[i] == INSIDE)
        {
            n++;
        }
    }
    if (state_women[i] == WAITING && n < MaxN)
    {
        state_women[i] = INSIDE;
        sem_post(&women[i]);
    }
}

void pass_post()
{
    for (int i = 0; i < COUNT; i++)
    {
        try_enter_woman(i);
        try_enter_man(i);
    }
}


void woman_wants_to_enter(int i)
{
    sem_wait(&mutex);

    state_women[i] = WAITING;
    pass_post(); 

    sem_post(&mutex);

    sem_wait(&women[i]);
}


void woman_leaves(int i)
{
    sem_wait(&mutex);

    state_women[i] = RELAX;
    pass_post();

    sem_post(&mutex);
}


///


void man_wants_to_enter(int i)
{
    sem_wait(&mutex);

    state_men[i] = WAITING;
    pass_post();

    sem_post(&mutex);

    sem_wait(&men[i]);
}


void man_leaves(int i)
{
    sem_wait(&mutex);

    state_men[i] = RELAX;
    pass_post();

    sem_post(&mutex);
}


void *real_man(void *id)
{
    int i = *((int*)id);
    while (true)
    {           
        //printf("Наслаждается жизнью парень %d\n", i+1); 
        sleep(rand() % 10);

        printf("Кун %d решил  занять душ\n", i+1);
        man_wants_to_enter(i);

        printf("Кун %d в душе\n", i+1);
        sleep(rand() % 10); // Чилит
        
        printf("Кун %d вышел\n", i+1);
        man_leaves(i);
        sleep(rand() % 10);
    }
}

void *real_woman(void *id)
{
    int i = *((int*)id);
    while (true)
    {           
        //printf("Тян %d грустит\n", i+1); 
        sleep(rand() % 10);

        printf("Тян %d решила занять душ\n", i+1);
        woman_wants_to_enter(i);

        printf("Тян %d в душе\n", i+1);
        sleep(rand() % 10); // долга
        
        printf("Тян %d вышла\n", i+1);
        woman_leaves(i);
        sleep(rand() % 11);
    }
}


int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    if (sem_init(&mutex, 0, 1) == -1) {
        perror("Ошибка инициализации семафора");
        return 1;
    }

    for (int i = 0; i < COUNT; i++) 
    {
        if (sem_init(&men[i], 0, 0) == -1) {
            perror("Ошибка инициализации семафора");
            return 1;
        }

        if (sem_init(&women[i], 0, 0) == -1) {
            perror("Ошибка инициализации семафора");
            return 1;
        }

        state_men[i] = RELAX;
        state_women[i] = RELAX;
    }

    MaxN = atoi(argv[1]); // Maximum number of people

    pthread_t threads[COUNT*2];
    int men_indices[COUNT];
    int women_indices[COUNT];

    for (int i = 0; i < COUNT; i++) 
    {
        men_indices[i] = i;
        women_indices[i] = i;

        pthread_create(&threads[i], NULL, real_woman, &women_indices[i]);
        pthread_create(&threads[COUNT+i], NULL, real_man, &men_indices[i]);
    }

    for (int i = 0; i < COUNT; i++)
     {
        
        pthread_join(threads[i], NULL);
        pthread_join(threads[COUNT+i], NULL);
    }
    sem_destroy(&mutex);

    for (int i = 0; i < COUNT; i++) 
    {
        sem_destroy(&men[i]);
        sem_destroy(&women[i]);
    }
    return 0;
}