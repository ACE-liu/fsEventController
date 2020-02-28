#if defined(ENABLE_MYSQL)
#pragma once
#include<iostream>
#include<string.h>
#include<vector>
#include<map>
#include<fstream>
#include<mysql.h>

using namespace std;
 
namespace XMYSQL {
 
	
	struct XData  //存放从结果集读出的一行数据 vector<XData>,XData只是一行中的一个字段
	{
		XData() = default;
		XData(const char* s) :data(s), size(strlen(s)) {}  
		XData(const int* d) :type(MYSQL_TYPE_LONG),data(reinterpret_cast<const char*>(d)),size(sizeof(int)) {}//支持插入整型
 
		bool LoadFile(const char* filename);//从磁盘读取二进制文件，内容写入到data，size，会在堆中申请内存，需要Drop进行释放
		bool SaveFile(const char* filename);//从数据库读取二进制文件，并保存到硬盘
		void Drop();						//释放LoadFile申请的空间（内存管理要十分慎重）
 
		enum_field_types type = MYSQL_TYPE_STRING;    //类型
		const char *data = 0;  //存放具体数据
		int size = 0;          //数据大小(int类型限制了文件最大大小，需要的话可改为long)
 
	};
}
#endif
