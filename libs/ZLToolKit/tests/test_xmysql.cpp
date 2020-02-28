#if defined(ENABLE_MYSQL)
#include<iostream>
#include "XMysql/XMysql.h"

using namespace std;
using namespace XMYSQL;   //自定义库的命名空间（没冲突就用命名空间进行简写，有冲突就不用命名空间，改用XMYSQL::name()）
 
int main()
{
	XMysql xmysql;
	//1 初始化mysql上下文
	xmysql.Init();
 
	//2 连接数据库
	xmysql.SetConnectTimeout(3);
	xmysql.SetReconnect(true);
	xmysql.Conncet("127.0.0.1","t3","t3@123","multimedia");
	
	//3 执行sql语句
	//3-1 创建表
	string sql = "";
	sql = "CREATE TABLE IF NOT EXISTS `t_video`\
	(`id` INT AUTO_INCREMENT,\
	`name` VARCHAR(1024),\
	`data` BLOB,\
	`size` INT,\
	PRIMARY KEY(`id`))";
	cout << xmysql.Query(sql.c_str()) << endl;
 
	//3-2 清空表数据(包括自增id)
	sql = "TRUNCATE t_video";
	cout << xmysql.Query(sql.c_str()) << endl;
 
	//3-3 自动重连
	//while(1)
	//	cout<<xmysql.Query(sql.c_str()) << endl;
 
	//3-4 插入多个记录
	//sql = "insert into `t_video` (`name`) values ('test001')";
	//xmysql.Query(sql.c_str());
	//xmysql.Query(sql.c_str());
	//xmysql.Query(sql.c_str());
	//xmysql.Query(sql.c_str());
	//xmysql.Query(sql.c_str());
 
	//3-5 使用StoreResult读取
	//sql = "select * from `t_video`";
	//xmysql.Query(sql.c_str());
	//xmysql.StoreResult();
	//for(;;)   //从结果集中获取所有行并打印
	//{	
	//	vector<XData> one_row;
	//	one_row = xmysql.FetchRow();    //获取一行
	//	if (!one_row.size())   //读完则退出
	//		break;
	//	for (auto e : one_row) //打印一行结果
	//	{
	//		if (e.data)  //非NULL则打印
	//			cout << e.data;
	//	}
	//	cout << endl;
	//}
	//xmysql.FreeResult();
 
	//3-6 使用UseResult方式读取
	//xmysql.Query(sql.c_str());
	//xmysql.UseResult();
	//xmysql.FreeResult();
 
 
	//3-7 插入非二进制内容
	//map<string, XData> kv;
	//kv.insert(make_pair("name", "test_001"));
	//kv.insert(make_pair("size", "1222"));
	////cout << xmysql.GetInsertSql(kv, "t_video").c_str() << endl;
	//xmysql.Insert(kv, "t_video");
 
	//3-8 插入二进制文件
	//map<string, XData> kv;
	//XData file;
	//file.LoadFile("mysql.jpg");  //读文件并更新data、size、type
	//kv.insert(make_pair("name", "mysql.jpg"));
	//kv.insert(make_pair("data", file));
	//kv.insert(make_pair("size", &file.size));
	//xmysql.InsertBin(kv, "t_video");
	//xmysql.InsertBin(kv, "t_video");
	//xmysql.InsertBin(kv, "t_video");
	//xmysql.InsertBin(kv, "t_video");
	//file.Drop();
	
	//3-9 读取二进制文件并存在硬盘
	//sql = "select * from `t_video`";
	//xmysql.Query(sql.c_str());
	//xmysql.StoreResult();
	//vector<XData> one_row;
	//one_row = xmysql.FetchRow();    //获取一行
	//string outFilename = "1out_";
	//outFilename += one_row[1].data;
	//cout << outFilename.c_str() << endl;
	//one_row[2].SaveFile(outFilename.c_str());
	//xmysql.FreeResult();
 
	//3-10 Update非二进制数据
	//map<string, XData> kv;
	//kv.insert(make_pair("name", "update_test_002.jpg"));
	//kv.insert(make_pair("size", "2000"));
	//string where = "`id` = '1'";
	//int re = xmysql.Update(kv, "t_video", where);
	//cout << re << endl;
 
	//3-11 Update二进制数据
	//map<string, XData> kv_new;
	//XData file_new;
	//file_new.LoadFile("mysql_1.jpg");  //读文件并更新data、size、type
	//kv_new.insert(make_pair("name", "mysql_1.jpg"));
	//kv_new.insert(make_pair("data", file_new));
	//kv_new.insert(make_pair("size", &file_new.size));
	//xmysql.UpdateBin(kv_new, "t_video","`id`='1'");
	//file_new.Drop();
 
	//3-12 使用事务插入多条记录
	map<string, XData> kv;
	xmysql.StartTransaction();   //事务开
	kv.insert(make_pair("name", "test_trans001"));
	xmysql.Insert(kv, "t_video");
	xmysql.Insert(kv, "t_video");
	xmysql.Insert(kv, "t_video");
	xmysql.Rollback();      //回滚
	xmysql.Insert(kv, "t_video");
	xmysql.Insert(kv, "t_video");
	xmysql.Commit();			//提交
	xmysql.StopTransactiom();   //事务关
 
 
	//4 清理资源
	xmysql.Close();
	return 0;
}
#endif
