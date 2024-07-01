#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SHM_NAME "my_shared_memorys"
#define SHM_SIZE 1024
#define MAX_S 4096
int shm;
char *addr;

int open_shm(int mode)
{
    if ((shm = shm_open(SHM_NAME, mode | O_RDWR, 0777)) == -1){
        return 1;
    }
    if (mode == O_CREAT)
    {
        if (ftruncate(shm, SHM_SIZE + 1) == -1)
        {
            return 1;
        }
    }

    addr = mmap(0, SHM_SIZE + 1, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);

    if (addr == (char *)-1)
    {
        return 1;
    }
    return 0;
}

int set_shm(char *val, int len)
{
    len = len > SHM_SIZE ? SHM_SIZE : len;
    memcpy(addr, val, len);
    addr[len] = '\0';
    return 0;
}

void end_shm()
{
    munmap(addr, SHM_SIZE + 1);
    close(shm);
}

int delete_shm()
{
    end_shm();
    return shm_unlink(SHM_NAME);
}


void write_int_array(char *array, int *position, int number)
{
    for (int i = 0; i < sizeof(int); ++i)
    {
        array[*position + i] = (number >> (8 * i)) & 0xFF;
    }
    *position = *position + sizeof(int);
}

void write_array(char *array, int *position, char *line, int len)
{
    for (int i = 0; i < len; ++i)
    {
        array[*position + i] = line[i];
    }
    *position = *position + len;
}

int read_int_array(const char *array, int *pos)
{
    int number = 0;
    for (int i = 0; i < sizeof(int); ++i)
    {
        number |= ((int)array[*pos + i] & 0xFF) << (8 * i);
    }
    *pos = *pos + sizeof(int);
    return number;
}