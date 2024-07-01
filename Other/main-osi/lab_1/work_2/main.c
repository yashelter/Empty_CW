#include "application.h"

int main()
{
    int made_users_i = 0;
    struct User **users = (struct User**) malloc(sizeof(struct User*));
    if (users == NULL) { printf("\nNo memory\n"); return 0;}

    
    struct User *user = register_user(users, 1);
    users[0] = user;
    cycle(users, 1, user);

    while (true)
    {
        printf("!hint! Write \'Quit\'  to exit the console !hint!\n");
        char ** input = retister_or_login();

        printf("\n>>> ");
        if (input == NULL) {continue;}

        if ((strcmp(input[0], "Quit")) == 0){ free(input); break; }

        else if (strcmp("Register", input[0]) == 0)
        {
            free_mas(input, 1);
            struct User **new_users = realloc(users, (++made_users_i ) * sizeof(struct User*));
            if (new_users == NULL) { continue;}

            users = new_users;
            user = register_user(users, made_users_i+1);

            if (user == NULL) { printf("Error : something went wrong"); continue; }

            users[made_users_i] = user;
            cycle(users, made_users_i+1, user);
        }

        else if ((strcmp("Login", input[0])) == 0)
        {
            free_mas(input, 1);
            printf("\n << Awaiting login and password : ");
            int l;
            input = get_string(&l);

            if (input == NULL ) {continue;}
            if (l != 2) {printf("Error : something invalid in input\n"); free_mas(input, l);continue;}

            char* logi = input[0];
            int pass = get_int(input[1]);

            enum statements stm1 = validate_login(logi);
            enum statements stm2 = validate_password(pass);

            if (stm1 != correct || stm2 != correct)
            {
                free_mas(input, l);
                printf("Error : invalid parameters\n");
                continue;
            }
            user = find_user(users, made_users_i+1, logi);
            if (user->password == pass){ cycle(users, made_users_i+1, user); }
            else {printf("\nError : passwords did't match\n\n"); }
            free_mas(input, l);
        }
    }
    for (int i = 0; i < made_users_i + 1; i++)
    {
        delete_user(&users[i]);
    }
    free(users);

    printf("\nSuccess : program went to end point\n");
    return 0;
}