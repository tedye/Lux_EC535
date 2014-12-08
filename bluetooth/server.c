#include <stdio.h>
#include <error.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h> //from bluez library
#include <bluetooth/rfcomm.h> //from bluez library
int main()
{
	//RFCOMM structure
	struct sockaddr_rc myAddress, remoteAddress;
	int listenfd, client, bytes_read;
	char buf[100];
	char str[10];
	FILE * pFile = fopen("/etc/lux/Lux", "w");
	listenfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(listenfd < 0){
		perror("RFCOMM Socket");
		return 1;
	}
	myAddress.rc_family = AF_BLUETOOTH;
	myAddress.rc_bdaddr = *BDADDR_ANY;
	myAddress.rc_channel = (uint8_t) 1; //defining binding port 1
	bind(listenfd, (struct sockaddr *)&myAddress, sizeof(myAddress));
	// listen to socket
	listen(listenfd, 1);
	//accept the requested connection
	int fsize = sizeof(remoteAddress);
	client = accept(listenfd, (struct sockaddr *)&remoteAddress, &fsize);
	memset(buf, 0, sizeof(buf));
	/*
	ba2str takes 6-byte remoteAddress.rc_bdaddr and copy it into buf
	*/
	listenfd = ba2str( &remoteAddress.rc_bdaddr, buf );
	if(listenfd<0){
		perror("ba2Str");
	}
	printf("Connected to '%s'\n",buf);
	
	while (1){
		memset(buf, 0, sizeof(buf));
		// read data from the client
		bytes_read = read(client, buf, sizeof(buf));
		if( bytes_read < 0 ) {
			perror("read\n");
			return 1;
		}
		fwrite(buf,1,sizeof(buf),pFile);
		fclose(pFile);

		printf("received '%s'\n",buf);
	}
	// close connection
	close(client);
	close(listenfd);
	return 0;
}
