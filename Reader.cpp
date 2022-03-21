#include "Reader.hpp"

std::vector<std::string> readLine(std::string&& s)
{
    std::vector<std::string> v;
    std::string tmps;
    for (auto c : s) {
        if (c == ',' || c == '\r' || c == '\n') {
            if (tmps.size() > 0) {
                v.emplace_back(std::move(tmps));
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
std::pair<
    std::vector<std::string>,
    std::vector<std::vector<std::string>>>
readDemand(const char* path)
{
    std::vector<std::string> head;
    std::vector<std::vector<std::string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    fgets(buf, BUFFER_SIZE, f);
    head = readLine(std::string(buf));
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(readLine(std::string(buf)));
    }
    return std::make_pair(head, data);
}
std::pair<std::vector<std::string>, std::vector<std::string>> readBandWidth(const char* path)
{
    std::vector<std::vector<std::string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    fgets(buf, BUFFER_SIZE, f);
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(readLine(std::string(buf)));
    }
    std::vector<std::string> v1, v2;
    for (auto& v : data) {
        v1.push_back(v[0]);
        v2.push_back(v[1]);
    }
    return v1, v2;
}
std::vector<std::vector<std::string>> readQos(const char* path)
{
    std::vector<std::vector<std::string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    fgets(buf, BUFFER_SIZE, f);
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(readLine(std::string(buf)));
    }
    return std::make_pair(head, data);
}
std::unordered_map<std::string, int> encodeNode(const std::vector<std::string>& v)
{
    int idx = 0;
    std::unordered_map<std::string, int> mp;
    for (auto& str : v) {
        mp[str] = idx++;
    }
    return mp;
}
std::vector<int> vectorTrans(std::vector<std::string> v)
{
    std::vector<int> numVec(v.size());
    for (auto& s : v) {
        numVec.push_back(std::stoi(s));
    }
    return numVec;
}
std::vector<std::vector<int>> twoDimVectorTrans(std::vector<std::vector<std::string>> v)
{
    std::vector<std::vector<int>> numVec(v.size());
    for (auto& vec : v) {
        numVec.push_back(vectorTrans(vec));
    }
    return numVec;
}
Data readData(std::vector<const char*> paths)
{
    Data data;
    auto p = readDemand(paths[0]);
    data.customer_node_ = encodeNode(p.first);
    data.need_ = twoDimVectorTrans(p.second);
    data.day_ = data.need_.size();
    auto p1 = readBandWidth(paths[1]);
    data.edge_node_ = encodeNode(p1.first);
    data.band_width_ = vectorTrans(p.second);
    data.qos_ = twoDimVectorTrans(readQos(paths[2]));
    return data;
}