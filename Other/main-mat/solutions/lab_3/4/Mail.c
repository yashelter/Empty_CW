#include "String.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    String *city;
    String *street;
    String *index;
    String *segment;

    int house;
    int a_number;
} Adress;

typedef struct
{
    Adress *adress;

    double mass;

    String *post_id;
    String *time_stamp;
    String *post_time;
    String *order_time;

    bool done;
} Mail;

void delete_adress(Adress *ad)
{
    if (ad == NULL)
    {
        return;
    }
    if (ad->city != NULL)
    {
        delete_string(ad->city);
    }
    if (ad->street != NULL)
    {
        delete_string(ad->street);
    }
    if (ad->index != NULL)
    {
        delete_string(ad->index);
    }
    if (ad->segment != NULL)
    {
        delete_string(ad->segment);
    }

    free(ad);
}

void delete_mail(Mail *mail)
{
    if (mail->adress != NULL)
    {
        delete_adress(mail->adress);
    }
    delete_string(mail->post_id);
    delete_string(mail->time_stamp);
    delete_string(mail->post_time);
    delete_string(mail->order_time);
    free(mail);
}
typedef struct
{
    Adress *adress;
    Mail **mails;

    int mails_cnt;
    int _buffer;
} Post;

String *generate_post_id()
{
    srand(time(NULL));
    String *s;
    statements stm = create_string("", &s);
    if (stm != correct)
    {
        return NULL;
    }

    for (int i = 0; i < 14; ++i)
    {
        stm = string_add_char(rand() % 10 + '0', s);
        if (stm != correct)
        {
            delete_string(s);
            return NULL;
        }
    }

    return s;
}

void print_adress(const Adress *adress)
{
    printf("\nCity is : %s\n", adress->city != NULL ? adress->city->value : "Unknown");
    printf("Street is : %s\n", adress->street != NULL ? adress->street->value : "Unknown");
    printf("House name is : %d\n", adress->house);
    printf("Segment is : %s\n", adress->segment != NULL ? adress->segment->value : "Unknown");
    printf("Apartament number is : %d\n", adress->a_number);
    printf("Index is : %s\n", adress->index != NULL ? adress->index->value : "Unknown");
}
void print_mail(Mail *mail)
{
    if (mail == NULL)
    {
        printf("No mail\n");
        return;
    }

    print_adress(mail->adress);
    printf("Weight is : %.2lf\n", mail->mass);
    printf("Post id is : %s\n", mail->post_id != NULL ? mail->post_id->value : "Unknown");
    printf("Time of enter is: %s\n", mail->time_stamp != NULL ? mail->time_stamp->value : "Unknown");
    printf("Expiring time is: %s\n", mail->post_time != NULL ? mail->post_time->value : "Unknown");
    printf("Getted : %s\n", mail->done ? "Yes" : "No");
    printf("Getting time is : %s\n", mail->order_time != NULL ? mail->order_time->value : "Unknown");
    printf("\n");
}

void print_mails(Post *office)
{
    for (int i = 0; i < office->mails_cnt; i++)
    {
        print_mail(office->mails[i]);
    }
}

bool is_number(const char *str)
{
    if (str == NULL || str[0] == '\0')
        return false;

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
    }

    return true;
}

void delete_mails(Mail **m, int cnt)
{
    for (int i = 0; i < cnt; i++)
    {
        delete_mail(m[i]);
    }
    free(m);
}

void delete_post(Post *post)
{
    for (int i = 0; i < post->mails_cnt; i++)
    {
        delete_mail((post->mails)[i]);
    }
    free(post->mails);
    
    delete_adress(post->adress);
}
char *get_сurrent_time()
{
    time_t crnt_time;
    struct tm *info;
    char *timer = (char *)malloc(20 * sizeof(char)); // "dd:MM:yyyy hh:mm:ss"

    if (timer == NULL)
        return NULL;

    time(&crnt_time);
    info = localtime(&crnt_time);

    strftime(timer, 20, "%d:%m:%Y %H:%M:%S", info);

    return timer;
}

