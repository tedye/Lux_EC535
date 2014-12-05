#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
//#include <linux/i2c.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "luxcalc.h"

#define COMMAND 0x80
#define CONTROL 0x00
#define ON 0x03
#define OFF 0x00
#define TIMING 0x01
#define WORD 0x20
#define LOW0 0x0C
#define HIGH0 0x0D
#define LOW1 0x0E
#define HIGH1 0x0F
#define GAIN 0x00
#define ID 0x0A
#define BLOCK 0x10

int main()
{
	int file;
	volatile unsigned addr = 0x39;
	char *filename = "/dev/i2c-0";
	unsigned buf,buf2, channel0, channel1;
	int Lux;
	char str[10];
	/*char* buf;
	char* buf2;
	buf = malloc(sizeof(char)*2);
	buf2 = malloc(sizeof(char));*/
	if((file = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open\n");
		exit(1);
	}
	if (ioctl(file,I2C_SLAVE_FORCE,addr) < 0)
	{
		printf("Failed\n");
		exit(1);
	}

	// turning it on
	/*write(file,COMMAND|CONTROL,1);
	write(file,ON,1);*/

	i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);

	// setting up timing and gain
	/*write(file,COMMAND|TIMING,1);
	write(file,GAIN|TIMING,1);*/

	i2c_smbus_write_byte_data(file,COMMAND|TIMING,GAIN|TIMING);

	/*write(file,COMMAND|ID,1);
	read(file,buf,1);
	read(file,buf2,1);
	printf("buf %x, buf2 %x\n",*buf,*buf2);*/

	buf = i2c_smbus_read_byte_data(file,COMMAND|ID);
	printf("ID %x\n",buf);
	// reading channel 0
	while(1)
	{
		usleep(101000);
		/*write(file,COMMAND|WORD|HIGH,1);
		read(file,buf,1);
		write(file,COMMAND|WORD|LOW,1);
		read(file,buf2,2);*/
		
		buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH0);
		buf <<= 8;
		buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW0);
		buf |= buf2;
		channel0 = buf;

		buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH1);
		buf <<= 8;
		buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW1);
		buf |= buf2;
		channel1 = buf;

		Lux = CalculateLux(0,1,channel0,channel1,0);
		FILE * pFile = fopen("/proc/fortune", "w");
		
		sprintf(str,"%d",Lux);
		fwrite(str,1,sizeof(str),pFile);
		fclose(pFile);
		//printf("value %d\n",Lux);
			
	}
}
