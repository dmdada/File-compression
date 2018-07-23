#include "FileCompress.h"

void show_list()//菜单
{
	cout <<   "文件压缩菜单: " << endl;
	cout << endl;
	cout << "1.压缩文件" << endl;
	cout << "2.解压文件" << endl;
	cout << "0.退出程序" << endl;
	cout << endl;
}

void window()
{
	show_list();
	int ctl = 1;

	while (ctl)
	{
		cout << "请输入你的选择： " << endl;
		cin >> ctl;
		string file;
		switch (ctl)
		{
		case 1:
		{
				  cout << "输入文件路径/或者直接输入文件名称（需要当前目录）: " << endl;
				 cin >> file;
				  FileCompress cps;
				  cps.Compress(file.c_str());
				 cout << "压缩成功" << endl;
				  ctl = 0;
		}
			break;
		case 2:
		{
				  cout << "输入文件路径 / 或者直接输入文件名称（需要当前目录）: " << endl;
				  cin >> file;
				  FileCompress cps;
				  cps.UnCompress(file.c_str());
				  cout << "解压成功!" << endl;
				  ctl = 0;
		}
			break;
		default:
			break;
		}
	}
	
}

int main()
{
	window();
	//FileCompress cps;
	//cps.Compress("dm.jpg");
	//FileCompress cps2;
	//cps2.UnCompress("dm.hl");
	system("pause");
	return 0;
}