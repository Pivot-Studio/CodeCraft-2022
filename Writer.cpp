#include "Writer.hpp"

void WriteData(
    const char* path,
    vector<string>& customer_node,
    vector<string>& edge_node,
    vector<vector<vector<pair<int, int>>>> plan)
{
    FILE* f = fopen(path, "w");
    if (!f) {
        exit(-1);
    }
    for (int i = 1; i < plan.size(); ++i) {
        for (int j = 1; j < plan[i].size(); ++i) {
            fprintf(f, "%s", customer_node[j]);
            for (int k = 0; k < plan[i][j].size(); ++k) {
                fprintf(f, "%c<%s,%d>", j == 0 ? ":" : ",", edge_node[plan[i][j][k].first], plan[i][j][k].second);
            }
            fputc('\n', f);
        }
    }
    fclose(f);
}