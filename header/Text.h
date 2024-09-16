/*

    Processes text for data

*/

#ifndef TEXT_H
#define TEXT_H

#include <vector>
#include <unordered_map>
#include <string_view>
using namespace std;

using Count = pair<string, int>;

struct Quote {
    Quote(bool f, size_t s, size_t e)
        : finished(f), start(s), end(e) {   }
    bool finished;
    size_t start;
    size_t end;
};


struct CountComp {
    bool operator()(const Count &lhs, const Count &rhs) const {
        return lhs.second > rhs.second;
    }
};

struct AlphaComp {
    bool operator()(const Count &lhs, const Count &rhs) const {
        return lhs.first.compare(rhs.first) > 0;
    }
};

class Text {
private:
    static const size_t WORD_MAX = UINT8_MAX;
    static const size_t MAX_FILE_BIT = 17;
    static const size_t MAX_FILE_COMP = 1 << MAX_FILE_BIT;

    const string file;
    int totalWords;
    unordered_map<string, int> counts;
    vector<Count> wordCount;



public:
    Text()
        : file(""), totalWords(0) {       }
    Text(string path);

    const Count& get_count(size_t i) const
        {   return wordCount[i];   }
    int operator[](const char* str)   
        {   return counts[str];   }
        
    int total() const 
        {   return totalWords; }
    size_t size() const
        { return wordCount.size(); }

private:
    void read_whole_file(string path);
    
    // length is number of chars in string
    void add_word(char* s, size_t length);

    char* minimize_allocation(size_t length) const;
};

#endif