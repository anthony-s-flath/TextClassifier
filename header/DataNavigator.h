/*

    Navigates through data file directory

*/


#ifndef DATANAVIGATOR_H
#define DATANAVIGATOR_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Text.h"
#include "PriorityMap.h"

using namespace std;
using Label = vector<string>;


struct LabelEqual {
    bool operator()(const Label &lhs, const Label &rhs) const {
        if (lhs.size() != rhs.size())
            return false;
        
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }
};

struct LabelHash {
    size_t operator()(const Label &l) const {
        string total = "";
        for (const auto& sub : l)
            total += sub;

        return hash<string>{}(total);
    }
};


struct DataNavigator {
public:
    unordered_set<Label, LabelHash, LabelEqual> labels;
    // path, text
    unordered_map<string, Text> texts;
    // label, file name
    PriorityMap<string, string> pmap;


private:
    const string dir;


public:
    DataNavigator(string path);

    static string printable(const Label& label);
    
    string get_dir() const
        { return dir; }


private:
    // recieves file path, pushes into pmap
    void cut(string file);
};

#endif