#include "Registration.h"

float minus1(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.posit[0] - b.posit[0];
}
float minus2(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.posit[1] - b.posit[1];
}
float minus3(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.posit[2] - b.posit[2];
}
float minus4(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.color[0] - b.color[0];
}
float minus5(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.color[1] - b.color[1];
}
float minus6(const VERTEXCOLOR& a, const VERTEXCOLOR& b) {
    return a.color[2] - b.color[2];
}

bool registration(Object3d& a, Object3d& b, float4x4& rotaMat, float4& transVec, bool& Flag)
{
    char DebugBuffer[DebugLens] = {};
    // Maximum Matches Minimum Cost算法开始
    int K = 8;
    // 定义配准所需变量
    KD_Tree<VERTEXCOLOR, float> tree;
    Digraph<float> G;
    std::vector<float4> plist1, plist2;
    // 构造KD树
    tree.SetElement(&a.pVertex[0], a.VertexNum);
    tree.SetSubtraction(6, minus1, minus2, minus3, minus4, minus5, minus6);
    tree.BuildTree();
    // 初始化有向图G
    // a.VertexNum + b.VertexNum:源点
    // a.VertexNum + b.VertexNum + 1:汇点
    // [0,b.VertexNum):模型b的点的标号
    // [b.VertexNum,a.VertexNum+b.VertexNum):模型a的点的标号
    // completeness:完成度（%）
    int T = a.VertexNum + b.VertexNum, completeness = 0;
    G.init(a.VertexNum + b.VertexNum + 1u, (K * b.VertexNum + a.VertexNum) * 2);
    // 构造匹配网络
    // 1、连接a模型与汇点
    for (unsigned int i = 0; i < a.VertexNum; i++)
        G.CreateEdge(i + b.VertexNum, T, 0.0f, 1.0f),
        G.CreateEdge(T, i + b.VertexNum, 0.0f, 0.0f);
    // 定义最优匹配所需变量
    // flow:流量，即配对数
    // cost:费用，配对的花费
    float flow = 0.0, cost = 0.0;
    for (unsigned int i = 0; i < b.VertexNum; i++) {
        // 查询模型a中距离模型b的第i个点前K近的点
        tree.FindKthNearest(b.pVertex[i], K, 10.0f);
        // 2、连接b模型的第i个点与a模型中的前5近的点
        while (!tree.qMax.empty()) {
            // 查询
            std::pair<float, int> temp = tree.qMax.top();
            tree.qMax.pop();
            // 连接
            if (temp.second == -1)continue;
            G.CreateEdge(i, temp.second + b.VertexNum, temp.first, 1.0f);
            G.CreateEdge(temp.second + b.VertexNum, i, -temp.first, 0.0f);
        }
        // 3、网络流计算当前的最优匹配
        G.Dinic_SPFA(i, T, flow, cost);
        // 调试输出
        if (i * 100 >= completeness * b.VertexNum) {
            snprintf(DebugBuffer, DebugLens, "%d%% %f %f\n", ++completeness, flow, cost);
            OutputDebugStringA(DebugBuffer);
        }
        if (Flag == true)
            return false;
    }
    // 将配对的点对填充进plist1与plist2
    // b模型的点填充进plist1
    // a模型的点填充进plist2

    std::vector<float> costlist;

    for (unsigned int u = 0; u < b.VertexNum; u++)
        for (int i = G.head[u]; i != -1; i = G.next[i])
            if (G.to[i] < T && G.flow[i] == 0.0f) {
                plist1.push_back(b.pVertex[G.from[i]].posit);
                plist2.push_back(a.pVertex[G.to[i] - b.VertexNum].posit);
                costlist.push_back(tree.Distance(b.pVertex[G.from[i]], a.pVertex[G.to[i] - b.VertexNum]));
            }
    FILE* fil = nullptr;
    fopen_s(&fil, "costlist.txt", "w");
    for (int i = 0; i < costlist.size(); i++)
        fprintf_s(fil, "%f\n", costlist[i]);
    fclose(fil);
    // Maximum Matches Minimum Cost算法结束

    /*for (unsigned int i = 0; i < b.VertexNum; i++)
        plist1.push_back(b.pVertex[i].posit);
    for (unsigned int i = 0; i < a.VertexNum; i++)
        plist2.push_back(a.pVertex[i].posit);*/
    for (unsigned int i = 0; i < plist1.size(); i++)
        plist1[i][1] = plist2[i][1] = 0.0f;

    // 调用SVD分解求得旋转矩阵和平移向量
    SVD((int)plist1.size(), &plist1[0], &plist2[0], rotaMat, transVec);
    return true;
}

