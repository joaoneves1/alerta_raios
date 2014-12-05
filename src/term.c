#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "lib_crc.h"
#include "lib_circular_buffer.h"


/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B921600
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyUSB0"

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define BUFFER_SIZE 132
#define MSG_SIZE BUFFER_SIZE-2

void clearScreen(void)
{
  const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO,CLEAR_SCREE_ANSI,12);
}

int16_t vetor_disco[13632]; // 213 blocos de 64 palavras aprox 1200 ms

int main(int argc, char *argv[]) 
{
	int print;
	printf("%d %s\n",argc, argv[0]);
	if(argc >1 && strcmp("-p", argv[1]) == 0) print=1;
	
	time_t nosso_tempo;
	
//************************************************/
	CircularBuffer cb;
    ElemType elem = {0};
    int testBufferSize = 11264; // 176 blocos de 64 palavras
    cbInit(&cb, testBufferSize);
//************************************************/

	FILE *arquivo;
	
	int fd;
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"	
	int res;
#pragma GCC diagnostic pop
	
	unsigned short crc16;
	int i,j=0;
	int detected= 0, pronto = 0;
	char *ptr;
	uint16_t crc;
	uint16_t n_mesg, msg_num = 0;
	int offset;
	struct termios oldtio,newtio;
	char buf[BUFFER_SIZE];
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
	if (fd < 0) {
		perror(MODEMDEVICE); 
		cbFree(&cb);
		exit(-1); 
	}
	tcgetattr(fd,&oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 132;   /* blocking read until 132 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	arquivo = fopen("raios_detetados.bin", "wb");
	if (arquivo == NULL) {
		perror("Nao foi possível abrir o arquivo!");
		cbFree(&cb);
		close(fd);
		exit(30);
	}
	
	while (1) {     /* loop until we have a terminating condition */
		/* read blocks program execution until a line terminating character is 
			input, even if more than 255 chars are input. If the number
			of characters read is smaller than the number of chars available,
			subsequent reads will return the remaining chars. res will be set
			to the actual number of characters actually read */
		res=read(fd,buf,BUFFER_SIZE);
// 		printf("%d chars - ",res);

	crc=((uint16_t) (0xFF00 & (buf[MSG_SIZE+1]<<8)) + (uint16_t) (0x00FF & buf[MSG_SIZE]));
	n_mesg=((uint16_t) (0xFF00 & (buf[1]<<8)) + (uint16_t) (0x00FF & buf[0]));
// 		char test[]="CRC16 stream simulation.";
		ptr=buf;
		crc16= 0;
		for (i = 0; i < MSG_SIZE; i++) {
			crc16 = update_crc_16(crc16, *ptr);
			ptr++;
		}
		if(crc==crc16){
// 			printf(" CRC OK! -  %4X %4X %d\n", crc, crc16, n_mesg);

			for(i=1;i<65;i++){
				elem.value=((int16_t) (0xFF00 & (buf[2*(i)+1]<<8)) + (int16_t) (0x00FF & buf[2*i]));
				if (detected == 1) {
//					if (n_mesg != msg_num) {
					if (j < 13632) { 
						vetor_disco[j] = elem.value;
						j++;
// 						printf("j = %d\n", j);
					}
					else {
						pronto = 1;
						detected = 0;
						cbWrite(&cb, &elem);
					}
				}
				else {
					cbWrite(&cb, &elem);
				}
				if(print==1){
					printf("%4d ",elem.value);
					if (((i+2) % 32) ==1)
					printf("\n");
				}
			}
		} else {
			clearScreen();
// 			printf("not OK!  %X %X\n", crc, crc16);
			tcflush(fd, TCIFLUSH);
		}
		if (pronto == 1) {
			fwrite(vetor_disco, sizeof(vetor_disco), 1, arquivo);
			printf("Write!!!!!");
			j = 0;
			pronto = 0;
		}
// 		if(j>=testBufferSize){
// 			while (!cbIsEmpty(&cb)) {
	// 					elem = cbRead(&cb);i
// 				for (offset=0;offset<(cb.end-cb.start);offset++){
// 					elem = cbPeek(&cb,offset);
// 					printf("%d\t%d\t%d\t%d\n", elem.value, cb.start, cb.end, cb.size);
// 				}
		if (detected == 0) {
			offset = r_change(&cb,n_mesg); // TODO: o que acontece quando n_mesg retorna a zero?
			if(offset){
				nosso_tempo = time(&nosso_tempo);
				fwrite(&nosso_tempo, sizeof(time_t),1, arquivo);
//				for (i = offset - 2263, j = 0; i < cb.end; i++, j++){ // TODO :: fazer a conta do offset levando em consideração o wrap around.
				for (i = (offset%64 - 37)*64, j = 0; i < (offset%64 +1)*64; i++, j++){
					ElemType temp_elem;
 					temp_elem = cbPeek(&cb, i);
					vetor_disco[j] = temp_elem.value;
				}
				cb.end = cb.start; // Esvazia o buffer circular	
				detected = 1;
				msg_num = n_mesg + 176;
				printf("Detected: %ld %d %d\n",nosso_tempo, n_mesg,msg_num);
			}	
	
		}
// 			}
// 		break;
// 		}
// 			for(i=0;i<64;i++){
// 				printf("%4d ",((uint16_t) (0xFF00 & (buf[2*(i)+1]<<8)) + (uint16_t) (0x00FF & buf[2*i])));
// 				if (((i+2) % 32) ==1)           printf("\n");
// 				}
		}
		tcsetattr(fd,TCSANOW,&oldtio);
		return(0);
}

	
// 	voltage_offset=(x-2047)/3.3
// Detected: 1417002186 52806    
// Detected: 1417002188 53222
// Detected: 1417002191 53624
// Detected: 1417002192 53827
// Detected: 1417002193 54044
// 13577

// Detected: 1417004398 56719 56895
// Detected: 1417004404 57850 58026
// Detected: 1417004408 58466 58642
// Write!!!!!