#include "Reader.hpp"
#include "Writer.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <queue>
using std::cin;
using std::cout;
using std::endl;
using std::greater;
using std::make_pair;
using std::priority_queue;
using std::sort;

const int MAXT = 8929;
const int MAXN = 136;

int EdgeTemporyAssign[MAXN]; //某时刻的临时分配序列,此编号不是edge的编号,而是排序后的序列号
int CrtDemandPos = 1; //未分配的t起始标
int T = 100; // T大小
int NoCost = 0; //最大空位数
int n = 18; //边缘节点数量

Data data;

struct Edge {
    int name; //名字,由于下标会变动,新增一个name(从1开始,输出时减去1即可)
    int flow; //边缘节点流量
    int EdgeType; // step1后边缘节点类型,0 1
    int EdgeCostFlag; //边缘节点分配次数
    bool operator>(const Edge& a) const
    {
        //按流量从大到小排序,否则按序号排序
        if (flow != a.flow) {
            return flow < a.flow;
        } else {
            return name > a.name;
        }
    }
} edge[MAXN];
struct Demand {
    int name;
    int flow;
} TimeTotalDemand[MAXT]; //每个时刻的总需求'

vector<vector<vector<pair<int, int>>>> plan; //输出方案
vector<vector<int>> demand; // demand[t][i]:t时刻第i个客户(t,i从1开始编号)
vector<string> edge_node; //第j个边缘节点(从1开始编号)
vector<string> customer_node;

void getData()
{
    data = readData({
        "./data/demand.csv",
        "./data/site_bandwidth.csv",
        "./data/qos.csv",
    });
}