int compare_mail(const void *a, const void *b)
{
    const Mail *m1 = (const Mail *)a;
    const Mail *m2 = (const Mail *)b;

    int cmp = strcmp(m1->adress->index->value, m2->adress->index->value);

    cmp = cmp == 0 ? strcmp(m1->post_id->value, m2->post_id->value) : cmp;

    return cmp;
}

int compare_mail_stamp(const void *a, const void *b)
{
    const Mail *mail_1 = (const Mail *)a;
    const Mail *mail_2 = (const Mail *)b;

    return strcmp(mail_1->time_stamp->value, mail_1->time_stamp->value);
}

statements add_mail(Post *post, Mail *mail)
{
    if (post == NULL || mail == NULL)
    {
        return invalid_input;
    }

    if (post->mails_cnt + 1 >= post->_buffer)
    {
        post->_buffer = post->_buffer * 2;
        Mail **temp = (Mail **)realloc(post->mails, sizeof(Mail *) * post->_buffer);
        if (temp == NULL)
        {
            return allocate_error;
        }
        post->mails = temp;
    }

    post->mails[post->mails_cnt] = mail;
    post->mails_cnt = post->mails_cnt + 1;
    qsort(post->mails, post->mails_cnt, sizeof(Mail *), compare_mail);
    return correct;
}

statements search_mail(Post *post, char *post_id, int *result)
{
    *result = -1;
    for (int i = 0; i < post->mails_cnt; i++)
    {
        if (strncmp(post->mails[i]->post_id->value, post_id, 14) == 0)
        {
            *result = i;
            return correct;
        }
    }
    return not_found;
}

statements search_mails(Post *post, Mail ***res1, Mail ***res2, int *l1, int *l2)
{
    Mail **delivered, **delivery_time_expired;
    delivered = (Mail **)malloc(sizeof(Mail *) * post->mails_cnt);
    if (delivered == NULL)
    {
        return allocate_error;
    }

    delivery_time_expired = (Mail **)malloc(sizeof(Mail *) * post->mails_cnt);
    if (delivery_time_expired == NULL)
    {
        free(*delivered);
        return allocate_error;
    }

    int j = 0, k = 0;
    for (int i = 0; i < post->mails_cnt; i++)
    {
        if (post->mails[i]->done)
        {
            delivered[j] = post->mails[i];
            j++;
        }
        else if (strcmp(post->mails[i]->post_time->value, get_сurrent_time()) < 0)
        {
            delivery_time_expired[k] = post->mails[i];
            k++;
        }
    }
    *l1 = j;
    *l2 = k;
    qsort(delivered, j, sizeof(Mail *), compare_mail_stamp);
    qsort(delivery_time_expired, k, sizeof(Mail *), compare_mail_stamp);
    *res1 = delivered;
    *res2 = delivery_time_expired;
    return correct;
}

statements get_b_line(String **result)
{
    char buffer[MIN_BUF];

    fgets(buffer, sizeof(buffer), stdin);
    int len = strlen(buffer) - 1;

    buffer[len] = '\0';
    if (buffer[0] == '\0')
    {
        return invalid_input;
    }
    String *temp;
    statements stm = create_string(buffer, &temp);
    if (stm != correct)
    {
        return stm;
    }
    *result = temp;
    return correct;
}

statements get_int(int *result)
{
    int x;
    char c;
    if (scanf("%d", &x) != 1)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        return invalid_input;
    }
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
    if (x < 0)
    {
        return invalid_input;
    }
    *result = x;
    return correct;
}

statements get_double(double *res)
{
    double x;
    char c;
    if (scanf("%lf", &x) != 1 || x < 0)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        return invalid_input;
    }
    *res = x;
    return correct;
}

