#!/bin/bash
app=0
case $1 in
	app)
		app=1
		;;
	*)
		app=0
		;;
esac
if [ $app -eq 1 ];then
	gcc upload.c fdfs_api.c mysql_api.c -lfcgi -lmysqlclient -o app
elif [ $app -eq 0 ];then
	echo "app生成错误"
fi
