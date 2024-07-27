## 轻量级KV存储引擎

本项目是一个基于**SkipList**实现的轻量级KV存储引擎，同时在底层实现一个简单的**定长内存池**管理内存。<br/>
支持插入数据、删除数据、查询数据、清空数据、数据规模查询、**迭代器遍历**、数据落盘、文件加载数据。

## 项目文件
* main.cpp 包含skiplist.h，使用跳表进行数据操作
* src 项目源代码文件目录
* skiplist_node.h 跳表的结点实现，位于src目录下
* skiplist.h 跳表的核心实现，位于src目录下
* memorypool.h 定长内存池的简单实现，位于src目录下
* bin 生成可执行文件目录
* makefile 编译脚本
* store 数据落盘的文件存放在这个文件夹
* test.sh 运行脚本
  
## 提供接口
* insert（插入数据）
* erase（删除数据）
* find（查询数据）
* count（判断数据是否存在）
* size（返回数据规模）
* empty（判断跳表是否为空）
* clear（清空数据）
* dump（数据落盘）
* load（加载数据）

## 项目运行
```
make            // complie demo main.cpp
./bin/main      // run 
```
或
```
sh test.sh
```
<br/>
如果想自己写程序使用这个KV存储引擎，只需要 include "skiplist.h" 即可。
