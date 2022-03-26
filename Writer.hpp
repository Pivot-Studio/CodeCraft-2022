#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>
using std::pair;
using std::string;
using std::vector;

void WriteData(
    const char* path,
    const vector<string>& customer_node,
    const vector<string>& edge_node,
    vector<vector<vector<pair<int, int>>>> plan);