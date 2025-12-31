#include "KDTree.h"

template<class T, class DisType>
bool KD_Tree<T, DisType>::SetElement(T* pT, int n)
{
	N = n;
	Arr.resize(N);
	for (int i = 0; i < N; ++i)
		Arr[i] = { pT[i],i };
	lson.resize(N);
	rson.resize(N);
	Dimention.resize(N);
	for (int i = 0; i < N; ++i)
		lson[i] = rson[i] = -1;
	return true;
}

template<class T, class DisType>
bool KD_Tree<T, DisType>::SetSubtraction(int k, ...)
{
	Minus.clear();
	va_list arg;
	va_start(arg, k);
	for (int i = 0; i < k; ++i)
		Minus.push_back(va_arg(arg, Subtraction*));
	va_end(arg);
	K = k;
	Mmax.resize(K), Mmin.resize(K);
	for (int i = 0; i < K; ++i)
		Mmax[i].resize(N), Mmin[i].resize(N);
	return true;
}

template<class T, class DisType>
void KD_Tree<T, DisType>::Update(int now)
{
	for (int i = 0; i < K; ++i)
		Mmin[i][now] = Mmax[i][now] = now;
	int l = lson[now], r = rson[now];
	if (l >= 0) {
		for (int i = 0; i < K; ++i) {
			if (Minus[i](Arr[Mmin[i][now]].A, Arr[Mmin[i][l]].A) > 0)
				Mmin[i][now] = Mmin[i][l];
			if (Minus[i](Arr[Mmax[i][now]].A, Arr[Mmax[i][l]].A) < 0)
				Mmax[i][now] = Mmax[i][l];
		}
	}
	if (r >= 0) {
		for (int i = 0; i < K; ++i) {
			if (Minus[i](Arr[Mmin[i][now]].A, Arr[Mmin[i][r]].A) > 0)
				Mmin[i][now] = Mmin[i][r];
			if (Minus[i](Arr[Mmax[i][now]].A, Arr[Mmax[i][r]].A) < 0)
				Mmax[i][now] = Mmax[i][r];
		}
	}
}

template<class T, class DisType>
int KD_Tree<T, DisType>::Build(int l, int r, int D)
{
	auto Less = [this, &D](const Tree_Node& aa, const Tree_Node& bb) { return Minus[D](aa.A, bb.A) < 0; };
	int mid = (l + r) >> 1;
	std::nth_element(Arr.begin() + l, Arr.begin() + mid, Arr.begin() + r, Less);

	Dimention[mid] = D;
	if (l < mid)lson[mid] = Build(l, mid, (D + 1) % K);
	if (mid + 1 < r)rson[mid] = Build(mid + 1, r, (D + 1) % K);

	Update(mid);
	return mid;
}

template<class T, class DisType>
bool KD_Tree<T, DisType>::BuildTree()
{
	root = Build(0, N, 0);
	return true;
}

template<class T, class DisType>
DisType KD_Tree<T, DisType>::Euclidean(DisType dis)
{
	return dis * dis;
}

template<class T, class DisType>
DisType KD_Tree<T, DisType>::Manhattan(DisType dis)
{
	return dis < 0 ? -dis : dis;
}

template<class T, class DisType>
DisType KD_Tree<T, DisType>::Distance(const T& a, const T& b)
{
	DisType sum = 0;
	for (int i = 0; i < K; ++i)
		sum += DisFunc(Minus[i](a, b));
	return sum;
}

template<class T, class DisType>
DisType KD_Tree<T, DisType>::MinDis(const T& a, int now)
{
	DisType MinDist = 0, P, Q;
	for (int i = 0; i < K; ++i) {
		P = std::max(Minus[i](a, Arr[Mmax[i][now]].A), DisType(0));
		Q = std::max(Minus[i](Arr[Mmin[i][now]].A, a), DisType(0));
		MinDist += DisFunc(P + Q);
	}
	return MinDist;
}

template<class T, class DisType>
DisType KD_Tree<T, DisType>::MaxDis(const T& a, int now)
{
	DisType MaxDist = 0, P, Q;
	for (int i = 0; i < K; ++i) {
		P = Minus[i](a, Arr[Mmin[i][now]].A);
		Q = Minus[i](a, Arr[Mmax[i][now]].A);
		MaxDist += std::max(DisFunc(P), DisFunc(Q));
	}
	return MaxDist;
}

template<class T, class DisType>
void KD_Tree<T, DisType>::FindNearest(const T& a, int now, DisType DisINF)
{
	if (now < 0)return;
	DisType nowDis = Distance(a, Arr[now].A);
	if (nowDis < qMax.top().first)
		qMax.pop(), qMax.push({ nowDis,Arr[now].index });
	int l = lson[now], r = rson[now];
	DisType lDis = DisINF, rDis = DisINF;
	if (l != -1)lDis = MinDis(a, l);
	if (r != -1)rDis = MinDis(a, r);
	if (lDis < rDis) {
		if (lDis <= qMax.top().first)FindNearest(a, l, DisINF);
		if (rDis <= qMax.top().first)FindNearest(a, r, DisINF);
	}
	else {
		if (rDis <= qMax.top().first)FindNearest(a, r, DisINF);
		if (lDis <= qMax.top().first)FindNearest(a, l, DisINF);
	}
}

template<class T, class DisType>
void KD_Tree<T, DisType>::FindFarthest(const T& a, int now)
{
	if (now < 0)return;
	DisType nowDis = Distance(a, Arr[now].A);
	if (nowDis > qMin.top().first)//|| (nowDis == qMin.top().first && Arr[now].index < -qMin.top().second)
		qMin.pop(), qMin.push({ nowDis,Arr[now].index });
	int l = lson[now], r = rson[now];
	DisType lDis = 0, rDis = 0;
	if (l != -1)lDis = MaxDis(a, l);
	if (r != -1)rDis = MaxDis(a, r);
	if (lDis > rDis) {
		if (lDis >= qMin.top().first)FindFarthest(a, l);
		if (rDis >= qMin.top().first)FindFarthest(a, r);
	}
	else {
		if (rDis >= qMin.top().first)FindFarthest(a, r);
		if (lDis >= qMin.top().first)FindFarthest(a, l);
	}
}

template<class T, class DisType>
int KD_Tree<T, DisType>::FindKthNearest(const T& a, int k, DisType DisINF)
{
	while (!qMax.empty())qMax.pop();
	while (k--)qMax.push({ DisINF,-1 });
	FindNearest(a, root, DisINF);
	return qMax.top().second;
}
template<class T, class DisType>
int KD_Tree<T, DisType>::FindKthFarthest(const T& a, int k)
{
	while (!qMin.empty())qMin.pop();
	while (k--)qMin.push({ DisType(-1),0 });
	FindFarthest(a, root);
	return qMin.top().second;
}