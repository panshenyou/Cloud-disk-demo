//Author:潘糁有
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "fdfs_api.h"
#include "mysql_api.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fcgi_config.h"
#include "fcgi_stdio.h"



char* memstr(char* full_data, int full_data_len, char* substr)
{
	if(full_data==NULL || full_data_len<=0||substr == NULL)
	{
		return NULL;	
	}
	if(*substr == '\0')
	{
		return 0;	
	}

	int sublen=strlen(substr);
	char* cur=full_data;
	int last_possible=full_data_len - sublen + 1;
	for(int i=0; i<last_possible; i++)
	{
		if(*cur==*substr)	
		{
			if(memcmp(cur, substr, sublen)==0)	
			{
				//found	
				return cur;
			}
		}
		cur++;
	}
	return NULL;
}

 
int get_file_content(char* begin, char* end, int len, char* filename)
{
	char* p=NULL;
	//取出并保存分界线字符串
	char boundary[256]={0};
	//p指向该行的\r字符
	p=strstr(begin, "\r\n");
	strncpy(boundary, begin, p-begin);
	//p指向下一行行首
	p+=2;
	//剩余字符串长度
	len-=(p-begin);
	printf("boundary:%s",boundary);

	//记录第二行行首位置
	begin=p;
	//
	p=strstr(begin, "\r\n");
	//移动到下一行行首
	p+=2;
	//剩余数据长度
	len-=(p-begin);
	//将filename高出来
	char* pt=strstr(begin, "filename=");
	pt+=strlen("filename=");//此时pt指向双引号
	//指针指向文件名的第一个字符
	pt++;
	char* q=strchr(pt, '"');//找到文件结尾的字符
	//保存文件名
	strncpy(filename, pt, q-pt);
	printf("<br>filename:%s<br>",filename);//通过重定向将filename打印到网页上
	 
	//提取文件内容
	begin=p;
	p=strstr(begin, "\r\n");
	//p跳过\r\n和空行
	p+=4;
	//剩余数据长度
	len-=(p-begin);
	//文件内容正式开始
	begin=p;
	//从剩余数据快中找到结束分界线
	p=memstr(begin, len, boundary);
	if(p==NULL)
	{
		//减去\r\n长度	
		p=end-2;
	}
	else
	{
		//减去\r\n长度	
		p-=2;
	}
	//将文件内容写入文件中

	int fd=open(filename, O_WRONLY|O_CREAT,0664);
	write(fd, begin, p-begin);
	close(fd);

	return 0;
}

int store_data(char* filename, char* fileid)
{
	//连接数据库
	
	MYSQL* conn=NULL;
	conn=mysql_conn("root","511623","test");
	if(conn==NULL)
	{
		printf("mysql error");			
		return -1;
	}
	//设置数据库编码，否则中文乱马
	mysql_query(conn, "set names utf8");
	//插入数据
	char buf[1024];
	sprintf(buf, "insert into dfs_file1(name, fileid) values ('%s','%s');",filename, fileid);
	printf("<br> sql: %s<br>\n", buf);
	//执行sql语句
	//如果成功，他返回0
	if(mysql_query (conn, buf)!=0)
	{
		printf("数据插入失败");	
		return -1;
	}
	//关闭数据库
	mysql_close(conn);

	return 0;
}


 
int main()
{
 	while(FCGI_Accept()>=0)
	{
		int len=0;
		char* contentLength=getenv("CONTENT_LENGTH");
		printf("Content-type: text/html\r\n""\r\n");
		if(contentLength !=NULL)
		{
			//将post传输数据的长度转换成十进制
			len=strtol(contentLength, NULL , 10);	
		}
		if(len<=0)
		{
			printf("No data from standard input.<p>\n");	
		}
		else
		{
			int ch;
			char filename[128]={0};
			//根据post数据长度，创建对应大小的数组
			char* file_data=(char*)malloc(len);
			char *begin,*end,*p;  //指针初始化
			end=NULL;
			begin=p=file_data;

			for(int i=0; i<len; i++)
			{
				if((ch=getchar())<0)	
				{
					printf("Error：Not enough bytes ");	
					break;
					break;
				}
					*p=ch;  //赋值
					p++; 
			}
			//end指向文件结束位置
			end=p;
			//得到文件的内容
			get_file_content(begin, end, len, filename);
			//文件上传到fdfs
			char fileid[1024]={0};
			fdfs_upload_file1(filename, fileid, 1024);
			printf("<br>fileid: %s\n<br>",fileid);

			//将数据存储到数据库中
			store_data(filename, fileid);
			//释放内存
			free(file_data);
			//删除文件
			unlink(filename);
		}
	
	}

	return 0;
}
