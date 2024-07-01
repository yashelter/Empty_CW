#include "struct.h"

#define BUFFER_SIZE 512
#define EPS 1e-6

void clear_buffer(FILE* file) 
{
    int ch;
    while (((ch = fgetc(file)) != '\n') && ch != EOF);
}

void print_in_list(Track_lst* list)
{
    track_path* ptr = list->master_track;
    while(ptr != NULL)
    {
        printf("Number of current bus is: %s\n", ptr->position->bus_id != NULL ? ptr->position->bus_id : "Unknown");
        printf("X coordinate is: %lf\n", ptr->position->x);
        printf("Y coordinate is: %lf\n", ptr->position->y);
        printf("Bus arrival time: %s\n", ptr->position->arrive != NULL ? ptr->position->arrive : "Unknown");
        printf("Bus departure time: %s\n", ptr->position->departure != NULL ? ptr->position->departure : "Unknown");
        printf("Status of current bus: %s\n\n", ptr->position->status == START ? "START" : ptr->position->status == FINAL ? "FINAL" : "INTERMEDIATE");
        ptr = ptr->route_ptr;
    }
}

void printings(Buses_list* list)
{
    int count = 0;
    main_node* ptr = list->main_head;
    while(ptr != NULL){
        print_in_list(&(ptr->track_lst));
        printf("\n");
        count++;
        ptr = ptr->main_next;
    }
}

int check_coordination_file_in(Track_lst* list, double x, double y)
{
    if(list == NULL) return -1;

    int count = 0;
    track_path* ptr = list->master_track;
    while(ptr != NULL)
    {
        if(fabs(ptr->position->x - x ) < EPS && fabs(ptr->position->y - y) < EPS) count++;
        ptr = ptr->route_ptr;
    }
    return count;
}


int check_coordination_file(Buses_list* list, double x, double y)
{
    if(list == NULL) return -1;

    int count = 0;
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        int current = check_coordination_file_in(&(ptr->track_lst), x, y);
        if(current == -1) return -1;
        if(current != 0) count++;
        ptr = ptr->main_next;
    }
    return count;
}

double read_and_convert_to_doub(statements* status, FILE* file) 
{
    char line[BUFFER_SIZE];
    if (!fgets(line, sizeof line, file)) 
    {
        *status = invalid_input;
        return 0.0;
    }

    int len = (int)strlen(line);
    if (line[len - 1] != '\n') clear_buffer(file);

    // Проверка наличия символа новой строки в строке
    char* newline_ptr = strchr(line, '\n');
    if (newline_ptr != NULL) *newline_ptr = '\0';  // Заменяем символ новой строки на конец строки

    if (line[0] == '\n' && line[1] == '\0') 
    {
        *status = empty_string;
        return 0.0;
    }

    char* end;
    errno = 0;
    double val = strtod(line, &end);
    
    if (errno == ERANGE || end != line + len - 1 || (*end != '\0' && *end != '\n')) 
    {
        *status = invalid_input;
        return 0.0;
    }

    *status = correct;
    return val;
}

char* read_check_line(statements* status, FILE* file)
{
    int size = 0, buffer = 128;
    char* result = (char*)malloc(sizeof(char) * (buffer));
    if(result == NULL) { *status = allocate_error; return NULL; }

    char c;
    while((c = fgetc(file)) != EOF && c != '\n')
    {
        size++;
        if(size == buffer)
        {
            buffer *= 2;
            char* ptr = (char*) realloc(result, sizeof(char) * (buffer));
            if(ptr == NULL)
            {
                free(result);
                *status = allocate_error;
                return NULL;
            }
            else result = ptr;
        }
        result[size - 1] = c;
    }
    result[size] = '\0';

    if(size == 0 && c == EOF)
    {
        *status = invalid_input;
        free(result);
        return NULL;
    }
    if(strcmp(result, "\0") == 0 || strcmp(result, "\n") == 0)
    {
        *status = empty_string;
        free(result);
        return NULL;
    }
    *status = correct;
    return result;
}

