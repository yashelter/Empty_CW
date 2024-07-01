#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_PRIOR 10

struct msgbuf
{
    long mtype;
    char mtext[100];
};

int set_line(struct msgbuf *line, FILE *in) {
    char buffer[100];

    int chrs = fscanf(in, "prior=%ld text=\"", &line->mtype);
    if (chrs != 1)
    {
        //printf("here %d\n",chrs);
        return 0;
    }

    char symbol = fgetc(in);
    int cnt = 0;
    while (symbol != EOF && symbol != '\n')
    {
        buffer[cnt++] = symbol;
        symbol = fgetc(in);
    }
    buffer[cnt-1] = '\0';
    if (line->mtype > MAX_PRIOR) 
    {
        line->mtype = MAX_PRIOR;
    }
    strcpy(line->mtext, buffer);

    return 2;
}

int main()
{
    key_t key;
    int msgid;

    struct msgbuf message;

    FILE *file = fopen("data.txt", "r");

    if (file == NULL)
    {
        printf("Error occured");
        return 1;
    }


    key = ftok("server", 'S');
    if (key == -1)
    {
        fclose(file);
        printf("Error occured");
        return 1;
    }


    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        fclose(file);
        printf("Error occured");
        return 1;
    }

    int cnt = 0;
    while (set_line(&message, file) == 2)
    {
        printf("Sended : %s\n", message.mtext);
        if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1)
        {
            fclose(file);
            printf("Error occured");
            return 1;
        }
        message.mtype = 1;
        message.mtext[0] = '\0';
        cnt++;
    }
    fclose(file);


    printf("\nAwaiting response\n\n");
    key = ftok("client", 'S');

    if (key == -1)
    {
        printf("Error occured");
        return 1;
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        printf("Error occured");
        return 1;
    }

    while (cnt > 0 && (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) != -1))
    {
        printf("Getted : %s\n", message.mtext);
        cnt--;
    }
    printf("All getted\n");

    // msgctl(msgid, IPC_RMID, NULL); нет смысла, это делает сервер
    return 0;
}