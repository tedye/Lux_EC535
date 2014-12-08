#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "luxcalc.h"
#include <sys/socket.h>
#include <error.h>
#include "bluetooth.h"
#include "rfcomm.h"
#include "hci.h"
#include "hci_lib.h"


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

/*
	This is a service application that reads data from the TSL2561 and calculates the Lux.
	Values are written to the application folder under /etc/lux/
*/

/* from git@gitorious.org:bluez/jdelfes-bluez.git */
int str2ba(const char *str, bdaddr_t *ba)
{
	int i;
	if (bachk(str) < 0) {
		memset(ba, 0, sizeof(*ba));
		return -1;
	}
	for (i = 5; i >= 0; i--, str += 3)
		ba->b[i] = strtol(str, NULL, 16);
	return 0;
}

int bachk(const char *str)
{
	if (!str)
		return -1;
	if (strlen(str) != 17)
		return -1;
	while (*str) {
		if (!isxdigit(*str++))
			return -1;
		if (!isxdigit(*str++))
			return -1;
		if (*str == 0)
			break;
		if (*str++ != ':')
			return -1;
	}
	return 0;
}
/* from git@gitorious.org:bluez/jdelfes-bluez.git end */

int main()
{
	int file;
	volatile unsigned addr = 0x29;
	char *filename = "/dev/i2c-0";
	unsigned buf,buf2, channel0, channel1;
	int Lux;
	int avgLux;
	char str[10];
	int loop;
	int avg;
	// bluetooth connection setup
	struct sockaddr_rc bluetooth_addr;
	int listenfd, status;
	//XX:XX:XX:XX:XX:XX IS BLUETOOTH ADDRESS
	char dest[18] = "00:03:19:50:29:45";
	int cc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(cc < 0){
		perror("Socket failed");
		exit(1);
	}
	// set the connection parameters (who to connect to)
	bluetooth_addr.rc_family = AF_BLUETOOTH;
	bluetooth_addr.rc_channel = (uint8_t) 1;
	str2ba( dest, &bluetooth_addr.rc_bdaddr );
	// connect to server
	status = connect(listenfd, (struct sockaddr *)&bluetooth_addr, sizeof(bluetooth_addr));
	// end of bluetooth setup. 


	avg = 3;
	if((file = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open\n");
		exit(1);
	}
	if (ioctl(file,I2C_SLAVE_FORCE,bluetooth_addr) < 0)
	{
		printf("Failed\n");
		exit(1);
	}

	i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);

	i2c_smbus_write_byte_data(file,COMMAND|TIMING,GAIN|TIMING);

	buf = i2c_smbus_read_byte_data(file,COMMAND|ID);
	printf("ID %x\n",buf);
	while(1)
	{
		avgLux = 0;
		loop = 0;
		while(loop<avg)
		{
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);
			usleep(101000);
			
			/*buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW0);
			buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH0);
			buf <<= 8;
			buf |= buf2;
			channel0 = buf;*/
			channel0 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW0);

			/*buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW1);
			buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH1);
			buf <<= 8;
			buf |= buf2;
			channel1 = buf;*/
			channel1 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW1);


			Lux = CalculateLux(0,1,channel0,channel1,0);
			avgLux += Lux;
			
			loop++;
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,OFF);
		}
		avgLux = avgLux/avg;
		if(avgLux >= 40000)
			avgLux = 500;
		else if (avgLux == 0)
			avgLux = 100;
		FILE * pFile = fopen("/etc/lux/Lux", "w");
		sprintf(str,"%d",avgLux);
		fwrite(str,1,sizeof(str),pFile);
		fclose(pFile);
		if( status == 0 ) {
			status = write(listenfd, str, sizeof(str));
		}
		if( status < 0 ){
		perror("data sending failed");
		exit(1);
	}


	}
		close(file);
}
