#include <iostream>
#include <chrono>
#include "DataNavigator.h"
#include "Classifier.h"

using std::cout, std::endl;

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: %.exe <training_dir> <testing_dir> <developement_dir>" << endl;
        return -1;
    }
    
    string train_dir(argv[1]);
    string test_dir(argv[2]);
    string dev_dir(argv[3]);

    if (train_dir.empty()) {
        cout << "No training data directory specified." << endl;
        return -1;
    }
    if (test_dir.empty()) {
        cout << "No testing data directory specified." << endl;
        return -1;
    }
    if (dev_dir.empty()) {
        cout << "No development data directory specified." << endl;
        return -1;
    }

    DataNavigator train(train_dir);
    DataNavigator test(test_dir);
    DataNavigator dev(dev_dir);
    
    auto t1 = chrono::high_resolution_clock::now();
    Classifier classifier(train, test, dev);
    classifier.run();
    auto t2 = chrono::high_resolution_clock::now();
    cout << "it took: " << chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << endl;

    return 0;
}