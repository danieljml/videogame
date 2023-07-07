#ifndef __VECTOR_H__
#define __VECTOR_H__

#define INITIAL_VECTOR_CAPACITY 2
#define VECTOR_GROWTH_FACTOR 2

typedef enum { false, true } bool;

typedef struct {
	int capacity;
	int length;
	void** items;
} Vector;

Vector* allocate_Vector(void);
void destroy_Vector(Vector* vector);

void* Vector_get_item(Vector* vector, int index);
void Vector_set_item(Vector* vector, int index, void* item);

void Vector_add_item(Vector* vector, void* item);
void Vector_remove_item(Vector* vector, int index);

#endif /*__VECTOR_H__*/
