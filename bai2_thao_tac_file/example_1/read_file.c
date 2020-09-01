#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>


void main(void)
{
	char* file_path = "data.txt";
	int fd =-1;
	int read_number_byte =0;
	char buffer[1024];
	fd = open(file_path, O_RDONLY);
	
	if(fd < 0)
	{
		printf("cannot open file\n");
		return;
	}
	read_number_byte = read(fd,buffer, sizeof(buffer));
	if(read_number_byte >0)
		printf("read_data:\n%s",buffer);
	printf("\nmain finish\n"); 
}
