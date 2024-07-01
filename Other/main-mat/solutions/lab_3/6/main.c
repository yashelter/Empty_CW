#include "functions.c"


int main(int argc, char** argv) 
{
    Buses_list main_list;

    if(argc < 2)
    {
        printf("too few arguments\n");
        return 1;
    }
    main_list.main_head = NULL;

    for(int i = 1; i < argc; ++i)
    {
        switch(parsing_input_data(argv[i], &main_list))
        {
            case allocate_error:
                printf("MEMORY ALLOCATION PROBLEM\n");
                free_list_buses(&main_list);
                return allocate_error;
            case invalid_file:
                printf("Problem with open file: %s\n", argv[i]);
                free_list_buses(&main_list);
                return invalid_file;
            case invalid_input:
                break;
            case empty_file:
                printf("Enter empty file: %s\n", argv[i]);
                free_list_buses(&main_list);
                return empty_file;
            case empty_string:
                printf("There is empty string in file!\n");
                free_list_buses(&main_list);
                return empty_string;
            case invalid_coords:
                printf("Same coordination in different files\n");
                free_list_buses(&main_list);
                return invalid_coords;
            case correct:
                break;
        }
    }
    if(check_all(&main_list) == invalid)
    {
        printf("Invalid input\n");
        free_list_buses(&main_list);
        return 0;
    }
    switch (check_time_bus(&main_list)) 
    {
        case same_time_ad:
            printf("Error: You have entered the same arrival and departure time for one bus.\n");
            free_list_buses(&main_list);
            return same_time_ad;
        case same_time:
            printf("Error: You have entered the same time of arrival and departure for one bus.\n");
            free_list_buses(&main_list);
            return same_time;
        case same_time_for_departure:
            printf("Error: You have entered the same time of departure.\n");
            free_list_buses(&main_list);
            return same_time_for_departure;
        case same_time_for_arrive:
            printf("Error: You have entered the same time of arrival.\n");
            free_list_buses(&main_list);
            return same_time_for_arrive;
        case departure_early_arrive:
            printf("Error: You have entered a departure time earlier than the arrival time for one bus.\n");
            free_list_buses(&main_list);
            return departure_early_arrive;
        case departure_bigger_arriving:
            printf("Error: You have entered a departure time later than or equal to some other arrival time.\n");
            free_list_buses(&main_list);
            return departure_bigger_arriving;
        default:
            break;
    }


    switch (check_coordination(&main_list))
    {
        case invalid:
            printf("Error: You have entered the same coordinates for two consecutive stops of the same bus.\n");
            free_list_buses(&main_list);
            return invalid;
        case correct:
            break;
    }
    run(&main_list);
    free_list_buses(&main_list);
}