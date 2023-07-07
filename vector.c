#include <stdlib.h>
#include <string.h>

#include "vector.h"

bool Vector_has_enough_capacity_for_next_item(Vector* vector) {
	return vector->length + 1 <= vector->capacity;
}

void Vector_increase_capacity(Vector* vector) {
	vector->capacity = vector->capacity * VECTOR_GROWTH_FACTOR;
	vector->items = (void **) realloc(vector->items, vector->capacity * sizeof(void *));
}

void Vector_increase_length(Vector* vector) {
	vector->length = vector->length + 1;
}

void Vector_decrease_length(Vector* vector) {
	int new_length = vector->length - 1;
	vector->length = new_length <= 0 ? 0: new_length;
}

Vector* allocate_Vector(void) {
	Vector* vector = (Vector *) malloc(sizeof(Vector));
	vector->capacity = INITIAL_VECTOR_CAPACITY;
	vector->length = 0;
	vector->items = (void **) calloc(vector->capacity, sizeof(void *));
	return vector;
}

void destroy_Vector(Vector* vector) {
	for (int index = 0; index < vector->length; index++) {
		void* item = Vector_get_item(vector, index);
		free(item);
	}
	free(vector->items);
	free(vector);
}

void* Vector_get_item(Vector* vector, int index) {
	if (index < 0 || index >= vector->length) {
		return NULL;
	}
	return *(vector->items + index);
}

void Vector_set_item(Vector* vector, int index, void* item) {
	*(vector->items + index) = item;
}

void Vector_add_item(Vector* vector, void* item) {
	if (!Vector_has_enough_capacity_for_next_item(vector)) {
		Vector_increase_capacity(vector);
	}
	Vector_set_item(vector, vector->length, item);
	Vector_increase_length(vector);
}

void Vector_remove_item(Vector* vector, int index) {
	if (index >= vector->length) {
		return;
	}
	Vector_decrease_length(vector);
	int items_to_move = vector->length - index;
	if (!items_to_move) {
		return;
	}
	void** destination = vector->items + index;
	void** source = vector->items + index + 1;
	size_t block_size = items_to_move * sizeof(void **);
	memmove(destination, source, block_size);
}
