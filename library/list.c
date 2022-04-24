#include "list.h"

const size_t GROW_FACTOR = 2;

typedef struct list
{
    void **array;
    size_t capacity;
    size_t size;
    free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer)
{
    list_t *s = malloc(sizeof(list_t));
    assert(s != NULL);
    s->array = malloc(initial_size * sizeof(void *));
    assert(s->array != NULL);
    s->capacity = initial_size;
    s->size = 0;
    s->freer = freer;
    return s;
}

void list_free(list_t *list)
{
    assert(list != NULL);
    if (list->freer != NULL)
    {
        for (size_t i = 0; i < list->size; i++)
        {
            list->freer(list->array[i]);
        }
    }
    free(list->array);
    free(list);
}

size_t list_size(list_t *list)
{
    return list->size;
}

size_t list_capacity(list_t *list)
{
    return list->capacity;
}

void *list_get(list_t *list, size_t index)
{
    assert(0 <= index);
    assert(index < list->size);
    return list->array[index];
}

void *list_remove(list_t *list, size_t index)
{
    assert(list->size > 0);
    assert(index < list->size);
    void *removed = list->array[index];
    for (size_t i = index; i < list->size - 1; i++)
    {
        list->array[i] = list->array[i + 1];
    }
    list->size--;
    return removed;
}

void list_resize(list_t *list)
{
    if (list->size >= list->capacity)
    {
        if (list->capacity == 0)
        {
            list->capacity += GROW_FACTOR;
        }
        else
        {
            list->capacity *= GROW_FACTOR;
        }
        list->array = realloc(list->array, sizeof(void *) * list->capacity);
        assert(list->array != NULL);
    }
}

void list_add(list_t *list, void *value)
{
    assert(value != NULL);
    list_resize(list);
    assert(list->size < list->capacity);
    list->array[list->size++] = value;
}
