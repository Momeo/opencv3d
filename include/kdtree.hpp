// specific kdtree implement for colorPoint with (x,y,z,r,g,b)
#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <numeric>

#include "opencv3d.hpp"
namespace opencv3d
{
	using std::cout;
	using std::endl;
	using std::cerr;

	class KdTree {
	public:
		colorPoint root;
		KdTree* parent;
		KdTree* leftChild;
		KdTree* rightChild;
		int ClusterIdx;

		int attribute;

		//默认构造函数
		KdTree()
		{
			parent = NULL;
			leftChild = NULL;
			rightChild = NULL;
			ClusterIdx = -1;
		}

		//判断kd树是否只是一个叶子结点
		bool isLeaf()
		{
			return (rightChild == NULL && leftChild == NULL);
		}
		//判断是否是树的根结点
		bool isRoot()
		{
			return (parent == NULL);
		}
		//判断该子kd树的根结点是否是其父kd树的左结点
		bool isLeft()
		{
			return parent->leftChild->root == root;
		}
		//判断该子kd树的根结点是否是其父kd树的右结点
		bool isRight()
		{
			return parent->rightChild->root == root;
		}
	};

	std::vector<std::vector<float> > Transpose(std::vector<colorPoint> Matrix)
	{
		unsigned row = (unsigned)Matrix.size();
		unsigned col = 3;
		std::vector<float> tmp_x, tmp_y, tmp_z;
		for (auto p : Matrix)
		{
			tmp_x.push_back(p.x);
			tmp_y.push_back(p.y);
			tmp_z.push_back(p.z);
		}
		std::vector<std::vector<float> > Trans;
		Trans.push_back(tmp_x);
		Trans.push_back(tmp_y);
		Trans.push_back(tmp_z);
		return Trans;
	}

	float findMiddleValue(std::vector<float> vec)
	{
		sort(vec.begin(), vec.end());
		auto pos = vec.size() / 2;
		return vec[pos];
	}

	float computeVar(std::vector<float> A)
	{
		//std::sort(A.begin(), A.end());
		float sum = (float)std::accumulate(A.begin(), A.end(), 0);
		float mean = sum / A.size();

		float accum = 0;
		for (auto& i : A)
			accum += (i - mean) * (i - mean);
		float var = accum / (A.size() - 1);
		return var;
	}

	int findSplitAttribute(std::vector<std::vector<float> > transData)
	{
		std::vector<int> vars;
		for (auto i : transData)
			vars.push_back(computeVar(i));
		auto biggest = std::max_element(std::begin(vars), std::end(vars));
		int idx = std::distance(std::begin(vars), biggest);
		return idx;
	}

	//构建kd树
	void buildKdTree(KdTree* tree, std::vector<colorPoint> data, unsigned depth)
	{

		//样本的数量
		int samplesNum = data.size();
		//终止条件
		if (samplesNum == 0)
		{
			return;
		}
		if (samplesNum == 1)
		{
			tree->root = data[0];
			return;
		}
		//样本的维度
		std::vector<std::vector<float> > transData = Transpose(data);
		//选择切分属性
		//unsigned splitAttribute = depth % k;
		unsigned splitAttribute = findSplitAttribute(transData);
		std::vector<float> splitAttributeValues = transData[splitAttribute];
		//选择切分值
		float splitValue = findMiddleValue(splitAttributeValues);
		//cout << "splitValue" << splitValue  << endl;

		// 根据选定的切分属性和切分值，将数据集分为两个子集
		std::vector<colorPoint> subset1;
		std::vector<colorPoint> subset2;
		for (unsigned i = 0; i < samplesNum; ++i)
		{
			if (splitAttributeValues[i] == splitValue)
				tree->root = data[i];
			else
			{
				if (splitAttributeValues[i] < splitValue)
					subset1.push_back(data[i]);
				else
					subset2.push_back(data[i]);
			}
		}

		//子集递归调用buildKdTree函数
		//tree->attribute = splitAttribute;
		//tree->leftChild = new KdTree;
		//tree->leftChild->parent = tree;
		//tree->rightChild = new KdTree;
		//tree->rightChild->parent = tree;
		//if (subset1.empty() || subset2.empty())
		//	cout << "tes" << endl;
		//buildKdTree(tree->leftChild, subset1, depth + 1);
		//buildKdTree(tree->rightChild, subset2, depth + 1);

		tree->attribute = splitAttribute;
		if (!subset1.empty())
		{
			tree->leftChild = new KdTree;
			tree->leftChild->parent = tree;
			buildKdTree(tree->leftChild, subset1, depth + 1);
		}
		if (!subset2.empty())
		{
			tree->rightChild = new KdTree;
			tree->rightChild->parent = tree;
			buildKdTree(tree->rightChild, subset2, depth + 1);
		}
	}

	//逐层打印kd树
	void printKdTree(KdTree* tree, unsigned depth)
	{
		for (unsigned i = 0; i < depth; ++i)
			cout << "\t";

		cout << tree->root << endl;
		if (tree->leftChild == NULL && tree->rightChild == NULL)//叶子节点
			return;
		else //非叶子节点
		{
			if (tree->leftChild != NULL)
			{
				for (unsigned i = 0; i < depth + 1; ++i)
					cout << "\t";
				cout << " left:";
				printKdTree(tree->leftChild, depth + 1);
			}

			cout << endl;
			if (tree->rightChild != NULL)
			{
				for (unsigned i = 0; i < depth + 1; ++i)
					cout << "\t";
				cout << "right:";
				printKdTree(tree->rightChild, depth + 1);
			}
			cout << endl;
		}
	}

