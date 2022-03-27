#include "Writer.hpp"

void WriteData(
    const char* path,
    string* customer_node,
    string* edge_node,
    vector<vector<vector<pair<int, int>>>> plan)
{
    FILE* f = fopen(path, "w+");
    if (!f) {
        exit(-1);
    }
    for (int i = 1; i < plan.size(); ++i) {
        for (int j = 1; j < plan[i].size(); ++j) {
            fprintf(f, "%s", customer_node[j].c_str());
            for (int k = 0; k < plan[i][j].size(); ++k) {
                fprintf(f, "%s<%s,%d>", j == 0 ? ":" : ",", edge_node[plan[i][j][k].second].c_str(), plan[i][j][k].first);
            }
            fputc('\n', f);
        }
    }
    fclose(f);
}