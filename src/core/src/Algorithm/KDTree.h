#pragma once
#include <gp/config.hpp>

#include <algorithm>
#include <stdarg.h>
#include <vector>

#include "Heap.h"

#define Euclidean_Distance 0
#define Manhattan_Distance 1

template <class T, class DisType>
class KD_Tree {
	// KD树中的节点的数据类型，包括数据以及其在原数组中的下标
	// The data type of the nodes in the KD tree, including the data and their indexes in the original array
	struct Tree_Node {
		T A;
		int index;
	};
	typedef DisType Subtraction(const T&, const T&);
	typedef DisType DisFunction(DisType);

public:
	// 小根堆qMin，用于存放前K最远邻居
	// Min heap qMin, used to store the previous K farthest neighbors
	Heap<std::pair<DisType, int>, MinHeap> qMin;
	// 大根堆qMax，用于存放前K最近邻居
	// Max heap qMax, used to store the previous K nearest neighbors
	Heap<std::pair<DisType, int>, MaxHeap> qMax;

private:
	// 函数指针数组Minus，用于存放第K个维度的减法
	// A array named Minus of function pointers, used to store the subtraction function in the Kth dimension
	std::vector<Subtraction*> Minus;
	// KD Tree中的节点
	// The nodes in the KD Tree
	std::vector<Tree_Node> Arr;
	// Mmin[K][Tree_node],表示Tree_node点在第K维度下最小值点的索引
	// Mmin[K][Tree_node], the index of the minimum point of the node Tree_node in the Kth dimension
	std::vector<std::vector<int>> Mmin;
	// Mmax[K][Tree_node],表示Tree_node点在第K维度下最大值点的索引
	// Mmax[K][Tree_node], the index of the maximum point of the node Tree_node in the Kth dimension
	std::vector<std::vector<int>> Mmax;
	// N,表示KD_Tree中有N个数据点;K,表示KD_Tree中有K个维度用于计算距离;root,表示根节点编号
	// N, means there are N data points in the KD_Tree
	// K, means there are K dimensions in the KD_Tree for distance calculation
	// root, means the index of the root node
	int N, K, root;

	// lson[Tree_node],表示Tree_node节点的左节点的索引
	// rson[Tree_node],表示Tree_node节点的右节点的索引
	// Dimention[Tree_node],表示Tree_node节点的轴垂直分割面
	// lson[Tree_node], the index of the left node of the node Tree_node
	// rson[Tree_node], the index of the right node of the node Tree_node
	// Dimention[Tree_node], the axis-aligned splitting plane of the node Tree_node
	std::vector<int> lson, rson, Dimention;

	// 距离函数指针DisFunc，可以指向Euclidean或Manhattan，默认指向Euclidean
	// Distance function pointer DisFunc, it can point to Euclidean or Manhattan, default point to Euclidean
	DisFunction* DisFunc;

	void Update(int now);
	int Build(int l, int r, int D);

	static DisType Euclidean(DisType dis);
	static DisType Manhattan(DisType dis);

	DisType MinDis(const T& a, int now);
	DisType MaxDis(const T& a, int now);
public:
	DisType Distance(const T& a, const T& b);
	void FindNearest(const T& a, int now, DisType DisINF);
	void FindFarthest(const T& a, int now);
public:
	KD_Tree() :N(0), K(0), root(-1), DisFunc(Euclidean) {}
	~KD_Tree() = default;
	bool SetElement(T* pT, int n);
	bool SetSubtraction(int k, ...);

	bool BuildTree();
	int FindKthNearest(const T& a, int k, DisType DisINF);
	int FindKthFarthest(const T& a, int k);
};
