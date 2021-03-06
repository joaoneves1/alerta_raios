// macro contra múltipla inclusão
#ifndef _LIB_CIRCULAR_BUFFER_H
#define _LIB_CIRCULAR_BUFFER_H

typedef struct { int value; } ElemType;

typedef struct {
	int         size;   /* maximum number of elements           */
	int         start;  /* index of oldest element              */
	int         end;    /* index at which to write new element  */
	ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size);
void cbFree(CircularBuffer *cb);
int cbIsFull(CircularBuffer *cb);
int cbIsEmpty(CircularBuffer *cb);
void cbWrite(CircularBuffer *cb, ElemType *elem);
// void cbRead(CircularBuffer *cb, ElemType *elem); // Alterado para retornar o valor
ElemType cbRead(CircularBuffer *cb);
ElemType cbPeek(CircularBuffer *cb, int offset);

int r_change(CircularBuffer *cb, int n_mesg);

#endif