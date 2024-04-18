
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./ledtest /dev/100ask_led0 on 
 * ./ledtest /dev/100ask_led0 off
 * ./ledtest /dev/100ask_led0 read 
 */
int main(int argc, char **argv)
{
	int fd;
	char status;
	char buf[1024];
	/* 1. 判断参数 */
	if (argc != 3) 
	{
		printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	/* 3. 写文件 */
	if (0 == strcmp(argv[2], "on"))
	{
		status = 1;
		write(fd, &status, 1);
	}
	else{
		status = 0;
		write(fd, &status, 1);
	}

	/*读文件*/
	if (0 == strcmp(argv[2], "read"))
	{
		len = read(fd, buf, 1);
		buf[1023] = '\0';
		printf("APP read: %s\n", buf);	
	}

	
	close(fd);
	
	return 0;
}


