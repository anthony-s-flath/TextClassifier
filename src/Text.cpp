#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <algorithm>
#include <ctime>

#include "Text.h"


Text::Text(string path)
    : file(path), totalWords(0) {
    read_whole_file(file);
    wordCount.reserve(counts.size());

    for (auto &it : counts)
        wordCount.emplace_back(it.first, it.second);
}


void Text::read_whole_file(string path) {
    ifstream is(path);
    if (!is.is_open()) {
        cout << "file \"" << path << "\" not found" << endl;
        return;
    }
    is.seekg(0, ios_base::end);
    size_t size = is.tellg();
    is.seekg(0, ios_base::beg);
    char* line;
    line = minimize_allocation(size); // doesn't matter what size allocated
    is.read(line, size);
    line[size] = '\0';
    

    size_t e = 0;
    size_t b = 0;
    for (char* itr = line; *itr != '\0'; ++itr, ++e) {
        if (*itr < 33 || *itr > 126) { // readable character, not whitespace
            *itr = '\0';
            add_word(line + b, e - b);
            b = e + 1;
        }
    }

    delete []line;
    is.close();
}

void Text::add_word(char* s, size_t length)  {
    if (!s || length == 0)
        return;
    
    // lowercase
    char* temp = s;
    while (*temp != '\0') {
        if ((*temp > 64 && *temp < 91))
            *temp += 32;
        ++temp;
    }
    
    // cut beginning
    while (*s != '\0' && (*s < 97 || *s > 122)) {
        ++s;
        --length;
    }
    
    if (length == 0)
        return;
    
    // cut ending
    temp = s + length - 1; // last char of string
    while (s != temp && (*temp < 97 || *temp > 122)) {
        --temp;
    }
    *(temp + 1) = '\0';

    if (*s != '\0') {
        ++totalWords;
        ++counts[s];
    }
}

char* Text::minimize_allocation(size_t length) const {
    int size = MAX_FILE_BIT;
    // find the most significant bit
    while (!(MAX_FILE_COMP & length)) {
        --size;
        length = length << 1;
    }
    size = 2 << size;
    return new char[size];
}