	//逐层打印kd树的值
	void printKdTreeValue(KdTree* tree)
	{
		cout << "节点值：" << tree->root << endl
			<< "IDX: " << tree->ClusterIdx << endl
			<< "是否为叶子节点: " << tree->isLeaf() << endl;

		if (tree->leftChild == NULL && tree->rightChild == NULL)//叶子节点
			return;
		else //非叶子节点
		{
			if (tree->leftChild != NULL)
				printKdTreeValue(tree->leftChild);
			if (tree->rightChild != NULL)
				printKdTreeValue(tree->rightChild);
		}
	}

	//逐层取得kd树的值
	void getKdTreeValue(KdTree* tree, std::vector<colorPoint>& out, std::vector<int>& idx)
	{
		idx.push_back(tree->ClusterIdx);
		out.push_back(tree->root);
		if (tree->leftChild == NULL && tree->rightChild == NULL)//叶子节点
			return;
		else //非叶子节点
		{
			if (tree->leftChild != NULL)
				getKdTreeValue(tree->leftChild, out, idx);
			if (tree->rightChild != NULL)
				getKdTreeValue(tree->rightChild, out, idx);
		}
	}

	// 寻找第一个
	bool searchUndefinedIdx(KdTree* tree, KdTree*& newTree)
	{
		//if (tree == NULL)
		//	return false;
		if (tree->ClusterIdx == -1)
		{
			newTree = tree;
			return true;
		}
		if (tree->leftChild == NULL && tree->rightChild == NULL)//叶子节点
			return false;
		//else
		//	return searchUndefinedIdx(tree->leftChild, newTree) | 
		//	searchUndefinedIdx(tree->rightChild, newTree);
		bool left = false;
		bool right = false;
		if (tree->leftChild != NULL)
			left = searchUndefinedIdx(tree->leftChild, newTree);
		if (tree->rightChild != NULL)
			right = searchUndefinedIdx(tree->rightChild, newTree);
		return (right | left);

	}

	//计算空间中两个点的距离
	float measureDistance(colorPoint point1, colorPoint point2, unsigned method)
	{
		switch (method)
		{
		case 0://欧氏距离
		{
			float res = pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2) + pow((point1.z - point2.z), 2);
			return sqrt(res);
		}
		case 1://曼哈顿距离
		{

			float res = abs(point1.x - point2.x) + abs(point1.y - point2.y) + abs(point1.z - point2.z);
			return res;
		}
		case 2://点云水平距离
		{
			float res = pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2) + 0.1 * pow((point1.z - point2.z), 2);
			return sqrt(res);
		}
		default:
		{
			cerr << "Invalid method!!" << endl;
			return -1;
		}
		}
	}

	// 递归寻找半径内的所有点
	void searchRadiusNeighbor(KdTree* tree, colorPoint goal, float radius, std::vector<colorPoint>& out)
	{
		KdTree* currentTree = tree;
		colorPoint currentNearest = currentTree->root;
		// 点云自定义距离
		float currentDistance = measureDistance(goal, currentNearest, 2);
		if (currentDistance <= radius)
			out.push_back(currentNearest);
		// 是否为叶节点
		if (!currentTree->isLeaf())
		{
			// 当前分割属性
			unsigned index = currentTree->attribute;
			float districtDistance = -1;
			switch (index)
			{
			case 0:
			{
				districtDistance = goal.x - currentTree->root.x;
				break;
			}
			case 1:
			{
				districtDistance = goal.y - currentTree->root.y;
				break;
			}case 2:
			{
				districtDistance = goal.z - currentTree->root.z;
				break;
			}
			default:
				break;
			}
			if (districtDistance == -1)
				cout << "出错districtDistance == -1" << endl;
			if (abs(districtDistance) <= radius)
			{
				if (currentTree->leftChild != NULL)
					searchRadiusNeighbor(currentTree->leftChild, goal, radius, out);
				if (currentTree->rightChild != NULL)
					searchRadiusNeighbor(currentTree->rightChild, goal, radius, out);
			}
			else
			{
				if (currentTree->rightChild != NULL && districtDistance < 0)
					searchRadiusNeighbor(currentTree->leftChild, goal, radius, out);
				else if (currentTree->rightChild != NULL && districtDistance > 0)
					searchRadiusNeighbor(currentTree->rightChild, goal, radius, out);
			}
		}
	}

	// 【限定条件isChosen下的】递归寻找半径内的所有点
	void searchRadiusNeighborConditional(KdTree* tree, KdTree* target_tree, float radius, std::vector<KdTree*>& out)
	{
		KdTree* currentTree = tree;
		colorPoint currentNearest = currentTree->root;
		colorPoint goal = target_tree->root;

		// 点云自定义距离
		if (currentTree->ClusterIdx == -1)
		{
			float currentDistance = measureDistance(goal, currentNearest, 0);
			if (currentDistance <= radius)
			{
				out.push_back(currentTree);
				currentTree->ClusterIdx = target_tree->ClusterIdx;
			}
		}


		// 是否为叶节点
		if (!currentTree->isLeaf())
		{
			// 当前分割属性
			unsigned index = currentTree->attribute;
			float districtDistance = goal[index] - currentTree->root[index];

			if (abs(districtDistance) <= radius)
			{
				if (currentTree->leftChild != NULL)
					searchRadiusNeighborConditional(currentTree->leftChild, target_tree, radius, out);
				if (currentTree->rightChild != NULL)
					searchRadiusNeighborConditional(currentTree->rightChild, target_tree, radius, out);
			}
			else
			{
				if (currentTree->leftChild != NULL && districtDistance < 0)
					searchRadiusNeighborConditional(currentTree->leftChild, target_tree, radius, out);
				else if (currentTree->rightChild != NULL && districtDistance > 0)
					searchRadiusNeighborConditional(currentTree->rightChild, target_tree, radius, out);
			}
		}
	}

}
