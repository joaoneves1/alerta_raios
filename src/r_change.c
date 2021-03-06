#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "lib_circular_buffer.h"


int r_change(CircularBuffer *cb, int n_mesg){
// Ler os dados do buffer circular e avaliar a derivada em diferentes intervalos de tempo;
    int tmp,tmp10;
	int i,ds=1;
	int detected=0;
	
	int filled = (cb->end > cb->start? cb->end - cb->start: cb->size - cb->start + cb->end)-ds;

	for (i=0;i< filled;i++){
		tmp   = cb->elems[(cb->start + i) % cb->size].value;
		tmp10 = cb->elems[(cb->start + i+ds) % cb->size].value;
		if ((tmp10-tmp) > 2000){
 			printf("Subida, %d\t%d\n",i, cb->end);
			detected=i;
		}
		if ((tmp-tmp10) > 2000){
			printf("Descida, %d\t%d\n",i, cb->end);
// 			printf("%d\t%d\t%d\n",i,cb->start + i,n_mesg);
			detected=i;
		}
		// processa tmp
	}
//	cb->start = cb->end; // Esvazia o buffer circular
	return detected;
}