statements add_in(Track_lst* r_list, Position* position)
{
    if(r_list == NULL || position == NULL) return invalid;

    track_path* new_node = create_route_node(position);
    if(new_node == NULL) return allocate_error;

    if(r_list->master_track == NULL)
    {
        r_list->master_track = new_node;
        return correct;
    }
    else if(strcmp(r_list->master_track->position->arrive, position->arrive) > 0)
    {
        new_node->route_ptr = r_list->master_track;
        r_list->master_track = new_node;
        return correct;
    }
    else 
    {
        track_path* ptr = r_list->master_track;
        while (ptr->route_ptr != NULL && strcmp(ptr->route_ptr->position->arrive, position->arrive) < 0){ ptr = ptr->route_ptr; }
        new_node->route_ptr = ptr->route_ptr;
        ptr->route_ptr = new_node;
        return correct;
    }
}

main_node* find_bus_number(Buses_list* list, char* number_bus)
{
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        if((strcmp(ptr->track_lst.master_track->position->bus_id, number_bus)) == 0) return ptr;
        ptr = ptr->main_next;
    }
    return NULL;
}

statements add_out(Buses_list* list, Position* position)
{
    statements status;
    main_node* new_node = find_bus_number(list, position->bus_id);
    if(new_node == NULL)
    {
        new_node = create_main_node();
        if(new_node == NULL) return allocate_error;
        status = add_in(&(new_node->track_lst), position);
        if(status != correct)
        {
            free(new_node);
            return status;
        }
        if(list->main_head == NULL)
        {
            list->main_head = new_node;
            return correct;
        }
        else
        {
            main_node* ptr = list->main_head;
            while(ptr->main_next != NULL) { ptr = ptr->main_next;}
            ptr->main_next = new_node;
            return correct;
        }
    }
    else
    {
        status = add_in(&(new_node->track_lst), position);
        return status;
    }
}

statements parsing_input_data(char* filename, Buses_list* list) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) return invalid_file;
    int c = fgetc(file);
    if (c == EOF) { fclose(file); return empty_file;}
    else fseek(file, 0, SEEK_SET);

    double x, y;
    statements status = correct;
    x = read_and_convert_to_doub(&status, file);
    if (status != correct) { fclose(file); return status; }
    y = read_and_convert_to_doub(&status, file);
    if (status != correct) { fclose(file); return status;}

    int check_mark = check_coordination_file(list, x, y);
    if(check_mark != 0)
    {
        fclose(file);
        return invalid_coords;
    }
    do
    {
        char* str = read_check_line(&status, file);
        if(status != correct)
        {
            fclose(file);
            return status;
        }
        char* bus_number = strtok(str, " ");
        char* stop_date = strtok(NULL, " ");
        char* stop_time = strtok(NULL, " ");
        char* go_date = strtok(NULL, " ");
        char* go_time = strtok(NULL, " ");
        char* type = strtok(NULL, " ");

        struct tm stop, go;
        char buffer1[20], buffer2[20];
        strptime(stop_date, "%d.%m.%Y", &stop);
        strptime(stop_time, "%H:%M:%S", &stop);
        strftime(buffer1, 20, "%d.%m.%Y %H:%M:%S", &stop);
        strptime(go_date, "%d.%m.%Y", &go);
        strptime(go_time, "%H:%M:%S", &go);
        strftime(buffer2, 20, "%d.%m.%Y %H:%M:%S", &go);

        status_arriving bs_st = !strcmp(type, "S\n") || !strcmp(type, "S") ? START : !strcmp(type, "I\n") || !strcmp(type, "I") ? INTERMEDIATE : FINAL;

        Position* cur_bus = (Position*)malloc(sizeof(Position));
        if(cur_bus == NULL)
        {
            fclose(file);
            free(str);
            return allocate_error;
        }
        cur_bus->status = bs_st;
        cur_bus->x = x;
        cur_bus->y = y;
        cur_bus->arrive = (char*) malloc(sizeof(char) * strlen(buffer1));
        if(cur_bus->arrive == NULL)
        {
            fclose(file);
            free(cur_bus); free(str);
            return allocate_error;
        }
        strcpy(cur_bus->arrive, buffer1);
        cur_bus->departure = (char*) malloc(sizeof(char)*(strlen(buffer2) + 1));
        if(cur_bus->departure == NULL)
        {
            fclose(file);
            free(cur_bus->arrive); free(cur_bus); free(str);
            return allocate_error;
        }
        strcpy(cur_bus->departure, buffer2);
        cur_bus->bus_id = (char*) malloc(sizeof(char)*(1 + strlen(bus_number)));
        if(cur_bus->bus_id == NULL)
        {
            fclose(file);
            free(cur_bus->arrive); free(cur_bus->departure); free(cur_bus); free(str);
            return allocate_error;
        }
        strcpy(cur_bus->bus_id, bus_number);

        statements statuss = add_out(list, cur_bus);
        if(statuss == allocate_error)
        {
            fclose(file);
            free(cur_bus->arrive); free(cur_bus->departure); free(cur_bus); free(str);
            return allocate_error;
        }
        if(statuss == invalid)
        {
            fclose(file);
            free(cur_bus->arrive); free(cur_bus->departure); free(cur_bus); free(str);
            return invalid_input;
        }
        free(str);
    }
    while(status == correct);

    fclose(file);
    return correct;
}

