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
    
    string trainDir(argv[1]);
    string testDir(argv[2]);
    string devDir(argv[3]);

    if (trainDir.empty()) {
        cout << "No training data directory specified." << endl;
        return -1;
    }
    if (testDir.empty()) {
        cout << "No testing data directory specified." << endl;
        return -1;
    }
    if (devDir.empty()) {
        cout << "No development data directory specified." << endl;
        return -1;
    }

    DataNavigator train(trainDir);
    DataNavigator test(testDir);
    DataNavigator dev(devDir);
    
    auto t1 = chrono::high_resolution_clock::now();
    Classifier classifier(&train, &test, &dev);
    classifier.run();
    auto t2 = chrono::high_resolution_clock::now();
    cout << "it took: " << chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
        << "ms" << endl;

    return 0;
}