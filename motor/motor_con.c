#include <time.h>
#include <stdio.h>
void put_to_sleep(){
	usleep(30);
}

int main()
{
	int ret;
	FILE* file = fopen("/dev/motor","w");
	char *go = "fg500";
	char *stop = "fs500";
	if(file){
		  ret = fwrite(go,5,1,file);
		  printf("ret %d", ret);
	}
}
