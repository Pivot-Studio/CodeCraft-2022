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
int m = 0;

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
} TimeTotalDemand[MAXT]; //每个时刻的总需求

vector<vector<vector< pair<int, int> >>> plan; //输出方案plan[t][i][j]<flow,bandwidth> t时刻第i个用户的分配方案
vector<vector<int>> demand; // demand[t][i]:t时刻第i个客户(t,i从1开始编号)

void getData()
{
    data = readData({
        "E:\\Code\\CPP\\CodeCraft-2022\\data\\demand.csv",
        "E:\\Code\\CPP\\CodeCraft-2022\\data\\site_bandwidth.csv",
        "E:\\Code\\CPP\\CodeCraft-2022\\data\\qos.csv",
    });
}
void PrintEdge()
{
    cout << "PrintEdge:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "edge " << edge[i].name << " type " << edge[i].EdgeType << " remains " << edge[i].flow << " cost " << edge[i].EdgeCostFlag << endl;
    }
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
bool EdgeCmpByNum(const Edge& a, const Edge& b)
{
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
    while (edgepos <= n && demandpos < data.demand_[t].size()) {
        int DemandTmp = data.demand_[t][demandpos];
        if (DemandTmp < EdgeTemporyAssign[edgepos]) {
            //cout<<"1:Demandpos="<<demandpos<<" DemandTmp="<<DemandTmp<<" t="<<t<<" edgepos="<<edgepos<<endl;
            cout<<"1:edge "<<edgepos<<"|"<<edge[edgepos].name<<" get "<<EdgeTemporyAssign[edgepos]<<" t="<<t<<"|"<<CrtDemandPos<<" DemandTmp="<<DemandTmp<<endl;
            plan[t][demandpos].push_back(pair<int,int>(DemandTmp, edge[edgepos].name));
            EdgeTemporyAssign[edgepos] -= DemandTmp;
            demandpos++;
        } else {
            //cout<<"2:Demandpos="<<demandpos<<" DemandTmp="<<DemandTmp<<" t="<<t<<" edgepos="<<edgepos<<endl;
            cout<<"2:edge "<<edgepos<<"|"<<edge[edgepos].name<<" get "<<EdgeTemporyAssign[edgepos]<<" t="<<t<<"|"<<CrtDemandPos<<" DemandTmp="<<DemandTmp<<endl;
            plan[t][demandpos].push_back(pair<int,int>(EdgeTemporyAssign[edgepos], edge[edgepos].name));
            data.demand_[t][demandpos] -= EdgeTemporyAssign[edgepos];
            edgepos++;
        }
    }

    memset(EdgeTemporyAssign,0,sizeof(EdgeTemporyAssign));
}
void AssignSingle(int pos)
{
    cout << "edge " << edge[pos].name << " get " << EdgeTemporyAssign[pos] << " at " << TimeTotalDemand[CrtDemandPos].name<<" pos "<<CrtDemandPos << endl;
    int t = TimeTotalDemand[CrtDemandPos].name; //得到真正的时间
    for (int i = 1; i < data.demand_[t].size(); i++) {
        plan[t][i].push_back(pair<int,int>(data.demand_[t][i], edge[pos].name));
    }
}
void step2()
{
    //todo:做一些处理去除I型以降低成本
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

    /*sort(edge+1,edge+n2, EdgeCmpByNum);
    //确定95分位
    GenerateAvgAssign(n2);
    bool  flag = false;
    for (int i = 0; i <= n2; ++i) {
        if(EdgeTemporyAssign[i]>edge[n2].flow){
            flag = true;
            break;
        }
    }
    if(flag){
        // todo:修正
    }

    int NintyFive[MAXN];
    for (int i = 1; i <= n2; ++i) {
        NintyFive[i]=EdgeTemporyAssign[i];
    }
    Assign(n2);
    CrtDemandPos++;

    //剩下的贴近最大值分配
    while (CrtDemandPos<=T){
        for(int i=1;i<=n2&&TimeTotalDemand[CrtDemandPos].flow>0;++i){
            if(NintyFive[i] <= TimeTotalDemand[CrtDemandPos].flow && NintyFive[i]<=edge[i].flow){
                EdgeTemporyAssign[i]=NintyFive[i];
                TimeTotalDemand[CrtDemandPos].flow-=NintyFive[i];
            }else if(NintyFive[i] > TimeTotalDemand[CrtDemandPos].flow){
                EdgeTemporyAssign[i]=TimeTotalDemand[CrtDemandPos].flow;
                TimeTotalDemand[CrtDemandPos].flow=0;
            }else{
                EdgeTemporyAssign[i]=edge[i].flow;
                TimeTotalDemand[CrtDemandPos].flow-=edge[i].flow;
            }
        }
        Assign(n2);
        CrtDemandPos++;
    }*/


    while (CrtDemandPos <= T) {
        GenerateAvgAssign(n2);
        int flag=0;//淘汰数
        //检查是否有超出流量
        for (int i = 1; i <= n2-flag; ++i) {
            if(EdgeTemporyAssign[i]>=edge[i].flow){
                //交换并淘汰
                Edge tmp = Edge{edge[n2-flag].name,edge[n2-flag].flow,edge[n2-flag].EdgeType,edge[n2-flag].EdgeCostFlag};
                int tmpAssign=EdgeTemporyAssign[n2];

                edge[n2-flag].name=edge[i].name;
                edge[n2-flag].flow=edge[i].flow;
                edge[n2-flag].EdgeType=edge[i].EdgeType;
                edge[n2-flag].EdgeCostFlag=edge[i].EdgeCostFlag;

                edge[i].name=tmp.name;
                edge[i].flow=tmp.flow;
                edge[i].EdgeType=tmp.EdgeType;
                edge[i].EdgeCostFlag=tmp.EdgeCostFlag;
                EdgeTemporyAssign[i]=tmpAssign;

                flag++;
            }
        }

        //不超流量
        cout<<"type2 " <<"crt:"<<CrtDemandPos<<endl;
        for(auto a:EdgeTemporyAssign){
            cout<<a<<" ";
        }
        cout<<endl;

        Assign(n2);
        n2-=flag;
        CrtDemandPos++;

        /*if (flag) {
            //超过了流量,修正调配方案
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
            cout<<"type1 " <<"crt:"<<CrtDemandPos<<endl;
            for(auto a:EdgeTemporyAssign){
                cout<<a<<" ";
            }
            PrintEdge();
            cout<<endl;
            Assign(n2);
            n2--; //最后一个节点归零,前移
            continue;
        }else{
            //不超流量
            cout<<"type2 " <<"crt:"<<CrtDemandPos<<endl;
            for(auto a:EdgeTemporyAssign){
                cout<<a<<" ";
            }
            cout<<endl;
            Assign(n2);
            CrtDemandPos++;
        }*/
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
    plan.resize(T+1);
    for (int i = 1; i < plan.size(); ++i) {
        plan[i].resize(m+1);
    }
}

void init()
{
    //读入客户节点
    for (int i = 1; i < data.demand_.size(); ++i) {
        int res=0;
        for (int j = 1; j < data.demand_[i].size(); ++j) {
            res+=data.demand_[i][j];
            //cout<<data.demand_[i][j]<<" ";
        }
        TimeTotalDemand[i].name=i;
        TimeTotalDemand[i].flow=res;
        //cout<<"i:"<<i<<" "<<res<<endl;
    }

    //边缘节点流量
    for (int i = 1; i < data.band_width_.size(); ++i) {
        //cout<<"i:"<<i<<" "<<data.band_width_[i]<<endl;
        edge[i].name=i;
        edge[i].flow=data.band_width_[i];
    }

    T=data.demand_.size()-2;
    //n=data.band_width_.size()-1;
    m=data.demand_[1].size()-1;

    NoCost = floor(T * 0.05); //空位数
    sort(TimeTotalDemand + 1, TimeTotalDemand + T + 1, DemandCmp);
    memset(EdgeTemporyAssign, 0, sizeof(EdgeTemporyAssign));
}
int main()
{
    getData();
    init();
    genNodeNameList();
    step1();
    step2();
    PrintEdge();

    /*
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
    }*/

    //去掉这个,将plan写入即可
    for (int i = 1; i < plan.size(); ++i) {
        cout<<"t:"<<i;
        for (int j = 0; j < plan[i].size(); ++j) {
            for(auto p:plan[i][j]){
                cout<<" j:"<<j<<"<"<<p.first<<","<<p.second<<">";
            }
            cout<<endl;
        }
    }

    //WriteData("./output/solution.txt", customer_node, edge_node, std::move(testv));
    return 0;
}
