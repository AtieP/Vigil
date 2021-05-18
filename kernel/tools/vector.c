#include <stddef.h>
#include <mm/mm.h>
#include <tools/builtins.h>
#include <tools/vector.h>

void vector_create(struct vector *vector) {
    vector->data = NULL;
    vector->size = 0;
}

void vector_push(struct vector *vector, void *data, size_t data_size) {
    void *ptr = kheap_realloc(vector->data, vector->size + data_size, vector->size);
    memcpy(ptr + vector->size, data, data_size);
    vector->data = ptr;
    vector->size += data_size;
}

void vector_delete(struct vector *vector) {
    kheap_free(vector->data);
}
