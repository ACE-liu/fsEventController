#if defined(ENABLE_MYSQL)
#include "XMysql.h"
 
namespace XMYSQL {
 
	XMysql::XMysql()
	{
	}
 
	XMysql::~XMysql()
	{
	}
 
	bool XMysql::Init()
	{
		Close();   //确保之前没资源
		cout << "XMysql::Init()" << endl;
 
		//新创建一个MYSQL对象
		mysql = mysql_init(0);  
		if (!mysql)
		{
			cerr << "mysql_init failed! " << endl;  //标准输入，标准输出，错误输出
		}
		return true;
	}
 
	void XMysql::Close()
	{
		cout << "XMysql::Close() " << endl;
		FreeResult();
		if (mysql)
		{
			mysql_close(mysql);
			mysql = nullptr;
		}
	}
 
	bool XMysql::Conncet(const char*host, const char* user, const char* pass, const char*db, unsigned short port, unsigned long flag)
	{
		if(!mysql&&!Init())
		{
			cerr << "Connect failed! mysql is not init! " << endl; 
			return false;
		}
		if (!mysql_real_connect(mysql, host, user, pass, db, port, 0, flag))
		{
			cerr << "mysql connect failed! " <<mysql_error(mysql)<< endl;
			return false;
		}
		cout << "mysql connect success! " << endl;
		return true;
	}
 
	bool XMysql::Query(const char *sql, unsigned long sqllen)
	{
		if (!mysql)
		{
			cerr << "Query failed! mysql is NULL! " << endl;
			return false;
		}
		if(!sql)
		{
			cerr << "Query failed! sql is NULL! " << endl;
			return false;
		}
		if (sqllen <= 0)
		{
			sqllen = (unsigned long)strlen(sql);
		}
		if (!sql)
		{
			cerr << "Query failed! sql is NULL! " << endl;
			return false;
		}
		int re = mysql_real_query(mysql, sql, sqllen);
		if (re != 0)
		{
			cerr << "mysql_real_query failed!" << mysql_error(mysql) << endl;
			return false;
		}
		return true;
	}
 
	bool XMysql::Options(enum mysql_option option, const void *arg)
	{
		if (!mysql)
		{
			cerr << "Option failed! mysql is NULL! " << endl;
			return false;
		}
		int re = mysql_options(mysql, option, arg);
		if (re != 0)
		{
			cerr << "mysql_options failed!" << mysql_error(mysql) << endl;
			return false;
		}
		return true;
	}
 
	bool XMysql::SetConnectTimeout(int sec)
	{
		return Options(MYSQL_OPT_CONNECT_TIMEOUT,&sec);
	}
 
	//自动重连，默认不自动
	bool XMysql::SetReconnect(bool isre)
	{
 
		return Options(MYSQL_OPT_RECONNECT,&isre);
	}
 
	bool XMysql::StoreResult()
	{
		if (!mysql)
		{
			cerr << "StoreResult failed! mysql is NULL! " << endl;
			return false;
		}
		FreeResult();
		result = mysql_store_result(mysql);  //获取结果集
		if (!result)
		{
			cerr << "mysql_store_result failed!" << mysql_error(mysql) << endl;
			return false;
		}
		return true;
	}
 
	//2 开始接受结果，通过Fetch获取
	bool XMysql::UseResult()
	{
		if (!mysql)
		{
			cerr << "UseResult failed! mysql is NULL! " << endl;
			return false;
		}
		FreeResult();
		result = mysql_use_result(mysql);  //获取结果集
		if (!result)
		{
			cerr << "mysql_use_result failed!" << mysql_error(mysql) << endl;
			return false;
		}
		return true;
	}
 
	void XMysql::FreeResult()
	{
		if (result)
		{
			mysql_free_result(result);
			result = nullptr;
		}
	}
 
	vector<XData> XMysql::FetchRow()
	{
		vector<XData> re;
		if (!result)
		{
			return re;
		}
 
		MYSQL_ROW row = mysql_fetch_row(result); //获取一行
		if (!row)
		{
			return re;
		}
 
		//获取列数
		int num = mysql_num_fields(result);
		unsigned long *lens = mysql_fetch_lengths(result);   //获取每个字段内容的大小
		for (int i = 0; i < num; i++)  //挨个将一行中的每个字段插入到vector
		{
			XData xdata;
			xdata.data = row[i];
			xdata.size = lens[i];
			re.push_back(xdata);   //插入形成一行
		}
		return re;
 
	}
 
	string XMysql::GetInsertSql(map<string, XData> kv, string table)
	{
		string sql = "";
		if (kv.empty() || table.empty())
			return "";
		//inser to t_video (name,size) values('name1','1024')
		sql = "insert into ";
		sql += "`"+table+"` ";
		string keys = "";
		string vals = "";
	
		//遍历key - value
		for (auto iter = kv.begin(); iter != kv.end(); iter++)
		{
			keys += "`";
			keys += iter->first;
			keys += "`,";
 
			vals += "'";
			vals += iter->second.data;
			vals += "',";
		}
		//去除keys 和vals结尾后的逗号
		keys.erase(keys.size() - 1);
		vals.erase(vals.size() - 1);
		sql += "("+keys+")";
		sql += "values";
		sql += "(" + vals + ")";
 
		return sql;
	}
 
