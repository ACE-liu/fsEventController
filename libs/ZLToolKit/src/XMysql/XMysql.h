#if defined(ENABLE_MYSQL)
#pragma once
#include"XData.h"
#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include<mysql.h>

using namespace std;
 
//命名空间开始
namespace XMYSQL {
 
	class XMysql
	{
	public:
		XMysql();
		~XMysql();
 
		bool Init();   //初始化mysql
		void Close();  //清理占用的所有资源
 
		//数据库连接(不考虑线程安全) flag：设置支持多条语句
		bool Conncet(const char*host,const char* user,const char* pass,const char*db,unsigned short port=3306,unsigned long flag=0);
		//执行sql语句 (if sqllen==0 使用strlen自动获取)
		bool Query(const char *sql, unsigned long sqllen = 0);
		//Mysql参数的设定（在Connect之前调用）
		bool Options(enum mysql_option option, const void *arg);
		//设置连接超时时间
		bool SetConnectTimeout(int sec);      
		//自动重连，默认不自动
		bool SetReconnect(bool isre = true);
 
		//结果集获取
		//1 返回全部结果
		bool StoreResult();
		//2 开始接受结果，通过Fetch获取
		bool UseResult();
 
		//释放结果集占用的空间
		void FreeResult();
 
		//获取一行数据
		vector<XData> FetchRow();
 
		//自动根据map生成insert SQL语句
		string GetInsertSql(map<string,XData> kv,string table);  //使用XData结构体是因为既能传字符串内容，也能传二进制内容到const char*
 
		//插入非二进制数据
		bool Insert(map<string, XData> kv, string table);
 
		//插入二进制数据
		bool InsertBin(map<string, XData> kv, string table);
 
		//获取Update数据的sql语句（输入为 关键字-值）
		string GetUpdateSql(map<string, XData> kv, string table,string where);
 
		//修改非二进制数据，返回更新影响的记录数，失败返回-1
		int Update(map<string, XData> kv, string table, string where);
 
		//修改二进制数据，返回更新影响的记录数，失败返回-1
		int UpdateBin(map<string, XData> kv, string table, string where);
 
		//开始事务
		bool StartTransaction();
 
		//结束事务
		bool StopTransactiom();
		
		//事务提交
		bool Commit();
 
		//回滚
		bool Rollback();
 
 
	protected:
		MYSQL *mysql = nullptr;   //mysql句柄	
		MYSQL_RES *result = nullptr;  //结果集
	};
 
}//命名空间结束
#endif

