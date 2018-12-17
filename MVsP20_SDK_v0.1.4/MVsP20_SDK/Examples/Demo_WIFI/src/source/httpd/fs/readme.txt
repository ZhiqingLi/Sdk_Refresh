1.首先将网页源文件编写好，如工程中webpage文件夹下的文件。
2.将makefsfile工具和webpage放在同一个文件夹下如fs.
3.运行cmd,进入到makefsfile工具的目录fs。
4.使用makefsfile -i webpage -o fsdata.c -r -h 命令生成一个 fsdata.c文件（或者直接运行make.bat批处理）
5.用新生成的fs/fsdata.c中生成的数组替换httpd/fsdata中需要更新的数据即可，只替换data数据区 