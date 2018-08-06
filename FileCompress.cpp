#include "FileCompress.h"
/*文件压缩类具体实现*/
void FileCompress::Compress(const string &FilePath)
{
	FILE *input = fopen(FilePath.c_str(), "rb");//二进制方式打开文件
	if (NULL == input)
	{
		cout << FilePath << " Not Found !" << endl;
		exit(1);
	}

	FillInfo(input);//统计字符信息，并获取huffman编码
	string CompressFileName;//保存压缩文件名字
	GetFileName(FilePath, CompressFileName);
	CompressFileName += ".hf";//保存为.hf格式文件，文件格式是我瞎取得

	FILE *output = fopen(CompressFileName.c_str(), "wb");//创建压缩文件
	if (NULL == output)
	{
		cout << CompressFileName << " 文件创建失败 !" << endl;
		exit(2);
	}

	CompressCore(input, output, FilePath);//核心压缩算法

	fclose(input);//关闭文件压缩
	fclose(output);
}

void FileCompress::UnCompress(const string &FilePath)//解压缩
{
	FILE *input = fopen(FilePath.c_str(), "rb");//打开压缩文件
	if (NULL == input)
	{
		cout << FilePath << " 未发现该文件!" << endl;
		exit(3);
	}

	// 处理头部信息
	string Postfix;
	GetHead(input, Postfix);//获取文件信息

	// 创建输出文件
	size_t begin = FilePath.find_first_of("\\");
	if (begin == string::npos)
		begin = -1;
	size_t end = FilePath.find_last_of(".");
	if (end == string::npos)
		end = FilePath.length();
	string FileName = FilePath.substr(begin + 1, end - begin - 1);
	FileName += Postfix;
	FILE *output = fopen(FileName.c_str(), "wb");//创建解压缩文件
	if (NULL == output)
	{
		cout << FileName << " 无法创建文件 !" << endl;
		exit(4);
	}

	int i = 0;
	// 填充字符
	for (; i < 256; ++i)
	{
		info[i].ch = i;
	}

	CodeInfo invalid;
	invalid.cnt = 0;
	HuffmanTree<CodeInfo> hfm(info, 256, invalid);//重新生成huffman树

	UnCompressCore(input, output, hfm.GetRoot());

	fclose(input);
	fclose(output);
}

