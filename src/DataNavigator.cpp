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
    string toPrint = "";
    for (size_t i = 0; i < label.size(); ++i) {
        toPrint += label[i] + ".";
    }
    return toPrint.substr(0, toPrint.size() - 1);
}

void DataNavigator::cut(string file) {
    Label label;

    // full label
    size_t labelBegin = dir.size() + 1;
    size_t labelEnd = file.find_last_of('/');
    
    size_t e;
    size_t b = labelBegin;
    while ((e = file.find('.', b + 1)) < labelEnd) {
        label.push_back(file.substr(b, e - b));
        b = e + 1;
    }
    label.push_back(file.substr(b, labelEnd - b));

    texts.emplace(file, file);
    labels.insert(label);
    pmap.push(label, file);
    return;
}
