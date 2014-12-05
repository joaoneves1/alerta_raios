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
 included by <*termios.h> */
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
	
	FILE *arquivo;
	
	time_t nosso_tempo;

	int fd;
	
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"	
	int res;
	#pragma GCC diagnostic pop
	
	unsigned short crc16;
	int i,j=0;
	
	char *ptr;
	uint16_t crc;
// 	uint16_t n_mesg, msg_num = 0;
	struct termios oldtio,newtio;
	char buf[BUFFER_SIZE];
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
	if (fd < 0) {
		perror(MODEMDEVICE); 
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
	
	arquivo = fopen("raios_lidos.bin", "wb");
	if (arquivo == NULL) {
		perror("Nao foi possível abrir o arquivo!");
		close(fd);
		exit(30);
	}
// 	printf("size=%d\n",sizeof(time_t));
// 	exit(0)
	
	while (j<1000) {     /* loop until we have a terminating condition */
		
		res=read(fd,buf,BUFFER_SIZE);
				if(res!=BUFFER_SIZE) {
					printf("%d chars - ",res);
					close(fd);
					fclose(arquivo);
					exit(25);
					}
		crc=((uint16_t) (0xFF00 & (buf[MSG_SIZE+1]<<8)) + (uint16_t) (0x00FF & buf[MSG_SIZE]));
// 		n_mesg=((uint16_t) (0xFF00 & (buf[1]<<8)) + (uint16_t) (0x00FF & buf[0]));
		ptr=buf;
		crc16= 0;
		for (i = 0; i < MSG_SIZE; i++) {
			crc16 = update_crc_16(crc16, *ptr);
			ptr++;
		}
		ptr=buf+2;
		if(crc==crc16){
			nosso_tempo = time(&nosso_tempo);
			fwrite(&nosso_tempo, sizeof(time_t),1, arquivo);
			fwrite(ptr, sizeof(char), 128, arquivo);
		} else {
			clearScreen();
			printf("not OK!  %X %X\n", crc, crc16);
			tcflush(fd, TCIFLUSH);
		}
		j++;
	}
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	fclose(arquivo);
	return(0);
}	
