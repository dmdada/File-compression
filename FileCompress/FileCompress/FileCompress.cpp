#include "FileCompress.h"
/*�ļ�ѹ�������ʵ��*/
void FileCompress::Compress(const string &FilePath)
{
	FILE *input = fopen(FilePath.c_str(), "rb");//�����Ʒ�ʽ���ļ�
	if (NULL == input)
	{
		cout << FilePath << " Not Found !" << endl;
		exit(1);
	}

	FillInfo(input);//ͳ���ַ���Ϣ������ȡhuffman����

	string CompressFileName;//����ѹ���ļ�����
	GetFileName(FilePath, CompressFileName);
	CompressFileName += ".hf";//����Ϊ.hf��ʽ�ļ����ļ���ʽ����Ϲȡ��

	FILE *output = fopen(CompressFileName.c_str(), "wb");//����ѹ���ļ�
	if (NULL == output)
	{
		cout << CompressFileName << " �ļ�����ʧ�� !" << endl;
		exit(2);
	}

	CompressCore(input, output, FilePath);//����ѹ���㷨

	fclose(input);//�ر��ļ�ѹ��
	fclose(output);
}

void FileCompress::UnCompress(const string &FilePath)//��ѹ��
{
	FILE *input = fopen(FilePath.c_str(), "rb");//��ѹ���ļ�
	if (NULL == input)
	{
		cout << FilePath << " δ���ָ��ļ�!" << endl;
		exit(3);
	}

	// ����ͷ����Ϣ
	string Postfix;
	GetHead(input, Postfix);//��ȡ�ļ���Ϣ

	// ��������ļ�
	size_t begin = FilePath.find_first_of("\\");
	if (begin == string::npos)
		begin = -1;
	size_t end = FilePath.find_last_of(".");
	if (end == string::npos)
		end = FilePath.length();
	string FileName = FilePath.substr(begin + 1, end - begin - 1);
	FileName += Postfix;
	FILE *output = fopen(FileName.c_str(), "wb");//������ѹ���ļ�
	if (NULL == output)
	{
		cout << FileName << " �޷������ļ� !" << endl;
		exit(4);
	}

	int i = 0;
	// ����ַ�
	for (; i < 256; ++i)
	{
		info[i].ch = i;
	}

	CodeInfo invalid;
	invalid.cnt = 0;
	HuffmanTree<CodeInfo> hfm(info, 256, invalid);//��������huffman��

	UnCompressCore(input, output, hfm.GetRoot());

	fclose(input);
	fclose(output);
}

void FileCompress::UnCompressCore(FILE *input, FILE *output, HuffmanTreeNode<CodeInfo> * pRoot)//��ѹ���㷨
{
	assert(NULL != input);
	assert(NULL != output);

	unsigned char ReadBuf[_SIZE_];//��ȡ������
	unsigned char WriteBuf[_SIZE_];
	memset(WriteBuf, '\0', _SIZE_);

	size_t n;
	size_t w_idx = 0;
	size_t pos = 0;
	HuffmanTreeNode<CodeInfo> * pCur = pRoot;
	long long file_len = pRoot->_weight.cnt;//�ļ��ַ������ܴ���
	do
	{
		memset(ReadBuf, '\0', _SIZE_);//���ö�ȡ������
		n = fread(ReadBuf, 1, _SIZE_, input);

		// ת��ReadBuf��WriteBuf
		size_t r_idx = 0;
		for (; r_idx < n; r_idx++)
		{
			// ת�������ֽ�
			unsigned char ch = ReadBuf[r_idx];
			for (; pos < 8; pos++, ch <<= 1)
			{
				if ((ch & 0x80) == 0x80)
				{
					pCur = pCur->pRight;
				}
				else
				{
					pCur = pCur->pLeft;
				}

				if (NULL == pCur->pLeft && NULL == pCur->pRight)//�Ӹ���㿪ʼ
				{
					WriteBuf[w_idx++] = pCur->_weight.ch;
					pCur = pRoot;
					if (w_idx == _SIZE_)//����������
					{
						fwrite(WriteBuf, 1, w_idx, output);//д���ļ�
						memset(WriteBuf, '\0', _SIZE_);//���³�ʼ��������
						w_idx = 0;
					}
					file_len--;
				} // if
				if (file_len == 0)
					break;
			} // for
			if (pos == 8)
				pos = 0;

		} // for

	} while (n > 0);

	if (w_idx < _SIZE_ && w_idx > 0)//����ʣ�ಿ��
		fwrite(WriteBuf, 1, w_idx, output);
}

void FileCompress::GetHead(FILE *src,string &Postfix)//��ȡ�ļ�ͷ��Ϣ
{
	assert(src);

	// ��ȡ��׺��
	unsigned char buf[_FILE_NAME_SIZE_];//�ļ���
	int size = _FILE_NAME_SIZE_;
	GetLine(src, buf, size);
	Postfix += (char *)buf;

	// ��ȡ����
	GetLine(src, buf, size);
	int line = atoi((char *)buf);

	// ��ȡ�������ַ����ִ���
	while (line--)
	{
		GetLine(src, buf, size);//��ȡÿ���ַ����Լ����ִ���
		info[*buf].cnt = atoi((char *)(buf + 2));//��ȡ�ַ����ִ���
	}
}

void FileCompress::GetLine(FILE *src, unsigned char *buf, int size)//��ȡһ����Ϣ
{
	assert(src);

	size_t n = 0;
	int index = 0;
	while (fread(buf + index, 1, 1, src) > 0)
	{
		if (index != 0 && buf[index] == '\n')
			break;

		index++;
	}
	buf[index] = '\0';
}


