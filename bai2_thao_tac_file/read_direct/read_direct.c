#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>
#include <dirent.h>


void main(void)
{
	char* file_path = "/home/thanhtung/Documents/";

	struct dirent* user_direct = NULL;
	DIR* direct_open;
	
	direct_open = opendir(file_path);
	if(direct_open == NULL)
		return;
	user_direct = readdir(direct_open);
	while(user_direct != NULL)
	{
		printf("folder:%s\n",user_direct->d_name);
		user_direct = readdir(direct_open);
	}
	printf("main finish\n");
}
