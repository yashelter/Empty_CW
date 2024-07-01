#include "shared_memory.c"
#include <signal.h>

#define SEMAPHORE_NAME "/server"
#define SEMAPHORE_CLIENT "/client"

int clients = 1;

int list_files(const char *dirname, char **result, size_t start_position)
{
    DIR *dir;
    struct dirent *entry;
    printf("\n %ld \n", start_position);
    
    dir = opendir(dirname);
    if (dir == NULL)
    {
        printf("Catalog not exist or can't be opened\n");
        return 0;
    }

    start_position += snprintf(*result + start_position, MAX_S - start_position, "Path was \"%-20s\"\n", dirname);

    while ((entry = readdir(dir)) != NULL)
    {
        char path[MAX_S] = "";
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
       
        struct stat file_stat;
        if (stat(path, &file_stat) == -1)
        {
            printf("Error with getting info about file\n");
            closedir(dir);
            return 0;
        }
        start_position += snprintf(*result + start_position, MAX_S - start_position, "%-20s ", entry->d_name);
        // printf("%s\n", entry->d_name);
        if (S_ISDIR(file_stat.st_mode))
        {
            start_position += snprintf(*result + start_position, MAX_S - start_position, "Directory\n");
        }
        else if (S_ISREG(file_stat.st_mode))
        {
            start_position += snprintf(*result + start_position, MAX_S - start_position, "FILE\n");
        }
        else if (S_ISLNK(file_stat.st_mode))
        {
            start_position += snprintf(*result + start_position, MAX_S - start_position, "Link\n");
        }
        else
        {
            start_position += snprintf(*result + start_position, MAX_S - start_position, "Unknown type\n");
        }
    }

    start_position += snprintf(*result + start_position, MAX_S - start_position, "\n\n");

    for (int i = sizeof(int) * 4; i < MAX_S; i++)
    {
        printf("%c", (*result)[i]);
    }
    printf("<%d>\n", (int)start_position);
    closedir(dir);
    return start_position;
}

sem_t *sem;

void clear()
{
    sem_close(sem);
    delete_shm();
}

void signal_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nCleaning\n");
        clear();
        exit(0);
    }
}

int main()
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Failed to register signal handler\n");
        return 1;
    }
    char *readed = malloc(sizeof(char) * (sizeof(int) * 5));
    if (readed == NULL){
        printf("Allocate Error\n");
        return 0;
    }

    int opened = open_shm(O_CREAT);
    if (opened != 0)
    {
        return 1;
    }

    if ((sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 0)) == SEM_FAILED)
    {
        delete_shm();
        printf("sem_open error");
        return 1;
    }
    
    int pos2 = 0;

    write_int_array(readed, &pos2, 0);
    write_int_array(readed, &pos2, 0);
    write_int_array(readed, &pos2, 1);
    write_int_array(readed, &pos2, 0);
    readed[pos2] = '\0';

    set_shm(readed, pos2 + 1);
    free(readed);

    atexit(clear);

    while (1)
    {
        printf("Awaiting...\n");

        sem_wait(sem);

        char *getted = malloc(sizeof(char) * MAX_S);
        if (getted == NULL)
        {
            printf("Allocate problems\n");
            continue;
        }
        memcpy(getted, addr, MAX_S);

        int pos = 0;

        int who = read_int_array(getted, &pos);
        if (who == 0)
        {
            who = clients++;
        }

        int to = read_int_array(getted, &pos);
        if (to != 0)
        {
            free(getted);
            continue;
        }
        int check_code = read_int_array(getted, &pos);
        int len = read_int_array(getted, &pos);

        if (who == 0)
        {
            free(getted);
            printf("Something went wrong\n");
            continue;
        }
        /*
        printf("who: %d\n", who);
        printf("to: %d\n", to);
        printf("check_code: %d\n", check_code);
        printf("len, pos: %d, %d\n", len, pos);
        */
        char *res = malloc(sizeof(char) * MAX_S);
        if (res == NULL)
        {
            free(getted);
            printf("Allocate problems\n");
            break;
        }
        char *line = malloc(sizeof(char) * MAX_S);
        if (line == NULL)
        {
            free(getted);
            free(res);
            printf("Allocate problems\n");
            break;
        }
        int new_counter = sizeof(int) * 4;

        for (int i = pos, cnt = 0; i < pos + len + 1; i++)
        {
            if (getted[i] == '\n' || i == pos + len)
            {
                new_counter = list_files(line, &res, new_counter);
                printf("<%s>\n", line);
                cnt = 0;
                memset(line, 0, sizeof(char) * MAX_S);
            }
            else
            {
                line[cnt++] = getted[i];
            }
        }
        pos = 0;
        write_int_array(res, &pos, 0);
        write_int_array(res, &pos, who);
        write_int_array(res, &pos, 0);
        write_int_array(res, &pos, new_counter - (sizeof(int) * 4));
        
        set_shm(res, MAX_S);

        free(res);
        free(getted);
        free(line);

        // sem_post(clients);
    }
    return 0;
}
