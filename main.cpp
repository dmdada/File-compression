#include "FileCompress.h"

void show_list()//�˵�
{
	cout <<   "�ļ�ѹ���˵�: " << endl;
	cout << endl;
	cout << "1.ѹ���ļ�" << endl;
	cout << "2.��ѹ�ļ�" << endl;
	cout << "0.�˳�����" << endl;
	cout << endl;
}

void window()
{
	show_list();
	int ctl = 1;

	while (ctl)
	{
		cout << "���������ѡ�� " << endl;
		cin >> ctl;
		string file;
		switch (ctl)
		{
		case 1:
		{
				  cout << "�����ļ�·��/����ֱ�������ļ����ƣ���Ҫ��ǰĿ¼��: " << endl;
				 cin >> file;
				  FileCompress cps;
				  cps.Compress(file.c_str());
				 cout << "ѹ���ɹ�" << endl;
				  ctl = 0;
		}
			break;
		case 2:
		{
				  cout << "�����ļ�·�� / ����ֱ�������ļ����ƣ���Ҫ��ǰĿ¼��: " << endl;
				  cin >> file;
				  FileCompress cps;
				  cps.UnCompress(file.c_str());
				  cout << "��ѹ�ɹ�!" << endl;
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