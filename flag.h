#ifndef FLAG_H
#define FLAG_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//test comment

typedef enum {
    BOOL,
    STR,
} type_t;

typedef struct
{
    type_t type;
    bool valid;
    void* content;
    const char* name;
} flag_t;

flag_t* set_flag(const type_t type, const char* name);
int filter_flags(int* argc, char** argv);
#endif

#ifndef FLAG_H_IMPLEMENTATION
#define FLAG_H_IMPLEMENTATION

#ifndef FLAG_CAPACITY
#define FLAG_CAPACITY -1 //little Hack (?)better Way(?)
#endif

//define FLAG_CAPACITY above #include "flag.h"
flag_t FLAG_BUFFER[FLAG_CAPACITY] = { { .name = NULL, .valid = false, .content = NULL } };

// TODO: make better
//combination of cryptohash and lagrange might do the trick -> look into that -> every other func can be simplified
size_t hash(const char* s)
{
    size_t index = 0;
    while (*s != '\0')
        index *= *s++;

    return index % FLAG_CAPACITY;
}

//finds right slot in global FLAG array and returns pointer to that slot
flag_t* set_flag(const type_t type, const char* name)
{
    //index where it should be
    size_t index = hash(name);

    //compensate collision
    while (FLAG_BUFFER[index].name != NULL)
        index = (index < FLAG_CAPACITY - 1) ? index + 1 : 0; //flip from arr len to 0

    FLAG_BUFFER[index].name = name;
    FLAG_BUFFER[index].type = type;

    return &FLAG_BUFFER[index];
}

flag_t* get_flag(const char* name)
{
    size_t index = hash(name);
    for (int k = 0; k < FLAG_CAPACITY; ++k) {
        if (strcmp(FLAG_BUFFER[index].name, name) == 0)
            return &FLAG_BUFFER[index];
        index = (index < FLAG_CAPACITY - 1) ? index + 1 : 0; //flip to index 0 if end of array is reached
    }
    return NULL;
}

// filters argv for flags and parameters, stores valid flags in global FLAG_BUFFER
// returns with exit code 1 if STR Flag doesn't have a parameter
int filter_flags(int* argc, char** argv)
{
    flag_t* flag = NULL;

    //store the not used parts of argc and argv
    char* rest_buffer[*argc];
    int rest_counter = 0;

    for (int i = 0; i < *argc; ++i) {
        flag = get_flag(argv[i]);
        if (flag != NULL) {
            flag->valid = true;
            switch (flag->type) {
            case STR:
                ++i; //inc index to look at argument of flag

                //retuns with errorcode if no argument is given
                if (!(i < *argc && get_flag(argv[i]) == NULL))
                    return 1;

                flag->content = argv[i];
                break;

            case BOOL:
                flag->content = (void*)&flag->valid; //little hacky | we need a void* ptr to a bool
                break;
            }

        } else
            rest_buffer[rest_counter++] = argv[i];
    }

    *argc = rest_counter;
    for (int i = 0; i < rest_counter; ++i) //that should be memcpy, but isn't
        argv[i] = rest_buffer[i];
    return 0;
}
#endif