bool EdgeCmp(const Edge& a, const Edge& b)
{
    //令type=1放前面(空位已满)
    if (a.EdgeType && b.EdgeType) {
        if (a.flow != b.flow) {
            return a.flow > b.flow;
        }
        return a.name < b.name;
    }
    return a.EdgeType > b.EdgeType;
}
bool DemandCmp(Demand a, Demand b)
{
    if (a.flow != b.flow) {
        return a.flow > b.flow;
    }
    return a.name < b.name;
}
void GenerateAvgAssign(int amount)
{
    //为尽可能不改变节点剩余流量顺序,把不足的依次从小流量节点中减去
    int avg = TimeTotalDemand[CrtDemandPos].flow / amount;
    int sub = TimeTotalDemand[CrtDemandPos].flow % amount;
    if (sub == 0) {
        for (int i = 1; i <= amount; i++) {
            EdgeTemporyAssign[i] = avg;
        }
    } else {
        for (int i = 1; i <= amount; i++) {
            EdgeTemporyAssign[i] = avg;
            //将余下的加到大流量节点上
            if (sub != 0) {
                sub--;
                EdgeTemporyAssign[i]++;
            }
        }
    }
}
void Assign(int EdgeEnd)
{
    // cout << "total cost=" << TimeTotalDemand[CrtDemandPos] << endl;
    for (int pos = 1; pos <= EdgeEnd; pos++) {
        edge[pos].flow -= EdgeTemporyAssign[pos];
        edge[pos].EdgeCostFlag++;
        //**在此分流量
        cout << "edge " << edge[pos].name << " get " << EdgeTemporyAssign[pos] << " at " << CrtDemandPos << endl;
    }

    int t = TimeTotalDemand[CrtDemandPos].name;
    int edgepos = 1;
    int demandpos = 1;
    while (edgepos <= n && demandpos <= demand[t].size()) {
        int DemandTmp = demand[t][demandpos];
        if (DemandTmp < EdgeTemporyAssign[edgepos]) {
            plan[t][demandpos].push_back(make_pair(DemandTmp, edge[edgepos].name));
            EdgeTemporyAssign[edgepos] -= DemandTmp;
            demandpos++;
        } else {
            plan[t][demandpos].push_back(make_pair(EdgeTemporyAssign[edgepos], edge[edgepos].name));
            demand[t][demandpos] -= EdgeTemporyAssign[edgepos];
            edgepos++;
        }
    }
}
void AssignSingle(int pos)
{
    cout << "edge " << edge[pos].name << " get " << EdgeTemporyAssign[pos] << " at " << CrtDemandPos << endl;
    int t = TimeTotalDemand[CrtDemandPos].name; //得到真正的时间
    for (int i = 1; i <= demand[t].size(); i++) {
        plan[t][i].push_back(make_pair(demand[t][i], edge[pos].name));
    }
}
void step2()
{
    // todo:做一些处理去除I型以降低成本
    //平均分配法:
    //对剩下的节点进行排序
    sort(edge + 1, edge + n, EdgeCmp);

    //寻找type0和type1的分界
    int TypeSplit = 0; // type1(空位未满)的开始下标
    if (!edge[n].EdgeType) {
        for (int i = 1; i <= n; i++) {
            if (!edge[i].EdgeType) {
                TypeSplit = i;
                break;
            }
        }
    } else {
        TypeSplit = n + 1;
    }
    int n2 = TypeSplit - 1; // 2型节点数量(也是最后一个节点位置)

    while (CrtDemandPos <= T) {
        GenerateAvgAssign(n2);

        //超过了流量,修正调配方案
        if (EdgeTemporyAssign[n2] > edge[n2].flow) {
            //加上倍数
            int times = (EdgeTemporyAssign[n2] - edge[n2].flow) / (n2 - 1);
            for (int i = 1; i <= n2 - 1; i++) {
                EdgeTemporyAssign[i] += times;
            }
            //加上余数
            int sub = (EdgeTemporyAssign[n2] - edge[n2].flow) % n2;
            for (int i = 1; i <= sub; i++) {
                EdgeTemporyAssign[i]++;
            }
            Assign(n2);
            n2--; //最后一个节点归零,前移
            continue;
        }

        Assign(n2);
        CrtDemandPos++;
    }
}
void PrintEdge()
{
    cout << "PrintEdge:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "edge " << edge[i].name << " type " << edge[i].EdgeType << " remains " << edge[i].flow << " cost " << edge[i].EdgeCostFlag << endl;
    }
}
void step1()
{
    priority_queue<Edge, vector<Edge>, greater<Edge>> q;
    //用优先队列排序剩余流量改进算法
    for (int i = 1; i <= n; i++) {
        q.push(edge[i]);
    }

    while (!q.empty()) {
        // todo:考虑到qos,这里要做改动
        struct Edge e = q.top();
        q.pop();
        //去除I型
        if (e.EdgeCostFlag >= NoCost) {
            edge[e.name].EdgeType = 1;
            continue;
        }
        //最大流量也不足以分配,退出
        if (e.flow < TimeTotalDemand[CrtDemandPos].flow) {

            break;
        }

        edge[e.name].flow -= TimeTotalDemand[CrtDemandPos].flow;
        edge[e.name].EdgeCostFlag++;

        //调用分配函数
        EdgeTemporyAssign[e.name] = TimeTotalDemand[CrtDemandPos].flow;
        AssignSingle(e.name);
        EdgeTemporyAssign[e.name] = 0; //清零

        q.push(Edge { edge[e.name].name, edge[e.name].flow, edge[e.name].EdgeType, edge[e.name].EdgeCostFlag });

        CrtDemandPos++;
        if (CrtDemandPos >= T) {
            //分配完了
            break;
        }
    }
}
void genNodeNameList()
{
    customer_node.resize(data.customer_node_count_ + 1);
    for (auto& p : data.customer_node_) {
        customer_node[p.second] = p.first;
    }
    edge_node.resize(data.edge_node_count_ + 1);
    for (auto& p : data.edge_node_) {
        edge_node[p.second] = p.first;
    }
}
void init()
{
    //边缘节点流量
    for (int i = 1; i <= n; i++) {
        edge[i].name = i;
        edge[i].flow = 430080;
    }
    //读入客户节点流量
    for (int i = 1; i <= T; i++) {
        cin >> TimeTotalDemand[i].flow;
    }

    NoCost = floor(T * 0.05); //空位数
    sort(TimeTotalDemand + 1, TimeTotalDemand + T + 1, DemandCmp);
    memset(EdgeTemporyAssign, 0, sizeof(EdgeTemporyAssign));
}
int main()
{
    getData();
    /*init();
    step1();
    step2();
    PrintEdge();*/
    genNodeNameList();
    vector<vector<vector<pair<int, int>>>> testv;
    testv.resize(data.day_ + 1);
    for (int i = 0; i <= data.day_; ++i) {
        testv[i].resize(data.customer_node_count_ + 1);
        for (int j = 0; j <= data.customer_node_count_; ++j) {
            testv[i][j].resize(data.edge_node_count_ + 1);
            for (int k = 0; k <= data.edge_node_count_ + 1; ++k) {
                testv[i][j][k] = { k, 0 };
            }
        }
    }
    WriteData("./output/solution.txt", customer_node, edge_node, std::move(testv));
    return 0;
}
