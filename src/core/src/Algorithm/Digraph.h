#pragma once
#include <gp/config.hpp>

#include <vector>
#include <limits>

#include "Heap.h"
#include "CircularQueue.h"

template<class DisType>
class Digraph {
	// Mmax,DisType数据类型的最大值
	// Mmin,DisType数据类型的最小值
	static constexpr DisType Mmax = std::numeric_limits<DisType>::max();
	static constexpr DisType Mmin = std::numeric_limits<DisType>::min();

	// qCir,一个循环队列，在SPFA算法中被使用
	CircularQueue<int> qCir;
	// qMin,一个小根堆，在Dijkstra算法中被使用
	Heap<std::pair<DisType, int>, MinHeap> qMin;
public:
	// N，表示最大顶点数
	// M，表示最大边数
	// cnt，表示当前图中的边数
	int  N, M, cnt;
	// way，dis，h在单源最短路算法SPFA和Dijkstra中被使用
	std::vector<int> way;
	std::vector<DisType> dis, h;
	// head[i],表示顶点i连接的第一条边的索引
	std::vector<int> head;
	// 边集
	// from[i],表示第i条边的出发点的编号
	// to[i],表示第i条边的目标点的编号
	// next[i],表示与第i条边相同出发点的下一条边的索引
	// lens[i],表示第i条边的边长
	// flow[i],表示第i条边的流量
	std::vector<int> from, to, next;
	std::vector<DisType> lens, flow;
public:
	Digraph() :N(0), M(0), cnt(0) {}
	~Digraph() = default;
	void init(int Vertex, int Edge);
	void CreateEdge(int x, int y, DisType z, DisType w = DisType(0));
	bool SPFA(int S, int T, DisType* Dis, int* Way);
	bool Dij(int S, int T, DisType* Dis, int* Way, DisType* H);
	void Dinic_Dij(int S, int T, DisType& Flow, DisType& Cost);
	void Dinic_SPFA(int S, int T, DisType& Flow, DisType& Cost);
};

template<class DisType>
inline void Digraph<DisType>::init(int Vertex, int Edge)
{
	N = Vertex, M = Edge, cnt = 0;
	// 点集
	head.resize(N);
	for (int i = 0; i < N; i++)
		head[i] = -1;
	// 边集
	from.resize(M);
	to.resize(M);
	next.resize(M);
	lens.resize(M);
	flow.resize(M);
}

template<class DisType>
inline void Digraph<DisType>::CreateEdge(int x, int y, DisType z, DisType w)
{
	next[cnt] = head[x]; head[x] = cnt;
	from[cnt] = x; to[cnt] = y; lens[cnt] = z; flow[cnt++] = w;
}

template<class DisType>
inline bool Digraph<DisType>::SPFA(int S, int T, DisType* Dis, int* Way)
{
	std::vector<bool> Vis;
	Vis.resize(N);
	for (int i = 0; i < N; ++i)
		Dis[i] = Mmax, Vis[i] = false;
	Dis[S] = 0;
	qCir.resize(N + 1);
	qCir.push(S);

	while (!qCir.empty()) {
		int u = qCir.front(); qCir.pop();
		Vis[u] = false;
		for (int i = head[u]; i != -1; i = next[i]) {
			if (Way != nullptr && !flow[i])continue;
			int v = to[i];
			if (Dis[v] > Dis[u] + lens[i]) {
				Dis[v] = Dis[u] + lens[i];
				if (Way != nullptr)
					Way[v] = i;
				if (!Vis[v])qCir.push(v);
				Vis[v] = true;
				if (Dis[qCir[0]] > Dis[qCir.back()])
					std::swap(qCir[0], qCir[qCir.size() - 1]);
				if (qCir.size() > 1 && Dis[qCir[1]] > Dis[qCir.back()])
					std::swap(qCir[1], qCir[qCir.size() - 1]);
			}
		}
	}
	return Dis[T] != Mmax;
}

template<class DisType>
inline bool Digraph<DisType>::Dij(int S, int T, DisType* Dis, int* Way, DisType* H)
{
	for (int i = 0; i < N; i++)
		Dis[i] = Mmax;
	Dis[S] = 0;
	qMin.push({ DisType(0),S });

	while (!qMin.empty()) {
		std::pair<DisType, int> now = qMin.top();
		qMin.pop();
		int u = now.second;
		if (Dis[u] < now.first)continue;

		for (int i = head[u]; i != -1; i = next[i]) {
			if (Way != nullptr && !flow[i])continue;
			int v = to[i];
			if (Way == nullptr || H == nullptr) {
				if (Dis[v] > Dis[u] + lens[i]) {
					Dis[v] = Dis[u] + lens[i];
					qMin.push({ Dis[v],v });
				}
			}
			else {
				if (Dis[v] > Dis[u] + lens[i] + H[u] - H[v]) {
					Dis[v] = Dis[u] + lens[i] + H[u] - H[v];
					Way[v] = i;
					qMin.push({ Dis[v],v });
				}
			}
		}
	}
	return Dis[T] != Mmax;
}

template<class DisType>
inline void Digraph<DisType>::Dinic_Dij(int S, int T, DisType& Flow, DisType& Cost)
{
	dis.resize(N);
	way.resize(N);
	h.resize(N);
	for (int i = 0; i < N; i++)
		h[i] = 0;

	while (Dij(S, T, &dis[0], &way[0], &h[0])) {
		for (int i = 0; i < N; i++)
			h[i] += dis[i];
		DisType liu = Mmax;
		for (int v = T, i = 0; v != S; v = from[i])
			i = way[v], liu = std::min(liu, flow[i]);
		for (int v = T, i = 0; v != S; v = from[i])
			i = way[v], flow[i] -= liu, flow[i ^ 1] += liu;
		Flow += liu;
		Cost += liu * h[T];
	}
}

template<class DisType>
inline void Digraph<DisType>::Dinic_SPFA(int S, int T, DisType& Flow, DisType& Cost)
{
	dis.resize(N);
	way.resize(N);

	while (SPFA(S, T, &dis[0], &way[0])) {
		DisType liu = Mmax;
		for (int v = T, i = 0; v != S; v = from[i])
			i = way[v], liu = std::min(liu, flow[i]);
		for (int v = T, i = 0; v != S; v = from[i])
			i = way[v], flow[i] -= liu, flow[i ^ 1] += liu;
		Flow += liu;
		Cost += liu * dis[T];
	}
}