void free_list_route(Track_lst* list)
{
    track_path* ptr;
    while(list->master_track != NULL)
    {
        ptr = list->master_track;
        list->master_track = list->master_track->route_ptr;
        free(ptr->position->bus_id);free(ptr->position->arrive);
        free(ptr->position->departure); free(ptr->position);
        free(ptr);
    }
}

void free_list_buses(Buses_list* list)
{
    main_node* ptr;
    while (list->main_head != NULL)
    {
        ptr = list->main_head;
        list->main_head = list->main_head->main_next;
        free_list_route(&ptr->track_lst); free(ptr);
    }
}

statements check_coordination_crnt_bus(Track_lst* list)
{
    track_path* ptr = list->master_track;
    while(ptr != NULL && ptr->route_ptr != NULL)
    {
        track_path* ptr2 = ptr->route_ptr;
        if(fabs(ptr2->position->x - ptr->position->x) < EPS && fabs(ptr2->position->y - ptr->position->y) < EPS) return invalid;
        ptr = ptr->route_ptr;
    }
    return correct;
}

statements check_coordination(Buses_list* list)
{
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        switch (check_coordination_crnt_bus(&(ptr->track_lst))) 
        {
            case invalid: return invalid;
            case correct: break;
        }
        ptr = ptr -> main_next;
    }
    return correct;
}



bus_statements check_time_crnt_bus(Track_lst* list)
{
    track_path* ptr = list->master_track;
    while(ptr != NULL && ptr->route_ptr != NULL)
    {
        track_path* ptr2 = ptr->route_ptr;
        int time_arrive = strcmp(ptr->position->arrive, ptr2->position->arrive);
        int time_departure = strcmp(ptr->position->departure, ptr2->position->departure);
        int time_all = strcmp(ptr->position->arrive, ptr->position->departure);

        if(time_arrive == 0 && time_departure == 0) return same_time;
        if(time_arrive == 0) return same_time_for_arrive;
        if(time_departure == 0) return same_time_for_departure;
        if(time_all == 0 || strcmp(ptr2->position->arrive, ptr2->position->departure) == 0) return same_time_ad;
        if(time_all > 0 || strcmp(ptr2->position->arrive, ptr2->position->departure) > 0) return departure_early_arrive;
        ptr = ptr->route_ptr;
    }

    track_path* ptrr = list->master_track;
    while(ptrr != NULL)
    {
        track_path* ptr_2 = ptrr->route_ptr;
        while(ptr_2 != NULL)
        {
            if(strcmp(ptrr->position->departure, ptr_2->position->departure) == 0) return same_time_for_departure;
            ptr_2 = ptr_2->route_ptr;
        }
        ptrr = ptrr->route_ptr;
    }

    ptr = list->master_track;
    while(ptr != NULL)
    {
        ptrr = ptr->route_ptr;
        while(ptrr != NULL)
        {
            if(strcmp(ptrr->position->arrive, ptr->position->departure) <= 0) return departure_bigger_arriving;
            ptrr = ptrr->route_ptr;
        }
        ptr = ptr->route_ptr;
    }
    return time_correct;
}