void parse_timestamp(char *timestamp, struct tm *tm)
{
    int day, month, year, hour, minute, second;
    if (sscanf(timestamp, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second) != 6)
    {
        // fprintf(stderr, "Ошибка разбора строки\n");
        // exit(EXIT_FAILURE);
    }

    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    tm->tm_hour = hour;
    tm->tm_min = minute;
    tm->tm_sec = second;
}

void skip_empty_safe()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
   {
    }
}
statements create_mail(Post *post, Mail **resulting)
{
    *resulting = NULL;
    Mail *mail = (Mail *)malloc(sizeof(Mail));
    if (mail == NULL)
    {
        return allocate_error;
    }
    mail->adress = (Adress *)malloc(sizeof(Adress));
    if (mail->adress == NULL)
    {
        free(mail);
        return allocate_error;
    }

    mail->adress->city = NULL;
    mail->adress->segment = NULL;
    mail->adress->index = NULL;
    mail->adress->street = NULL;

    mail->order_time = NULL;
    mail->post_id = NULL;
    mail->post_time = NULL;
    mail->time_stamp = NULL;

    *resulting = mail;
    statements stm;

    printf("Enter the city name >>> ");
    stm = get_b_line(&(mail->adress->city));
    if (stm != correct)
    {
        return stm;
    }

    printf("Enter the street name >>> ");
    stm = get_b_line(&(mail->adress->street));
    if (stm != correct)
    {
        return stm;
    }

    printf("Enter the house number >>> ");
    stm = get_int(&(mail->adress->house));
    if (stm != correct)
    {
        return stm;
    }
    if (mail->adress->house < 0)
    {
        return invalid_input;
    }

    printf("Enter the segment house >>> ");
    stm = get_b_line(&(mail->adress->segment));

    if (stm != correct)
    {
        return stm;
    }
    printf("Enter the apartament number >>> ");
    stm = get_int(&(mail->adress->a_number));
    if (stm != correct)
    {
        return stm;
    }
    if (mail->adress->a_number < 0)
    {
        return invalid_input;
    }

    printf("Enter the index >>> ");
    stm = get_b_line(&(mail->adress->index));
    if (stm != correct)
    {
        return stm;
    }
    if (strlen(mail->adress->index->value) != 6 || !is_number(mail->adress->index->value))
    {
        return invalid_input;
    }

    // all adress getted
    //
    //

    printf("Enter the mail mass >>> ");
    stm = get_double(&(mail->mass));
    if (stm != correct || mail->mass < 0)
    {
        return invalid_input;
    }

    String *s = generate_post_id();
    int n;
    while (s != NULL && (search_mail(post, s->value, &n) == correct))
    {
        delete_string(s);
        s = NULL;
        s = generate_post_id();
    }

    if (s == NULL)
    {
        // clean
        return allocate_error;
    }
    mail->post_id = s;

    char *temp = get_сurrent_time();
    if (temp == NULL)
    {
        return allocate_error;
    }
    stm = create_string(temp, &(mail->time_stamp));
    if (stm != correct)
    {
        return stm;
    }
    mail->done = false;
    mail->order_time = NULL;

    struct tm tm;
    // parse_timestamp(mail->time_stamp->value, &tm);

    time_t current_time_t;
    time(&current_time_t);

    time_t delivery_seconds = rand() % 200;

    time_t delivery_deadline_t = current_time_t + delivery_seconds;

    struct tm *info = localtime(&delivery_deadline_t);

    char deadline_buffer[20];
    strftime(deadline_buffer, 20, "%d:%m:%Y %H:%M:%S", info);

    stm = create_string(deadline_buffer, &(mail->post_time));
    if (stm != correct)
    {
        return stm;
    }
    stm = add_mail(post, mail);
    if (stm != correct)
    {
        return stm;
    }
    *resulting = mail;
    return correct;
}

