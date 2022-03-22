#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>
using std::stoi;
using std::string;
using std::unordered_map;
using std::vector;

struct Data {
    int day_, customer_node_count_, edge_node_count_;
    unordered_map<string, int> edge_node_, customer_node_;
    vector<vector<int>> qos_;
    vector<vector<int>> demand_;
    vector<int> band_width_;

    Data(Data&&) = default;
    Data(const Data&) = delete;
    Data() = default;

    Data& operator=(Data&&) = default;
};

Data readData(vector<const char*> paths);
