#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

#include <stdlib.h>

#include "type.h"

typedef struct ArrayList
{
    usize size;
    usize cap;
    usize it_size;
    void *data;
} ArrayList;

#endif