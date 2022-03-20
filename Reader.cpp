#include <cstdio>
#include <string>
#include <vector>

std::vector<std::string> readerLine(std::string&& s)
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
    head = readerLine(std::string(buf));
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(std::string(buf));
    }
    return std::make_pair(head, data);
}
std::vector<std::vector<std::string>> readBandWidth(const char* path)
{
    std::vector<std::vector<std::string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    fgets(buf, BUFFER_SIZE, f);
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(std::string(buf));
    }
    return data;
}
std::pair<
    std::vector<std::string>,
    std::vector<std::vector<std::string>>>
readQos(const char* path)
{
    std::vector<std::string> head;
    std::vector<std::vector<std::string>> data;
    FILE* f = fopen(path, "r");
    if (!f) {
        exit(-1);
    }
    fgets(buf, BUFFER_SIZE, f);
    head = readerLine(std::string(buf));
    while (!feof(f)) {
        fgets(buf, BUFFER_SIZE, f);
        data.emplace_back(std::string(buf));
    }
    return std::make_pair(head, data);
}