void FileCompress::GetFileName(const string &FilePath, string &output)//��ȡ�ļ�����
{
	

	size_t begin = FilePath.find_last_of("\\");
	if (begin == string::npos)
	{
		begin = -1;
	}
	size_t end = FilePath.find_last_of(".");
	if (end == string::npos)
	{
		end = FilePath.length();
	}
	output = FilePath.substr(begin + 1, end - begin - 1);//�����ļ���
}

void FileCompress::GetPostfixName(const string &FilePath, string &output)
{
	size_t begin = FilePath.find_last_of(".");
	if (begin != string::npos)
		output = FilePath.substr(begin, FilePath.length() - begin);
}

void FileCompress::FillInfo(FILE *src)//�ռ��ļ��ַ���Ϣ
{
	assert(src);

	
	// ����ַ�
	for (int i = 0; i < 256; ++i)
	{
		info[i].ch = i;
	}

	// �����ִ���
	unsigned char buf[_SIZE_];
	size_t n;
	do
	{
		n = fread(buf, 1, _SIZE_, src);
		size_t idx = 0;
		while (idx < n)
		{
			info[buf[idx++]].cnt++;
		}
	} while (n > 0);

	// ������
	CodeInfo invalid;
	invalid.cnt = 0;
	HuffmanTree<CodeInfo> hfm(info, 256, invalid);//����Huffman��

	FillCode(hfm.GetRoot());//��ȡ����������
}

void FileCompress::FillCode(const HuffmanTreeNode<CodeInfo> *pRoot)//�ļ���huffman����
{
	if (NULL != pRoot)
	{
		FillCode(pRoot->pLeft);
		FillCode(pRoot->pRight);

		if (NULL == pRoot->pLeft && NULL == pRoot->pRight)//����Ҷ�ӽڵ�
		{
			const HuffmanTreeNode<CodeInfo> *tmp = pRoot;
			string code;//�������Ķ���
			const HuffmanTreeNode<CodeInfo> *pParent = tmp->pParent;
			while (NULL != pParent)//��Ҷ�ӵ�������ռ����
			{
				if (pParent->pLeft == tmp)
				{
					code += "0";//����Ϊ0
				}
				else
				{
					code += "1";//�Һ���Ϊ1
				}
				tmp = pParent;
				pParent = tmp->pParent;
			}
			reverse(code.begin(), code.end());//����һ�±�Ϊ����huffman����
			info[pRoot->_weight.ch].code = code;//����÷�֧����
		}
	}
}

void FileCompress::CompressCore(FILE *src, FILE *dst, const string &FilePath)//srcԴ�ļ���dstѹ���ļ�
{
	assert(NULL != src);
	assert(NULL != dst);

	fseek(src, 0, SEEK_SET);//λ��ָ��

	unsigned char buf[_SIZE_];//������
	unsigned char out[_SIZE_];
	int out_idx = 0;//�������
	size_t n;//��ȡ���ֽ�
	int pos = 0;
	unsigned char ch = 0;

	SaveCode(dst, FilePath);//���������Ϣ���ļ��ײ�

	// ������
	do
	{
		// ����ȡÿ���ֽ�ת��
		memset(buf, '\0', _SIZE_);
		n = fread(buf, 1, _SIZE_, src);//��Դ�ļ��ж�������
		size_t idx = 0;
		while (idx < n)
		{
			// ת�������ֽ�
			const string &CurCode = info[buf[idx++]].code;//��ȡԴ�ļ���Դ�ַ�
			size_t len = CurCode.length();//��Ԫ�ַ�huffman���볤��
			size_t i_len = 0;
			while (i_len < len)
			{
				for (; pos < 8 && i_len < len; pos++)
				{
					ch <<= 1;
					if (CurCode[i_len++] == '1')
					{
						ch |= 1;
					}
				}

				// �Ȼ��浽out
				if (8 == pos)
				{
					out[out_idx++] = ch;
					pos = 0;
					ch = 0;

					// ������ļ�
					if (_SIZE_ == out_idx)
					{
						fwrite(out, 1, out_idx, dst);
						out_idx = 0;
					}
				}
			} //while
		} // while
	} while (n > 0);

	// ����ʣ���λ
	if (8 > pos && 0 < pos)
	{
		int j = 0;
		while (j++ < 8 - pos)
			ch <<= 1;
		out[out_idx++] = ch;
	}

	// ����ʣ����ֽ�
	if (out_idx > 0)
		fwrite(out, 1, out_idx, dst);
}

void FileCompress::SaveCode(FILE *dst, const string &FilePath)//��ȡ�ļ�ѹ��������Ϣ����д���ļ���
{
	

	assert(NULL != dst);

	string output;
	GetPostfixName(FilePath, output);//��ȡ��չ��
	output += "\n";//����

	string code;
	int cnt = 0;
	unsigned char buf[33];//������
	for (int i = 0; i < 256; ++i)
	{
		if (info[i].cnt != 0)
		{
			cnt++;
			code += info[i].ch;//��ȡ�ַ�
			code += ",";//���ŷָ�
			sprintf((char *)buf, "%lld", info[i].cnt);//д��һ�����ݵ�������
			
			code += (char *)buf;
			code += "\n";//����
		}
	}

	sprintf((char *)buf, "%d", cnt);//������ַ���
	output += (char *)buf;		
	output += "\n";
	output += code;

	fwrite(output.c_str(), 1, output.size(), dst);//д���ļ�
}

