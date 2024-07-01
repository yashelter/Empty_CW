#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define N 1024

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file_with_filenames>\n", argv[0]);
        return 1;
    }

    // Создаем разделяемую память для результатов

    
    key_t shm_key = ftok("/tmp", 'a');
    int shm_id = shmget(shm_key, (argc - 1) * sizeof(int), IPC_CREAT | 0666);
    int *mas = (int *)shmat(shm_id, NULL, 0);

    for (int i = 0; i < argc - 1; i++)
    {
        mas[i] = 0;
    }

    char line[N];
    char search_string[N];
    printf("Enter the search string: ");
    fgets(search_string, sizeof(search_string), stdin);

    search_string[strlen(search_string) - 1] = '\0';
    FILE *files = fopen(argv[1], "r");

    if (files == NULL) {return 1;}
    char path[1024];
    int i = -1;
    while (fgets(path, sizeof(path), files) != NULL && (++i > -1))
    {
        path[strlen(path) - 1] = '\0';
        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            printf("Fork failed");
            return 1;
        }
        else if (child_pid == 0)
        {
            //printf("This is the child process.\n");
            FILE *file = fopen(path, "r");
            if (file == NULL)
            {
                printf("Error opening file\n");
                exit(0);
            }

            while (fgets(line, sizeof(line), file) != NULL)
            {
                if (strstr(line, search_string) != NULL)
                {
                    mas[i] = 1;
                    // можно вывести тут
                    fclose(file);
                    exit(0);
                }
            }
            fclose(file);
            exit(0);

        } else
        {
            //printf("This is the parent process. Child PID: %d\n", child_pid);
        }
    }

    // Ожидание завершения дочерних процессов
    for (int i = 0; i < argc - 1; i++)
    {
        int status;
        wait(&status);
    }

    for (int i = 0; i < argc - 1; i++)
    {
        if (mas[i] == 1)
        {
            printf("Found in file %s\n", argv[i + 1]);
        }
    }

    // Отсоединяемся от разделяемой памяти
    shmdt(mas);
    // Удаляем разделяемую память
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