bus_statements check_time_bus(Buses_list* list)
{
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        bus_statements status = check_time_crnt_bus(&(ptr->track_lst));
        switch (status) 
        {
            case time_correct: break;
            default: return status;
        }
        ptr = ptr->main_next;
    }
    return time_correct;
}


statements check_in_list(Track_lst* list)
{
    int start, finish = 0;
    if(list->master_track->position->status != START) return invalid;
    else start = 1;

    track_path* ptr = list->master_track;
    track_path* pre_final = NULL;
    while(ptr->route_ptr != NULL)
    {
        pre_final = ptr;
        ptr = ptr->route_ptr;
        if( ptr->position->status == FINAL && start)
        {
            start = 0;
            finish = 1;
            continue;
        }
        else if(ptr->position->status == START && finish){
            finish = 0;
            start = 1;
            continue;
        }
        else if(ptr->route_ptr == NULL) break;
        else if(ptr->position->status == INTERMEDIATE && start && !finish) continue;
        else return invalid;
    }

    if(ptr->position->status != FINAL)return invalid;
    if(pre_final->position->status == FINAL && ptr->position->status == FINAL) return invalid;
    return correct;
}

statements check_all(Buses_list* list)
{
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        switch (check_in_list(&(ptr->track_lst))) {
            case invalid: return invalid;
            case correct: break;
        }
        ptr = ptr->main_next;
    }
    return correct;
}

int count_route_in(Track_lst* list)
{
    if(list == NULL) return -1;

    int count = 0;
    track_path* ptr = list->master_track;
    while(ptr != NULL){
        if(ptr->position->status == START || ptr->position->status == FINAL) count++;
        ptr = ptr -> route_ptr;
    }
    if(count % 2 != 0)return -1;
    count /= 2;
    return count;
}

char* bus_id_route(Buses_list* list, statements* status, max_min type)
{
    if(list == NULL)
    {
        *status = invalid; return NULL;
    }
    int max = -1;
    int min = INT_MAX;
    main_node* max_route = NULL;
    main_node* min_route = NULL;
    main_node* ptr = list->main_head;
    while(ptr != NULL){
        int current = count_route_in(&(ptr->track_lst));
        if(current == -1)
        {
            *status = invalid;
            return NULL;
        }
        if(current >= max)
        {
            max = current;
            max_route = ptr;
        }
        if(current <= min)
        {
            min = current;
            min_route = ptr;
        }
        ptr = ptr -> main_next;
    }

    if(max_route == NULL || min_route == NULL)
    {
        *status = invalid;
        return NULL;
    }
    else
    {
        if(type == MAX) return max_route->track_lst.master_track->position->bus_id;
        if(type == MIN) return min_route->track_lst.master_track->position->bus_id;
    }
    return NULL;
}

double length_by_x_y(track_path* first, track_path* second)
{
    if(first == NULL || second == NULL)return -1;
    return sqrt((first->position->x - second->position->x) * (first->position->x - second->position->x) + (first->position->y - second->position->y) * (first->position->y - second->position->y));
}

double track_length_in(Track_lst* list)
{
    if(list == NULL)return -1.0;

    double length = 0l;
    track_path* ptr = list->master_track;
    while(ptr->route_ptr != NULL)
    {
        track_path* ptr2 = ptr->route_ptr;
        double current = length_by_x_y(ptr, ptr2);
        if(fabs(current + 1) < EPS) return -1l;
        length += current;
        ptr = ptr->route_ptr;
    }
    return length;
}

char* bus_id_length_track(Buses_list* list, statements* status, max_min type, double* len)
{
    if(list == NULL) { *status = invalid; return NULL;}

    double max = -1l;
    double min = MAXFLOAT;
    main_node* max_route = NULL;
    main_node* min_route = NULL;
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        double current = track_length_in(&(ptr->track_lst));
        if(fabs(current + 1) < EPS) { *status = invalid; return NULL;}
        if(current > max)
        {
            max = current;
            max_route = ptr;
        }
        if(current < min)
        {
            min = current;
            min_route = ptr;
        }
        ptr = ptr -> main_next;
    }
    if(max_route == NULL || min_route == NULL) { *status = invalid; return NULL;}
    else
    {
        if(type == MAX)
        {
            *len = max;
            return max_route->track_lst.master_track->position->bus_id;
        }
        else if(type == MIN)
        {
            *len = min;
            return min_route->track_lst.master_track->position->bus_id;
        }
    }
    return NULL;
}

