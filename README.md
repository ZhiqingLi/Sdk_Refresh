1.获取远程代码到本地的方法。
$ git remote -v      
//查询当前远程的版本
$ git fetch origin master  [示例1：获取远端的origin/master分支]
$ git fetch origin dev [示例2：获取远端的origin/dev分支]
$ git log -p master..origin/master [示例1：查看本地master与远端origin/master的版本差异]
$ git log -p dev..origin/dev   [示例2：查看本地dev与远端origin/dev的版本差异]
$ git merge origin/master  [示例1：合并远端分支origin/master到当前分支]
$ git merge origin/dev [示例2：合并远端分支origin/dev到当前分支]

2.如果我们想让服务器代码完全替换和覆盖我们本地的代码改动，那么只需要执行
$ git fetch origin master
$ git reset --hard origin/master
$ git pull
即可

3.提交本地修改到远程仓库
$ git add .
$ git commit -m "提交注释"
$ git push -u origin master