bool SVD(int Num, float4* a, float4* b,float4x4& rotaMat,float4& transVec) {
    char DebugBuffer[DebugLens] = {};

    float4 mean1, mean2;
    mean1 << 0.0f, 0.0f, 0.0f, 0.0f;
    mean2 << 0.0f, 0.0f, 0.0f, 0.0f;
    // 求质心
    for (int i = 0; i < Num; i++)
        mean1 += a[i];
    mean1 /= (float)Num;
    for (int i = 0; i < Num; i++)
        mean2 += b[i];
    mean2 /= (float)Num;
    // 求去质心坐标
    for (int i = 0; i < Num; i++)
        a[i] -= mean1;
    for (int i = 0; i < Num; i++)
        b[i] -= mean2;
    // 求H矩阵
    float4x4 H;
    H << 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f;
    for (int i = 0; i < Num; i++)
        H += a[i].transpose() * b[i];
    // SVD分解
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
    float4x4 V = svd.matrixV();
    float4x4 U = svd.matrixU();
    // 求旋转矩阵
    rotaMat = U * V.transpose();
    // 
    bool invalidflag = rotaMat.determinant() < 0;
    // 求平移向量
    transVec = mean2 - mean1 * rotaMat;

    snprintf(DebugBuffer, DebugLens, "%.8f %.8f %.8f %.8f\n%.8f %.8f %.8f %.8f\n%.8f %.8f %.8f %.8f\n%.8f %.8f %.8f %.8f\n",
        rotaMat(0, 0), rotaMat(0, 1), rotaMat(0, 2), rotaMat(0, 3),
        rotaMat(1, 0), rotaMat(1, 1), rotaMat(1, 2), rotaMat(1, 3),
        rotaMat(2, 0), rotaMat(2, 1), rotaMat(2, 2), rotaMat(2, 3),
        rotaMat(3, 0), rotaMat(3, 1), rotaMat(3, 2), rotaMat(3, 3));
    OutputDebugStringA(DebugBuffer);
    OutputDebugStringA("\n");
    snprintf(DebugBuffer, DebugLens, "%d %f\n%.8f %.8f %.8f %.8f\n", invalidflag, rotaMat.determinant(),
        transVec[0], transVec[1], transVec[2], transVec[3]);
    OutputDebugStringA(DebugBuffer);

    return invalidflag;
}

Object3d SphericalFilter(Object3d& a, int k, float R)
{
    Object3d A;
    KD_Tree<VERTEXCOLOR, float> tree;

    // 构造KD树
    tree.SetElement(&a.pVertex[0], a.VertexNum);
    tree.SetSubtraction(6, minus1, minus2, minus3, minus4, minus5, minus6);
    tree.BuildTree();
    // 降噪
    for (unsigned int i = 0; i < a.VertexNum; i++)
    {
        int index = tree.FindKthNearest(a.pVertex[i], k, R + 1);
        float Dis = tree.qMax.top().first;
        if (index != -1 && Dis < R) {
            A.pVertex.push_back(a.pVertex[i]);
            A.pIndices.push_back((unsigned int)A.pIndices.size() - 1);
        }
    }
    A.VertexNum = (unsigned int)A.pVertex.size();
    A.VertexStrides = sizeof(VERTEXCOLOR);
    A.VertexOffsets = 0;
    A.IndexNum = (unsigned int)A.pIndices.size();
    A.IndexStrides = sizeof(unsigned int);
    A.IndexOffsets = 0;
    return A;
}