void FileCompress::UnCompressCore(FILE *input, FILE *output, HuffmanTreeNode<CodeInfo> * pRoot)//解压缩算法
{
	assert(NULL != input);
	assert(NULL != output);

	unsigned char ReadBuf[_SIZE_];//读取缓冲区
	unsigned char WriteBuf[_SIZE_];
	memset(WriteBuf, '\0', _SIZE_);

	size_t n;
	size_t w_idx = 0;
	size_t pos = 0;
	HuffmanTreeNode<CodeInfo> * pCur = pRoot;
	long long file_len = pRoot->_weight.cnt;//文件字符出现总次数
	do
	{
		memset(ReadBuf, '\0', _SIZE_);//设置读取缓冲区
		n = fread(ReadBuf, 1, _SIZE_, input);

		// 转换ReadBuf至WriteBuf
		size_t r_idx = 0;
		for (; r_idx < n; r_idx++)
		{
			// 转换单个字节
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

				if (NULL == pCur->pLeft && NULL == pCur->pRight)//从根结点开始
				{
					WriteBuf[w_idx++] = pCur->_weight.ch;
					pCur = pRoot;
					if (w_idx == _SIZE_)//缓存区已满
					{
						fwrite(WriteBuf, 1, w_idx, output);//写入文件
						memset(WriteBuf, '\0', _SIZE_);//重新初始化缓冲区
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

	if (w_idx < _SIZE_ && w_idx > 0)//处理剩余部分
		fwrite(WriteBuf, 1, w_idx, output);
}

void FileCompress::GetHead(FILE *src,string &Postfix)//获取文件头信息
{
	assert(src);

	// 获取后缀名
	unsigned char buf[_FILE_NAME_SIZE_];//文件名
	int size = _FILE_NAME_SIZE_;
	GetLine(src, buf, size);
	Postfix += (char *)buf;

	// 获取行数
	GetLine(src, buf, size);
	int line = atoi((char *)buf);

	// 获取并设置字符出现次数
	while (line--)
	{
		GetLine(src, buf, size);//获取每行字符，以及出现次数
		info[*buf].cnt = atoi((char *)(buf + 2));//获取字符出现次数
	}
}

void FileCompress::GetLine(FILE *src, unsigned char *buf, int size)//获取一行信息
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


void FileCompress::GetFileName(const string &FilePath, string &output)//获取文件名字
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
	output = FilePath.substr(begin + 1, end - begin - 1);//复制文件名
}

void FileCompress::GetPostfixName(const string &FilePath, string &output)
{
	size_t begin = FilePath.find_last_of(".");
	if (begin != string::npos)
		output = FilePath.substr(begin, FilePath.length() - begin);
}

void FileCompress::FillInfo(FILE *src)//收集文件字符信息
{
	assert(src);

	
	// 填充字符
	for (int i = 0; i < 256; ++i)
	{
		info[i].ch = i;
	}

	// 填充出现次数
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

	// 填充编码
	CodeInfo invalid;
	invalid.cnt = 0;
	HuffmanTree<CodeInfo> hfm(info, 256, invalid);//创建Huffman树

	FillCode(hfm.GetRoot());//获取哈夫曼编码
}

void FileCompress::FillCode(const HuffmanTreeNode<CodeInfo> *pRoot)//文件的huffman编码
{
	if (NULL != pRoot)
	{
		FillCode(pRoot->pLeft);
		FillCode(pRoot->pRight);

		if (NULL == pRoot->pLeft && NULL == pRoot->pRight)//到达叶子节点
		{
			const HuffmanTreeNode<CodeInfo> *tmp = pRoot;
			string code;//保存编码的对象
			const HuffmanTreeNode<CodeInfo> *pParent = tmp->pParent;
			while (NULL != pParent)//从叶子到根结点收集结点
			{
				if (pParent->pLeft == tmp)
				{
					code += "0";//左孩子为0
				}
				else
				{
					code += "1";//右孩子为1
				}
				tmp = pParent;
				pParent = tmp->pParent;
			}
			reverse(code.begin(), code.end());//逆序一下变为正常huffman编码
			info[pRoot->_weight.ch].code = code;//保存该分支编码
		}
	}
}

void FileCompress::CompressCore(FILE *src, FILE *dst, const string &FilePath)//src源文件，dst压缩文件
{
	assert(NULL != src);
	assert(NULL != dst);

	fseek(src, 0, SEEK_SET);//位置指针

	unsigned char buf[_SIZE_];//缓冲区
	unsigned char out[_SIZE_];
	int out_idx = 0;//输出标量
	size_t n;//读取的字节
	int pos = 0;
	unsigned char ch = 0;

	SaveCode(dst, FilePath);//保存编码信息到文件首部

	// 读数据
	do
	{
		// 依次取每个字节转换
		memset(buf, '\0', _SIZE_);
		n = fread(buf, 1, _SIZE_, src);//从源文件中读入数据
		size_t idx = 0;
		while (idx < n)
		{
			// 转换单个字节
			const string &CurCode = info[buf[idx++]].code;//获取源文件，源字符
			size_t len = CurCode.length();//求元字符huffman编码长度
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

				// 先缓存到out
				if (8 == pos)
				{
					out[out_idx++] = ch;
					pos = 0;
					ch = 0;

					// 输出到文件
					if (_SIZE_ == out_idx)
					{
						fwrite(out, 1, out_idx, dst);
						out_idx = 0;
					}
				}
			} //while
		} // while
	} while (n > 0);

	// 处理剩余的位
	if (8 > pos && 0 < pos)
	{
		int j = 0;
		while (j++ < 8 - pos)
			ch <<= 1;
		out[out_idx++] = ch;
	}

	// 处理剩余的字节
	if (out_idx > 0)
		fwrite(out, 1, out_idx, dst);
}

void FileCompress::SaveCode(FILE *dst, const string &FilePath)//获取文件压缩索引信息，并写入文件。
{
	

	assert(NULL != dst);

	string output;
	GetPostfixName(FilePath, output);//获取扩展名
	output += "\n";//换行

	string code;
	int cnt = 0;
	unsigned char buf[33];//缓冲区
	for (int i = 0; i < 256; ++i)
	{
		if (info[i].cnt != 0)
		{
			cnt++;
			code += info[i].ch;//获取字符
			code += ",";//逗号分隔
			sprintf((char *)buf, "%lld", info[i].cnt);//写入一组数据到缓冲区
			
			code += (char *)buf;
			code += "\n";//换行
		}
	}

	sprintf((char *)buf, "%d", cnt);//输出总字符数
	output += (char *)buf;		
	output += "\n";
	output += code;

	fwrite(output.c_str(), 1, output.size(), dst);//写入文件
}