double longest_route(Track_lst* list, max_min type)
{
    if(list == NULL) return -1;

    double max = -1l, min = MAXFLOAT, size_route = 0l;
    track_path* ptr = list->master_track;
    while(ptr != NULL && ptr->route_ptr != NULL)
    {
        track_path* ptr2 = ptr->route_ptr;
        double current = length_by_x_y(ptr, ptr2);
        if(current == -1) return -1;
        size_route += current;
        if(ptr2->position->status == FINAL)
        {
            if(size_route > max) max = size_route;
            if(size_route < min) min = size_route;
            size_route = 0l;
            ptr = ptr2->route_ptr;
            continue;
        }
        ptr = ptr->route_ptr;
    }
    if(type == MAX) return max;
    if(type == MIN) return min;
    return 0.0;
}

char* max_route_length_bus_id (Buses_list* list, statements* status, max_min type, double* len){
    if(list == NULL) { *status = invalid; return NULL;}

    double max = -1l, min = MAXFLOAT;
    main_node* max_route = NULL;
    main_node* min_route = NULL;
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        double current = longest_route(&(ptr->track_lst), type);
        if(fabs(current + 1) < EPS)
        {
            *status = invalid;
            return NULL;
        }
        if(current > max)
        {
            max = current;
            max_route = ptr;
        }
        if(current < min)
        {
            min = current;
            min_route = ptr;
        }
        ptr = ptr -> main_next;
    }
    if(max_route == NULL || min_route == NULL){ *status = invalid; return NULL;}
    else
    {
        if(type == MAX)
        {
            *len = max;
            return max_route->track_lst.master_track->position->bus_id;
        }
        if(type == MIN)
        {
            *len = min;
            return min_route->track_lst.master_track->position->bus_id;
        }
    }
    return NULL;
}

double long_short_stop_in(Track_lst* list, max_min type)
{
    if(list == NULL) return -1;

    double max = -1l;
    double min = MAXFLOAT;
    track_path* ptr = list->master_track;
    while(ptr != NULL)
    {
        struct tm in, out;
        strptime(ptr->position->arrive, "%d.%m.%Y %H:%M:%S", &in);
        strptime(ptr->position->departure, "%d.%m.%Y %H:%M:%S", &out);
        tzset();
        in.tm_isdst = -1;
        out.tm_isdst = -1;
        time_t first = mktime(&in);
        time_t second = mktime(&out);

        double difference = difftime(second, first);

        if(difference > max ) max = difference;
        if(difference < min) min = difference;
        ptr = ptr->route_ptr;
    }
    if(type == MAX) return max;
    if(type == MIN) return min;
    return 0.0;
}

char* long_short_stop (Buses_list* list, statements* status, max_min type, double* len)
{
    if(list == NULL) { *status = invalid; return NULL; }

    double max = -1l, min = MAXFLOAT;
    main_node* max_route = NULL;
    main_node* min_route = NULL;
    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        double current = long_short_stop_in(&(ptr->track_lst), type);
        if(fabs(current + 1) < EPS) { *status = invalid; return NULL; }

        if(current > max)
        {
            max = current;
            max_route = ptr;
        }
        if(current < min)
        {
            min = current;
            min_route = ptr;
        }
        ptr = ptr -> main_next;
    }
    if(max_route == NULL || min_route == NULL) { *status = invalid; return NULL;}
    else
    {
        if(type == MAX)
        {
            *len = max;
            return max_route->track_lst.master_track->position->bus_id;
        }
        if(type == MIN)
        {
            *len = min;
            return min_route->track_lst.master_track->position->bus_id;
        }
    }
    return NULL;
}

double stop_time_in(Track_lst* list)
{
    if(list == NULL) return -1;
    double sum = 0l;

    track_path* ptr = list->master_track;
    while(ptr != NULL)
    {
        struct tm in, out;
        strptime(ptr->position->arrive, "%d.%m.%Y %H:%M:%S", &in);
        strptime(ptr->position->departure, "%d.%m.%Y %H:%M:%S", &out);

        tzset();
        in.tm_isdst = -1;
        out.tm_isdst = -1;
        time_t first = mktime(&in);
        time_t second = mktime(&out);

        double difference = difftime(second, first);
        sum += difference;
        ptr = ptr->route_ptr;
    }
    return sum;
}

