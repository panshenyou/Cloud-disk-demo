//Author:潘糁有
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "fdfs_api.h"

#define CLIENT_CONF "/etc/fdfs/client.conf"
 
int fdfs_upload_file1(const char* filename, char* fileid, int size)
{
	//创建匿名管道
	int fd[2];
	int ret=pipe(fd); 
	if(ret==-1)
	{
		perror("pipe error");	
		exit(1);
	} 

	pid_t pid = fork();
	if(pid==-1)
	{
		perror("fork error");	
		exit(1);
	}
	if(pid>0)
	{
		close(fd[1]);	
		//阻塞读管道
		read(fd[0],fileid, size);
		//回收子进程
		close(fd[0]);
		wait(NULL);
	}
	else if(pid==0)
	{
		close(fd[0]);	
		const char* conf_file=CLIENT_CONF;
		dup2(fd[1], STDOUT_FILENO);
		execlp("fdfs_upload_file", "fdfs_upload_file", conf_file, filename, NULL);
	}
	
	return 0;
}
