#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include<Windows.h>
#include "HuffmanTree.hpp"
using namespace std;
#define _SIZE_ 1024
#define _FILE_NAME_SIZE_ 128

struct CodeInfo
{
	CodeInfo()
	: code()
	, cnt(0)
	{}

	friend bool operator>(const CodeInfo &left, const CodeInfo &right)
	{
		return left.cnt > right.cnt;
	}

	friend bool operator!=(const CodeInfo &left, const CodeInfo &right)
	{
		return left.cnt != right.cnt;
	}

	friend CodeInfo operator+(const CodeInfo &left, const CodeInfo &right)
	{
		CodeInfo ret;
		ret.cnt = left.cnt + right.cnt;
		return ret;
	}

	unsigned char ch;// �ַ�
	string code;// ����
	long long cnt;// ���ִ���
};


class FileCompress
{
public:

	void Compress(const string &FilePath);// ѹ��
	void UnCompress(const string &FilePath);// ��ѹ��
private:
	void GetFileName(const string &FilePath, string &output);// ��ȡ�ļ���
	void GetPostfixName(const string &FilePath, string &output);// ��ȡ��չ��
	void FillInfo(FILE *src);// ���info��Ϣ
	void FillCode(const HuffmanTreeNode<CodeInfo> *pRoot);// ��������Ϣ
	void CompressCore(FILE *src, FILE *dst, const string &FilePath);// ����ѹ������
	void SaveCode(FILE *dst, const string &FilePath);// ���������Ϣ��ѹ���ļ��ײ�	
	void GetLine(FILE *src, unsigned char *buf, int size);// ��ȡһ��Ԫ��
	void GetHead(FILE *src, string &Postfix);// ��ȡͷ����Ϣ
	void UnCompressCore(FILE *input, FILE *output, HuffmanTreeNode<CodeInfo> * pRoot);// ���Ľ�ѹ����
private:
	CodeInfo*info=new CodeInfo[256];
};

