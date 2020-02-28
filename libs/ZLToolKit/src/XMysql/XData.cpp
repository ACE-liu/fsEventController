#if defined(ENABLE_MYSQL)
#include"XData.h"
namespace XMYSQL {
 
	bool XData::LoadFile(const char* filename)
	{
		if (!filename)
			return false;
		//打开文件
		ifstream in(filename, ios::in | ios::binary);  
		if (!in.is_open())
		{
			cerr << "LoadFile " << filename << " failed!" << endl;
		}
		//文件大小
		in.seekg(0, ios::end);  //移到尾部进行读取大小
		size = in.tellg();
		in.seekg(0, ios::beg);
 
		//cout << filename << "the file'size is : " << size << endl;
 
		if (size <= 0)  //空文件
		{
			return false;
		}
 
		data = new char[size];  //申请内存
		int readed = 0;
		while (!in.eof())
		{
			in.read(const_cast<char *>(data), size - readed);
			if (in.gcount() > 0)
				readed += in.gcount();  //返回本次读的数据大小
			else
				break;
		}
		in.close();
		this->type = MYSQL_TYPE_BLOB;
		return true;
	}
 
	void XData::Drop()
	{
		delete data;
		data = nullptr;
	}
 
	bool XData::SaveFile(const char* filename)
	{
		if (!data||size <= 0)
		{
			return false;
		}
			
		fstream out(filename, ios::out | ios::binary);
		if (!out.is_open())
		{
			cout << "SaveFile failed!open failed! " << filename << endl;
			return false;
		}
		out.write(data, size);
		cout << "size is :" << size << endl;
		out.close();
		return true;
	}
 
}
#endif
