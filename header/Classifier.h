/*

    Classifies texts by labeleling

*/


#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "DataNavigator.h"
#include "Text.h"


class Classifier {
private:
    const vector<double> ALPHAS{1, 0.1, 0.01, 0.001};
    DataNavigator* train;
    DataNavigator* test;
    DataNavigator* dev;
    unordered_map<Label, size_t, LabelHash, LabelEqual> trainSize;
    unordered_map<Label, double, LabelHash, LabelEqual> trainLog2Percent;
    double alpha;

public:
    Classifier(DataNavigator* trainNav, DataNavigator* testNav, DataNavigator* devNav);

    // irerates over tesing files to classify
    void run();

private:
    // iterates over all training labels to see if they fit
    Label classify(const Text &text);

    // iterates over text word counts
    long double label_likelihood(long double mc, const Label& trainLabel, const Text& text);
    
    // iterates over instances under label
    long double phi(const Label &label, const char* word);

    long double multinomial_coefficient(const Text &text);

    void set_alpha();
};

#endif