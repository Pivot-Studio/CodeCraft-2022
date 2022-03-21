#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

struct Data {
    int day_;
    std::unordered_map<std::string, int> edge_node_, customer_node_;
    std::vector<std::vector<int>> qos_;
    std::vector<std::vector<int>> need_;
    std::vector<int> band_width_;

    Data(Data&&) = default;
    Data(const Data&) = delete;
    Data() = default;

    Data& operator=(Data&&) = default;
};

Data readData(std::vector<const char*> paths);
