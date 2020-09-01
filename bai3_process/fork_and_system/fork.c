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
// lib for system function

#include <stdlib.h>


#define FILE_PATH  "data.txt"

int main(void)
{
	int a = 0;
	int b = 0;
	int fd =-1;
	int pid = -1;


	pid  = fork();
	if(pid ==-1)
	{	
		printf("cannot create child process\n");
		return -1;
	}	
	
	if(pid == 0)
	{
		int status = 0;
		printf("child process\n");
		status = system("ls -l > data.txt");
		return 0;
	}
	else
	{
		int child_status =-1;
		printf("this is the parent process\n");
		wait(&child_status);
		if(child_status == 0)
		{
			int fd =-1;
			int number_byte_read =0;
			char buffer[1024];
			fd = open(FILE_PATH, O_RDONLY);
			if(fd ==-1)
				return -1;
			number_byte_read = read(fd, buffer, sizeof(buffer));
			if(number_byte_read >0)
				printf("data read:\n%s",buffer);
			close(fd);
		}
		printf("main finish\n");
	}
}

