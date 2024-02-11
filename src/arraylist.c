#include "arraylist.h"

ArrayList new_arraylist(usize it_size)
{
    ArrayList l;
    l.size = it_size;
    l.cap = 1;
    l.it_size = it_size;
    l.data = malloc(it_size);
    return l;
}

void *get(const ArrayList *const l, usize i)
{
    return l->data + l->it_size * i;
}

usize len(const ArrayList *const l)
{
    return l->size / l->it_size;
}