char* stop_time(Buses_list* list, statements* st, double* len)
{
    if(list == NULL){ *st = invalid; return NULL;}
    double max = -1l;

    main_node* max_route = NULL;

    main_node* ptr = list->main_head;
    while(ptr != NULL)
    {
        double current = stop_time_in(&(ptr->track_lst));
        if(fabs(current + 1) < EPS) { *st = invalid; return NULL; }
        if(current > max)
        {
            max = current;
            max_route = ptr;
        }
        ptr = ptr -> main_next;
    }
    if(max_route == NULL)
    {
        *st = invalid;
        return NULL;
    }
    else
    {
        *len = max;
        return max_route->track_lst.master_track->position->bus_id;
    }
}

void run(Buses_list* list)
{
    while(1) 
    {
        printf("\nChoose action:\n");
        printf("a - the bus with the number of routes\n");
        printf("c - the bus with the track length\n");
        printf("e - the bus with the route length\n");
        printf("g - the bus with the stop length\n");
        printf("w - the bus with the downtime\n");
        printf("i - info about all bus by sorted\n");
        printf("q - exit\n");
        printf("?-: ");
        char act;
        fflush(stdin);
        if(scanf("%c", &act) == EOF) break;
        char c;
        if ((c = getchar()) != '\n' && c != EOF) 
        {
            clear_buffer(stdin);
            printf("INPUT ERROR\n");
            continue;
        }
        statements status = correct;
        max_min comprasion_type;

        if (act == 'a' || act == 'c' || act == 'e' || act == 'g' || act == 'w') 
        {
            printf("Choose comparison type:\n");
            printf("1 - Greater\n");
            printf("2 - Smaller\n");
            printf("?-: ");
            int compType;
            if(scanf("%d", &compType) == EOF) break;
            if ((c = getchar()) != '\n' && c != EOF) 
            {
                clear_buffer(stdin);
                printf("INPUT ERROR\n");
                continue;
            }
            if (compType == 1) comprasion_type = MAX;
            else if (compType == 2) comprasion_type = MIN;
            else 
            {
                printf("Invalid comparison type\n");
                continue;
            }
        }

        if (act == 'a') 
        {
            char* bus_name = bus_id_route(list, &status, comprasion_type);
            if(status == invalid)
            {
                printf("ERROR with a!\n"); break;
            }
            printf("The bus with the number of routes is: %s\n", bus_name);
        }
        else if (act == 'c') 
        {
            double length;
            char* bus_name = bus_id_length_track(list, &status, comprasion_type, &length);
            if(status == invalid)
            {
                printf("ERROR with c!\n"); break;
            }
            printf("The bus with the track length (%lf) is: %s\n", length, bus_name);
            continue;
        }
        else if (act == 'e') 
        {
            double routeLength;
            char* bus_name = max_route_length_bus_id(list, &status, comprasion_type, &routeLength);
            if(status == invalid) 
            {
                printf("ERROR with e!\n"); break;
            }
            printf("The bus with the route length (%lf) is: %s\n", routeLength, bus_name);
        }
        else if (act == 'g') 
        {
            double stopLength;
            char* bus_name = long_short_stop(list, &status, comprasion_type, &stopLength);
            if(status == invalid)
            {
                printf("ERROR with g!\n"); break;
            }
            printf("The bus with the stop length (%lf): %s\n", stopLength, bus_name);
        }
        else if (act == 'w') 
        {
            double downtime;
            char* bus_name = stop_time(list, &status, &downtime);
            if(status == invalid)
            {
                printf("ERROR with w!\n"); break;
            }
            printf("The bus with the downtime (%lf): %s\n", downtime, bus_name);
        }
        else if (act == 'i') 
        {
            printf("All transports :\n");
            printings(list);
        }
        else if (act == 'q') 
        {
            printf("Goodbye!\n");
            break;
        }
    }
}
