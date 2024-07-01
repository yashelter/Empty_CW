
#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#define __USE_XOPEN
#include <time.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <ctype.h>


typedef enum bus_statements
{
    same_time_for_arrive,
    same_time_for_departure,
    same_time,
    same_time_ad,
    departure_early_arrive,
    departure_bigger_arriving,
    time_correct
} bus_statements;

typedef enum statements
{
    correct,
    invalid_file,
    invalid_input,
    allocate_error,
    empty_file,
    empty_string,
    invalid_coords,
    runtime_error,
    invalid,
} statements;

typedef enum status_arriving
{
    START,
    INTERMEDIATE,
    FINAL
} status_arriving;

typedef enum max_min
{
    MAX,
    MIN
} max_min;

// Точка, где всё состояние автобуса
typedef struct Position
{
    char* bus_id;
    char* arrive;
    char* departure;
    double x;
    double y;
    status_arriving status;
} Position;

// маршрут
typedef struct track_path
{
    Position* position;
    struct track_path* route_ptr;
} track_path;

// список маршрутов
typedef struct Track_lst
{
    track_path* master_track;
} Track_lst;


typedef struct main_node
{
    Track_lst track_lst;
    struct main_node* main_next;
} main_node;

typedef struct Buses_list
{
    main_node* main_head;
} Buses_list;


track_path* create_route_node(Position* position)
{
    if(position == NULL) return NULL;
    track_path* new_node = (track_path*)malloc(sizeof(track_path));
    if(new_node == NULL) return NULL;

    new_node->position = position;
    new_node->route_ptr = NULL;
    return new_node;
}

main_node* create_main_node()
{
    main_node* new_node = (main_node*) malloc(sizeof(main_node));
    if(new_node == NULL) return NULL;

    new_node->track_lst.master_track = NULL;
    new_node->main_next = NULL;
    return new_node;
}

#endif 