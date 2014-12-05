// Circular buffer example, keeps one slot open

#include <stdio.h>
#include <stdlib.h>

/* Opaque buffer element type.  This would be defined by the application. */
typedef struct { int value; } ElemType;

/* Circular buffer object */
typedef struct {
	int         size;   /* maximum number of elements           */
	int         start;  /* index of oldest element              */
	int         end;    /* index at which to write new element  */
	ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size) {
	cb->size  = size + 1; /* include empty elem */
	cb->start = 0;
	cb->end   = 0;
	cb->elems = calloc(cb->size, sizeof(ElemType));
}

void cbFree(CircularBuffer *cb) {
	free(cb->elems); /* OK if null */
}

int cbIsFull(CircularBuffer *cb) {
	return (cb->end + 1) % cb->size == cb->start;
}

int cbIsEmpty(CircularBuffer *cb) {
	return cb->end == cb->start;
}

/* Write an element, overwriting oldest element if buffer is full. App can
choose to avoid the overwrite by checking cbIsFull(). */
void cbWrite(CircularBuffer *cb, ElemType *elem) {
	cb->elems[cb->end] = *elem;
	cb->end = (cb->end + 1) % cb->size;
	if (cb->end == cb->start)
		cb->start = (cb->start + 1) % cb->size; /* full, overwrite */
}

// /* Read oldest element. App must ensure !cbIsEmpty() first. */
// void cbRead(CircularBuffer *cb, ElemType *elem) {
// 	*elem = cb->elems[cb->start];
// 	cb->start = (cb->start + 1) % cb->size;
// }
// 

/* Read oldest element. App must ensure !cbIsEmpty() first. */
ElemType cbRead(CircularBuffer *cb) {
    ElemType tmp;
	tmp = cb->elems[cb->start];
    cb->start = (cb->start + 1) % cb->size;
	return tmp;
}


ElemType cbPeek(CircularBuffer *cb, int offset) {
    ElemType tmp;
//verificar se start+offset maior que end.	
	tmp = cb->elems[cb->start+offset];
//     cb->start = (cb->start + 1) % cb->size;
	
	return tmp;
}