statements track_mail(Post *post)
{
    char c;
    printf("Eneter the id of mail >>> -: ");

    char mail_id[14];
    scanf("%s", mail_id);
    skip_empty_safe();

    int position;

    statements stm = search_mail(post, mail_id, &position);

    if (stm != correct)
    {
        printf("The mail with this id not found\n");
        return correct;
    }
    else if (stm == correct)
    {
        printf("Mail with this id founded\n\n");
        char response;
        printf("\tAll avaliable optons :\n");
        printf(">>> <g> Set as delivered\n");
        printf(">>> <d> delete mail\n");
        printf(">>> <p> print info\n");
        printf(">>> <q> exit from menu\n\n\n");
        while (true)
        {
            printf("-: ");
            scanf(" %c", &response);
            skip_empty_safe();
            if (response == 'g')
            {
                if (post->mails[position]->done){
                    printf("This mail already delivered\n");
                    break;}
                else
                {
                    post->mails[position]->done = true;
                    char *temp = get_сurrent_time();

                    String *str;
                    stm = create_string(temp, &str);
                    if (temp != NULL)
                    {
                        free(temp);
                    }
                    if (stm != correct)
                    {
                        return stm;
                    }
                    post->mails[position]->order_time = str;
                    printf("Marked ad delivered\n");
                    break;
                }
            }
            else if (response == 'p')
            {
                print_mail(post->mails[position]);
                break;
            }
            else if (response == 'd')
            {
                Mail *temp = post->mails[position] ;
                for (int j = position; j < post->mails_cnt - 1; j++)
                {
                    post->mails[j] = post->mails[j + 1];
                }
                post->mails_cnt--;
                delete_mail(temp);
                printf("The mail was deleted\n");
                break;
            }
            else if (response == 'q'){
                 break;
            }
            else
            {
                printf("Plese inter valid parameter\n");
            }
        }
    }
    else
    {
        printf("Occured error : %d", stm);
    }
}
void run(Post *post)
{
    printf("\tAll avaliable commands :\n");
    printf(">>> <c> Create new mail\n");
    printf(">>> <m> Track mail\n");
    printf(">>> <f> Find delivered and expired\n");
    printf(">>> <t> Trace all mails\n");
    printf(">>> <q> exit from programm\n\n");

    while (true)
    {
        printf("\n\nChose the action >>> -:\n");
        char c, input;

        printf("-: ");
        scanf("%c", &input);
        skip_empty_safe();

        if (input == 'c')
        {
            Mail *temp;
            statements stm = create_mail(post, &temp);
            if (stm != correct)
            {
                if (temp != NULL)
                {
                    delete_mail(temp);
                }
                printf("\nError : while entering mail (code %d)\nPress Enter to continue ...", stm);
            }
            skip_empty_safe();
        }
        else if (input == 'm')
        {
            track_mail(post);
        }
        else if (input == 'f')
        {
            Mail **delivered = NULL;
            Mail **failed_date = NULL;
            int cnt1 = 0, cnt2 = 0;
            statements stm = search_mails(post, &delivered, &failed_date, &cnt1, &cnt2);
            if (stm == correct)
            {
                printf("\nDelivered mails : ");
                if (cnt1 == 0)
                {
                    printf("0\nNot any mail was delivered\n");
                }
                else
                {
                    for (int i = 0; i < cnt1; i++)
                    {
                        print_mail(delivered[i]);
                    }
                }
                printf("\nWith failed date : ");
                if (cnt2 == 0)
                {
                    printf("0\nNo one mail was failed\n");
                }
                else
                {
                    for (int i = 0; i < cnt2; i++)
                    {
                        print_mail(failed_date[i]);
                    }
                }
                delete_mails(failed_date, cnt1);
                delete_mails(delivered, cnt2);
            }
            else
            {
                printf("Occured error (code %d)\n\n", stm);
            }
        }
        else if (input == 't')
        {
            if (post->mails_cnt == 0)
                printf("No mails at office\n");
            else
            {
                printf("All tracking mails :\n");
                print_mails(post);
            }
        }
        else if (input == 'q')
        {
            printf("All success\n");
            break;
        }
        else
        {
            printf("<%c>, input", input);
            printf("\n> Invalid input, please re-enter valid command\n");
        }
    }
}