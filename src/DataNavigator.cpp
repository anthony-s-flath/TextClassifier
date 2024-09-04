#include <filesystem>
#include "DataNavigator.h"


DataNavigator::DataNavigator(string path)
    : dir(path) {
    for (auto &f : filesystem::recursive_directory_iterator(path)) {
        if (!f.is_directory())
            cut(f.path());
    }
}

string DataNavigator::printable(const Label& label) {
    string to_print = "";
    for (size_t i = 0; i < label.size(); ++i) {
        to_print += label[i] + ".";
    }
    return to_print.substr(0, to_print.size() - 1);
}

void DataNavigator::cut(string file) {
    Label label;

    // full label
    size_t l_begin = dir.size() + 1;
    size_t l_end = file.find_last_of('/');
    
    size_t e;
    size_t b = l_begin;
    while ((e = file.find('.', b + 1)) < l_end) {
        label.push_back(file.substr(b, e - b));
        b = e + 1;
    }
    label.push_back(file.substr(b, l_end - b));

    texts.emplace(file, file);
    labels.insert(label);
    pmap.push(label, file);
    return;
}
