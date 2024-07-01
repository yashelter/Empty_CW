#include "manager.h"

struct User
{
    char* login;
    int password;
    int queries;
    int max_queries;
};

void delete_user(struct User **user)
{
    if (*user != NULL) 
    {
        free((*user)->login);
        free(*user);
        *user = NULL;
    }
}

enum statements validate_login(char *login)
{
    int l = strlen(login);
    if (l > 6 || l < 1){ return invalid_login; }
    
    for(int i = 0; i < l; i++)
    {
        if (!isalpha(login[i]) && !isdigit(login[i])){ return invalid_login;}
    }
    return correct;
}

enum statements validate_password(int password)
{
    if (password > 100000 || password < 0) {return invalid_password; }
    return correct;
}


// if null = err
struct User* generate_user(char *login, int password, struct User **users, int l)
{
    enum statements stm = validate_login(login);

    if (stm != correct) {free(login); return NULL;}

    stm = validate_password(password);
    if (stm != correct) {free(login); return NULL;}

    for (int i = 0; i < l; i++)
    {
        if (strcmp(login, users[i]->login) == 0) { return NULL; }
    }


    struct User *new_user = (struct User*) malloc(sizeof(struct User));
    if (new_user == NULL) {free(login); return NULL; }

    new_user->login = login;
    new_user->password = password;
    new_user->max_queries = -1;
    return new_user;
}

struct User* register_user(struct User** users, int l)
{
    struct User* user = NULL;
    while (user == NULL)
    {
        printf("\n\n");
        print_hint();
        int l;
        char** input = get_string(&l);

        if (l != 2)
        {
            printf("\n<< Invalid parameters\n\n");
            free_mas(input, l);
            continue;
        }

        user = generate_user(input[0], get_int(input[1]), users, l);
        if (user == NULL) { printf("\n Please check for validness\n\n"); } 
    }
    return user;
}
struct User* find_user(struct User** users, int cnt, char* login)
{
    for (int i = 0; i < cnt; i++)
    {
        if (strcmp(users[i]->login, login) == 0){ return users[i]; }
    }
    return NULL;
}

int print_current_time_f(char *timeString, struct tm *localTimeInfo)
{
    return strftime(timeString, 50, "%H:%M:%S", localTimeInfo);
}

int print_current_date_f(char *timeString, struct tm *localTimeInfo)
{
    return strftime(timeString, 50, "%d-%m-%Y", localTimeInfo);
}

void time_print(int (*strformat)(char*, struct tm*), char *word)
{
    time_t currentTime;
    struct tm localTimeInfo;
    char timeString[50];

    currentTime = time(NULL);

    if (currentTime == -1) {
        printf("Error: something went wrong, cannot get %s\n", word);
        return;
    }

    if (localtime_s(&localTimeInfo, &currentTime) != 0) {
        printf("Error: cannot get %s\n", word);
        return;
    }

    if (strformat(timeString, &localTimeInfo) == 0) {
        printf("Error: cannot format %s\n", word);
        return;
    }

    printf("Current %s is: %s\n", word, timeString);
}

void print_current_time() { time_print(print_current_time_f, "time"); }
void print_current_date() { time_print(print_current_date_f, "date"); }



void how_much_time(struct tm* input, int base_format)
{
    time_t currentTime;

    struct tm localTimeInfo;
    char timeString[50];
    
    time(&currentTime);

    if (currentTime == -1) 
    {
        printf("Error: something went wrong, cannot get time \n");
        return;
    }
    errno = 0;

    if (localtime_s(&localTimeInfo, &currentTime) != 0) 
    {
        printf("%lld ", currentTime);
        printf("Error: cannot get time");
        return;
    }
    localTimeInfo.tm_hour = 0;
    localTimeInfo.tm_min = 0;
    localTimeInfo.tm_sec = 0;

    if (input == NULL) { printf("<< Error : can't get diff time\n"); return; }
    
    long long startTime = mktime(input), endTime = mktime(&localTimeInfo);

    long long currentTimeInSeconds = endTime - startTime;
    if (currentTimeInSeconds == 1696712401) {printf("<< Seems like something incorrect \n"); return;}
    
    printf("From input time to now : %lld in input format", currentTimeInSeconds / base_format);

}

bool set_sanctions(struct User **mas, int mas_cnt, char* login, int num){
    struct User *user = find_user(mas, mas_cnt, login);
    if (user == NULL) {return false; }
    user->max_queries = num;
    return true;
}

void cycle (struct User **mas, int mas_cnt, struct User *current_User)
{
    if (current_User == NULL) { return; }
    if (current_User->max_queries != -1) { current_User->queries = current_User->max_queries; }

    printf("Succesfully authorized\n\n");
    while (true)
    {
        printf("\n>>> ");
        int len = 0;

        char** querys = get_string(&len);

        if (querys == NULL) { printf("<< Getted Error : in input \n"); continue;}
        char* query = querys[0];

        if (strcmp(query, "Logout") == 0) { free_mas(querys, len); break; }

        else if (current_User->max_queries != -1 && current_User->queries < 1)
        {
            printf("\n<< You get max queries, please \'Logout\'");
        }

        else if (strcmp(query, "Time") == 0) 
        { 
            print_current_time(); 
            if (current_User->max_queries != -1) {current_User->queries--;}
        
        }
        else if (strcmp(query, "Date") == 0) 
        { 
            print_current_date(); 
            if (current_User->max_queries != -1) {current_User->queries--;}
        }

        else if (strncmp(query, "Howmuch", 7) == 0) 
        { 
            if (len != 3) {printf("<< Getted Error : in input \n"); }
            else
            {
                //printf("\n Debug : \'%s\'  \'%s\'  \'%s\'\n", query, querys[1], querys[2]);
                struct tm *time = get_time(querys[1]);

                if (time == NULL) {printf("<< Error : invalid input\n");free_mas(querys, len); continue; }

                int base;
                
                enum statements stm = get_time_format(querys[2], &base);
                
                if (time != NULL && stm == correct)
                {
                    how_much_time(time, base);
                    if (current_User->max_queries != -1) {current_User->queries--;}
                }
            }
        }
        else if (strncmp(query, "Sanctions", 9) == 0) 
        {
            if (len != 3) { printf("<< Getted Error : in input \n"); }
            else
            {
                //printf("\n Debug : \'%s\'  \'%s\'  \'%s\'\n", query, querys[1], querys[2]);
                int lk;
                printf("Awaiting acception : ");
                char** acept = get_string(&lk);
                printf("\n");
                if (lk != 1 && strlen(acept[0]) != 5 && strncmp(query, "12345", 5) ) 
                {
                    printf("<< Error : not accepted\n\n");
                    free_mas(acept, lk);
                }
                else
                {
                    enum statements stm = validate_login(querys[1]);
                    int num = get_int(querys[2]);

                    if (num > 0 && stm == correct)
                    {
                        bool setted = set_sanctions(mas, mas_cnt, querys[1], num);
                        if (!setted) { printf("<< Error No such user found\n\n"); }
                        else {printf("<< Sacttions gived succesfully\n\n");}
                        if (current_User->max_queries != -1) {current_User->queries--;}
                    }
                }
            }
             if (current_User->max_queries != -1) {current_User->queries--;} 

             
        }
        else { printf("\n<< Command is unsupported \n"); }
        if (querys != NULL) { free_mas(querys, len);}

       
    }
}