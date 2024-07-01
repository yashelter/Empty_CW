#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

#define MAX_TEXT_SIZE 100
#define MAX_MESSAGES 10
#define MAX_PRIOR 10
#define MAX_ITERATIONS 10

void process_text(char *text)
{
    int i, j = 0;
    for (i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == ' ' || text[i] == '\t')
        {
            text[j++] = ' ';
            while (text[i] == ' ' || text[i] == '\t')
            {
                i++;
            }
        }
        text[j++] = text[i];
    }
    text[j] = '\0';
}

struct msgbuf
{
    long mtype;
    char mtext[100];
};

key_t key, client;
int msgid, client_msgid;

void clear()
{
    msgctl(msgid, IPC_RMID, NULL);
    msgctl(client_msgid, IPC_RMID, NULL);
    printf("Server stopped\n\n");
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
    key = ftok("server", 'S');
    if (key == -1)
    {
        printf("Error occured");
        return 1;
    }

    client = ftok("client", 'S');
    if (key == -1)
    {
        printf("Error occured");
        return 1;
    }

    msgid = msgget(key, 0666 | IPC_CREAT); // id отправки (чтения) на сервер
    if (msgid == -1)
    {
        printf("Error occured");
        return 1;
    }

    client_msgid = msgget(client, 0666 | IPC_CREAT); // id клиента
    if (client_msgid == -1)
    {
        msgctl(msgid, IPC_RMID, NULL);
        printf("Error occured");
        return 1;
    }

    atexit(clear);

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Failed to register signal handler\n");
        return 1;
    }

    struct msgbuf message;
    int need = MAX_ITERATIONS;

    while (1)
    {
        for (int i = MAX_PRIOR; i >= 0; i--)
        {
            while (msgrcv(msgid, &message, sizeof(message.mtext), i, IPC_NOWAIT) != -1)
            {
                printf("Received message: %s\n", message.mtext);
                process_text(message.mtext);
                if (msgsnd(client_msgid, &message, sizeof(message.mtext), 0) == -1)
                {
                    printf("Getted conflict OS problem\n");
                    return 1;
                }
                else
                {
                    printf("Succesfully returned : %s\n\n", message.mtext);
                }
            }
        }
        int sec = rand() % 9;
        printf("Waiting for %d\n", sec);
        sleep(sec);
        printf("Continue...\n\n");
        //need--;
    }

    return 0;
}
