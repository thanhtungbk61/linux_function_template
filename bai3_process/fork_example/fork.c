#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h> 

// lib thao tac voi file

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

#define FILE_PATH  "data.txt"

int main(void)
{
	int a = 0;
	int b = 0;
	int fd =-1;
	int pid = -1;
	fd = open(FILE_PATH,O_RDWR);
	if(fd == -1)
	{
		printf("cannot open file\n");
		return -1;
	}
		
	lseek(fd, 0, SEEK_END);
	pid  = fork();
	if(pid ==-1)
	{	
		printf("cannot create child process\n");
		return -1;
	}	
	
	if(pid == 0)
	{
		int number_data_write = 0;
		char* data = "child data write\n";
		printf("this is the child process\n");
		number_data_write = write(fd, data, sizeof("this the child process\n"));
		printf("child number data write:%d", number_data_write);
		return 0;
	}
	else
	{
		int child_status =-1;
		int number_byte_write =0;
		char* data = "parent data write\n";
		printf("this is the parent process\n");
		number_byte_write = write(fd, data, sizeof("parent data write\n"));
		printf("parent nuber data write:%d", number_byte_write);
		wait(&child_status);
		printf("main finish\n");
	}
}

