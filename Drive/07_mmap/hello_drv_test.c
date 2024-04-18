#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

/*
*  ./hell_drv_test 
*/

int main(int argc, char **argv)
{
	int fd;
	char *buf;
	int len;
	char str[1024];

	/*2、打开文件*/
	fd = open("/dev/hello", O_RDWR);
	if(fd == -1){
		printf("can not open file /dev/heloo\n");
		return -1;
	}

	/*mmap*/
	buf = mmap(NULL, 1024*8, PROT_READ |PROT_WRITE , MAP_SHARED, fd, 0);
	if(buf == MAP_FAILED){
		printf("can not mmap file /dev/heloo\n");
		return -1;
	}

	printf("mmap address 0x%xn", buf);
	
	/*write*/
	strcpy(buf, "wiki.100ask.net");
	
	/*read & compare*/
	read(fd, str, 1024);
	
	if(strcmp(buf, str) == 0){
		printf("compare ok!\n");
	}
	else{
		printf("compare err!\n");
	}

	while(1)
	{
		sleep(10);
	}
	
	munmap(buf, 1024*8);
	close(fd);
	
	return 0;

}






































