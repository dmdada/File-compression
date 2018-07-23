#pragma once
#include <iostream>
#include <assert.h>
#include <queue>
#include <vector>
/*哈夫曼树的实现*/
template <typename T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(const T &data)
	: _weight(data)
	, pLeft(NULL)
	, pRight(NULL)
	, pParent(NULL)
	{}

	T 		_weight;//权重数组
	HuffmanTreeNode *pLeft;//左孩子
	HuffmanTreeNode *pRight;//右孩子
	HuffmanTreeNode *pParent;//双亲
};
template <typename T>
struct greater  //优先级队列实例化greater,使top为最小值

{

	bool operator()(const T &left, const T &right)
	{
		return left->_weight > right->_weight;
	}
};


template <typename T>
class HuffmanTree//哈夫曼树
{
public:
	HuffmanTree(const T *weight, int size, const T &invalid)//构造函数
		: pRoot(NULL)
	 , _invalid(invalid)//无效值
	{
		assert(NULL != weight && size >= 0);
		_Create(weight, size);
	}

	~HuffmanTree()//析构函数
	{
		_Destroy(pRoot);
	}

	
	HuffmanTreeNode<T> * GetRoot()
	{
		return pRoot;
	}

private:
	void _Destroy(HuffmanTreeNode<T> * &pRoot)//析构函数
	{
		if (NULL != pRoot)
		{
			_Destroy(pRoot->pLeft);
			_Destroy(pRoot->pRight);
			delete pRoot;
			pRoot = NULL;
		}
	}

	void _Create(const T *weight, int size)//创建哈夫曼树
	{
		if (0 == size)
			return;
		else if (1 == size)
		{
			if (*weight != _invalid)//如果不为无效值
				pRoot = new HuffmanTreeNode<T>(*weight);
		}
		else
		{
			std::priority_queue<HuffmanTreeNode<T> *, std::vector<HuffmanTreeNode<T>* >, greater<HuffmanTreeNode<T>*> >//优先级队列
				heap;

			for (int i = 0; i < size; ++i)
			{
				if (weight[i] != _invalid)
				{
					HuffmanTreeNode<T> *tmp = new HuffmanTreeNode<T>(weight[i]);
					heap.push(tmp);//压入
				}
			}

			HuffmanTreeNode<T> *pLeft, *pRight;
			while (heap.size() >= 2)
			{
				pLeft = heap.top();
				heap.pop();//弹出
				pRight = heap.top();
				heap.pop();

				HuffmanTreeNode<T> *pParent = new HuffmanTreeNode<T>(pLeft->_weight + pRight->_weight);
				pParent->pLeft = pLeft;
				pParent->pRight = pRight;
				pLeft->pParent = pParent;
				pRight->pParent = pParent;

				heap.push(pParent);
			}
			if (!heap.empty())
				pRoot = heap.top();//根节点为堆顶元素
		}
	}

private:
	HuffmanTreeNode<T>	*pRoot;
	T					_invalid;
};