	bool XMysql::Insert(map<string, XData> kv, string table)
	{
		if (!mysql)
		{
			cerr << "Insert failed! mysql is NULL! " << endl;
			return false;
		}
 
		//拼接生成sql语句
		string sql = GetInsertSql(kv, table);
 
		if (sql.empty())
		{
			cerr << "Insert failed! sql is NULL! " << endl;
			return false;
		}
		if (Query(sql.c_str()))
			return false;
		int num = mysql_affected_rows(mysql);
		if (num <= 0)
			return false;
		return true;
	}
 
	bool XMysql::InsertBin(map<string, XData> kv, string table)
	{
		string sql = "";
		if (kv.empty() || table.empty()||!mysql)
			return false;
		//insert to t_video (name,size) values(?,?)
		sql = "insert into ";
		sql += "`" + table + "` ";
		string keys = "";
		string vals = "";
		//绑定字段
		MYSQL_BIND bind[256] = { 0 };
		int i = 0;
		//遍历key - value
		for (auto iter = kv.begin(); iter != kv.end(); iter++)
		{
			keys += "`";
			keys += iter->first;
			keys += "`,";
 
			vals += "?,";
			bind[i].buffer = const_cast<char*>(iter->second.data);
			bind[i].buffer_length = iter->second.size;
			bind[i].buffer_type = iter->second.type;
			i++;
		}
		//去除keys 和vals结尾后的逗号
		keys.erase(keys.size() - 1);
		vals.erase(vals.size() - 1);
		sql += "(" + keys + ")";
		sql += "values";
		sql += "(" + vals + ")";
 
		//初始化上下文
		MYSQL_STMT *stmt = mysql_stmt_init(mysql);
		if (!stmt)
		{
			cerr << "mysql_stmt_init failed!" << mysql_error(mysql) << endl;
			return false;
		}
 
		//预处理
		if (mysql_stmt_prepare(stmt, sql.c_str(),sql.length())!=0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_prepare failed!11" << mysql_error(mysql) << endl;
			return false;
		}
		//绑定
		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_bind_param failed! " << mysql_stmt_error(stmt) << endl;
			return false;
		}
		//执行
		if (mysql_stmt_execute(stmt) != 0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_execute failed! " << mysql_stmt_error(stmt) << endl;
			return false;
		}
		//释放资源
		mysql_stmt_close(stmt);
 
		return true;
	}
 
	string XMysql::GetUpdateSql(map<string, XData> kv, string table, string where)
	{
		string sql = "";
		if (kv.empty() || table.empty())
			return "";
		//update `t_video` set `name` = 'test_001' where `id`=`id`
		sql = "update ";
		sql += "`" + table + "`";
		sql += " set ";
		for (auto iter = kv.begin();iter != kv.end(); iter++)
		{
			sql += "`";
			sql += iter->first;
			sql += "`='";
			sql += iter->second.data;
			sql += "',";
		}
		sql.erase(sql.length() - 1);
		sql +=" where " +where;
		return sql;
	}
 
	int XMysql::Update(map<string, XData> kv, string table, string where)
	{
		if (!mysql)
		{
			return -1;
		}
		string sql = GetUpdateSql(kv, table, where);
		if (sql.empty())
		{
			return -1;
		}
		if (!Query(sql.c_str()))
		{
			return -1;
		}
		int num = mysql_affected_rows(mysql);
 
		return num;
	}
 
	int XMysql::UpdateBin(map<string, XData> kv, string table, string where)
	{
		if (!mysql)
		{
			return -1;
		}
		//update `t_video` set `data`=?,`name`=?,`size`=? where `id`='1'
		string sql = "";
		sql = "update ";
		sql += "`" + table + "`";
		sql += " set ";
		MYSQL_BIND bind[256] = { 0 };
		int i = 0;
		for (auto iter = kv.begin(); iter != kv.end(); iter++)
		{
			sql += "`";
			sql += iter->first;
			sql += "`=?,";
			bind[i].buffer = const_cast<char*>(iter->second.data);
			bind[i].buffer_length = iter->second.size;
			bind[i].buffer_type = iter->second.type;
			i++;
		}
		sql.erase(sql.length() - 1);
		sql += " where " + where;
 
		//初始化上下文
		MYSQL_STMT *stmt = mysql_stmt_init(mysql);
		if (!stmt)
		{
			cerr << "mysql_stmt_init failed!22" << mysql_error(mysql) << endl;
			return -1;
		}
		//预处理
		if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_prepare failed!22" << mysql_error(mysql) << endl;
			return -1;
		}
		//绑定
		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_bind_param failed! " << mysql_stmt_error(stmt) << endl;
			return -1;
		}
		//执行
		if (mysql_stmt_execute(stmt) != 0)
		{
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_execute failed! " << mysql_stmt_error(stmt) << endl;
			return -1;
		}
		//释放资源
 
		int num = mysql_stmt_affected_rows(stmt);
		mysql_stmt_close(stmt);
 
		return num;
	}
 
	bool XMysql::StartTransaction()
	{
		return Query("SET AUTOCOMMIT = 0");
	
	}
 
	bool XMysql::StopTransactiom()
	{
		return Query("SET AUTOCOMMIT = 1");
	}
 
	bool XMysql::Commit()
	{
		return Query("COMMIT");
	}
 
	//回滚
	bool XMysql::Rollback()
	{
		return Query("ROLLBACK");
	}
 
}  //命名空间

#endif
