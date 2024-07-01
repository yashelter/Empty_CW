#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("Идентификатор текущего процесса (PID): %d\n", getpid());
    printf("Идентификатор родительского процесса (PPID): %d\n", getppid());
    printf("Идентификатор группы вызывающего процесса (GID): %d\n", getpgrp());
    printf("Реальный идентификатор владельца (UID): %d\n", getuid());
    printf("Реальный идентификатор группы владельца (GID): %d\n", getgid());
    printf("Эффективный идентификатор владельца (EUID): %d\n", geteuid());
    printf("Эффективный идентификатор группы владельца (EGID): %d\n", getegid());
    return 0;
}