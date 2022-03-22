#include "Reader.hpp"

vector<string> readLine(string&& s)
{
    vector<string> v;
    string tmps;
    for (auto c : s) {
        if (c == ',' || c == '\r' || c == '\n') {
            if (tmps.size() > 0) {
                v.emplace_back(move(tmps));
                tmps = "";
            }
        } else {
            tmps.push_back(c);
        }
    }
    return v;
}
const int BUFFER_SIZE = 512;
char buf[BUFFER_SIZE];
vector<vector<string>> readCSV(const char* path)
{
    vector<string> head;
    vector<vector<string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(readLine(string(buf)));
    }
    return data;
}
unordered_map<string, int> encodeNode(const vector<string>& v)
{
    int idx = 0;
    unordered_map<string, int> mp;
    for (auto& str : v) {
        mp[str] = idx++;
    }
    return mp;
}
Data readData(vector<const char*> paths)
{
    Data data;

    auto demand = readCSV(paths[0]);
    data.day_ = demand.size() - 1;
    data.customer_node_ = encodeNode(demand[0]);
    data.customer_node_.erase(demand[0][0]);
    data.customer_node_count_ = data.customer_node_.size();
    vector<vector<int>> v;
    v.resize(data.day_ + 1, vector<int>(data.customer_node_count_ + 1));
    for (int i = 1; i <= data.day_; ++i) {
        v[i].resize(data.customer_node_count_ + 1);
        for (int j = 1; j <= data.customer_node_count_; ++j) {
            v[i][j] = stoi(demand[i][j]);
        }
    }
    data.demand_ = std::move(v);

    auto bandwidth = readCSV(paths[1]);
    vector<string> edge_name(bandwidth.size()), bw(bandwidth.size());
    for (auto& v : bandwidth) {
        edge_name.push_back(v[0]);
        bw.push_back(v[1]);
    }
    data.edge_node_ = encodeNode(edge_name);
    data.edge_node_.erase(edge_name[0]);
    data.edge_node_count_ = data.edge_node_.size();
    vector<int> v1;
    v1.resize(data.edge_node_count_ + 1);
    for (int i = 1; i <= data.edge_node_count_; ++i) {
        v1[i] = stoi(bw[i]);
    }
    data.band_width_ = std::move(v1);

    auto qos = readCSV(paths[2]);
    v.resize(data.edge_node_count_ + 1);
    for (int i = 1; i <= data.edge_node_count_; ++i) {
        v[i].resize(data.customer_node_count_ + 1);
        for (int j = 1; j <= data.customer_node_count_; ++j) {
            v[i][j] = stoi(qos[i][j]);
        }
    }
    data.qos_ = std::move(v);
    return data;
}