//Author:潘糁有
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <string.h>
#include "mysql_api.h"

 
/*
 * 打印数据库出错时的错误信息
 * conn----连接数据库的句柄
 * title---用户错误提示信息
 */
/*
void print_error(MYSQL* conn, const char *title)
{
 
	return 0;
}
*/

/*
 * 连接数据库
 * user_name----数据库用户
 * password-----数据库密码
 * db_name------数据库名称
 * return
 * 成功：返回数据库的句柄
 * 失败: 返回NULL
 */
MYSQL* mysql_conn(char* user_name, char* passwd, char* db_name)
{
	MYSQL* conn=NULL;
	
	conn=mysql_init(NULL);
	if(conn==NULL)
	{
		fprintf(stderr, "mysql 初始化失败");
		return NULL;
	}
	if(mysql_real_connect(conn, "192.168.255.128", "root", "511623", "test", 0, NULL,0)==NULL)	
	{
		fprintf(stderr,"Fialed to connect to database: Error: %s\n", mysql_error(conn));	
		mysql_close(conn);
		return NULL;
	}
	
	return conn;
}










