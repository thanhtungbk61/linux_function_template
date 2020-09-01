#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define FILE_PATH "/dev/led_dev"

void device_read(int fd)
{
    char data[1];
    int read_length = 0;
    read_length = read(fd, data, 1);
    if(read_length <0)
		printf("cannot read device file, read_read_lenght:%d\n", read_length);
    else
		printf("read_value:%d\n", atoi(data));
}

void device_write(int fd, char* data)
{
    int write_length =0;
    write_length = write(fd, data, 1);
    if(write_length < 0)
		printf("cannot write to device file: write_length:%d\n", write_length);
    else
		printf("write success\n");
}    


void main(void)
{
    int fd;
    char option, write_data;
    fd = open(FILE_PATH, O_RDWR);

    if(fd <0)
    {
		printf("cannot open device file\n");
		return;
    }

    printf("open device file success\n");
    printf("select one option below\n");
    printf("\tr (to read data from device)\n");
    printf("\tw (to write data to device)\n");
    printf("\tq (to quite application)\n");
    printf("\t1 (to write 1 to device)\n");
    printf("\t0 (to write 0 to device)\n");
    while(1)
    {
		printf("Enter your option\n");
		scanf("%c", &option);
		switch(option)
		{
	    	case('r'):
				device_read(fd);
				break;
	    	case('w'):
				printf("Enter your data want to write\n");
				scanf("%c", &write_data);
				device_write(fd, &write_data);
			break;

	    	case('q'):
				close(fd);
				return;
			case('1'):
				device_write(fd, "1");
				break;
			case('0'):
				device_write(fd, "0");
	   		default:
				break;
		}
    }
    printf("application finished\n");
}



