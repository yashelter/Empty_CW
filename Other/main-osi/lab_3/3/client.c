#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include "shared_memory.c"
#include <stdbool.h>

#define SEMAPHORE_NAME "/server"

int main()
{

    sem_t *sem;
    open_shm(0);

    if ((sem = sem_open(SEMAPHORE_NAME, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    printf("Sending data\n");

    char *data_to_send = malloc(sizeof(char) * MAX_S);
    char *readed = malloc(sizeof(char) * (sizeof(int) * 5));

    char *line = "/";
    int pos = 0, pos2 = 0;

    write_int_array(data_to_send, &pos, -1);
    write_int_array(data_to_send, &pos, 0);
    write_int_array(data_to_send, &pos, 0);
    write_int_array(data_to_send, &pos, strlen(line));

    write_array(data_to_send, &pos, line, strlen(line));

    write_int_array(readed, &pos2, -1);
    write_int_array(readed, &pos2, 0);
    write_int_array(readed, &pos2, 1);
    write_int_array(readed, &pos2, 0);

    data_to_send[pos] = '\0';
    readed[pos2] = '\0';

    bool sended = 0;

    while (1)
    {
        printf("Looking for opportunity...\n");

        char *getted = malloc(sizeof(char) * MAX_S);
        if (getted == NULL)
        {
            printf("Allocate problems\n");
            continue;
        }

        memcpy(getted, addr, MAX_S);
        int position = 0;
        int who = read_int_array(getted, &position);
        int to = read_int_array(getted, &position);
        int check_code = read_int_array(getted, &position);
        int len = read_int_array(getted, &position);

        free(getted);
        //printf("%d", check_code);
        if (sended)
        {
            if (who == 0)
            {
                for (int i = sizeof(int) * 4; i < MAX_S; i++)
                {
                    printf("%c", addr[i]);
                }
                set_shm(readed, pos2 + 1);
                break;
            }
            
        }
        else if (check_code == 1)
        {
            set_shm(data_to_send, pos+1);
            sem_post(sem);
            printf("Sended, waiting result\n");
            sended = 1;
        }
        sleep(3);
    }
    sem_close(sem);
    free(readed);
    free(data_to_send);
    end_shm();
    return 0;
}
