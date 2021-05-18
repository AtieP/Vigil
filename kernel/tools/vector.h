#ifndef __TOOLS_VECTOR_H__
#define __TOOLS_VECTOR_H__

#include <stddef.h>

struct vector {
    void *data;
    size_t size;
};

void vector_create(struct vector *vector);
void vector_push(struct vector *vector, void *data, size_t data_size);
void vector_delete(struct vector *vector);

#endif
