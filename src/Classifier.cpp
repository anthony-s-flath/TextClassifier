#include <cmath>
#include <iostream>
#include "Classifier.h"

using namespace std;

Classifier::Classifier(DataNavigator* trainNav, DataNavigator* testNav, DataNavigator* devNav)
    : train(trainNav), test(testNav), dev(devNav) {

    // label amount and percents
    size_t fullAmount = train->pmap.size();
    for (auto itr = train->labels.begin(); itr != train->labels.end(); ++itr) {
        Label label = *itr;
        size_t labelAmount = train->pmap.size(label);
        trainSize[label] = labelAmount;
        trainLog2Percent[label] = double(labelAmount) / fullAmount;
    }

    set_alpha();
}

void Classifier::run() {
    // testing
    int numCorrect = 0;
    for (auto testItr = test->pmap.begin(); testItr != test->pmap.end(); ++testItr) {
        Label expected = testItr.get_key();
        Label actual = classify(test->texts[*testItr]);
        if (actual == expected)
            ++numCorrect;

        cout << "Expected: " <<  DataNavigator::printable(expected) << endl;
        cout << "Actual: " << DataNavigator::printable(actual) << endl << endl;
    }

    cout << "Train dir: " << train->get_dir() << endl;
    cout << "Train size: " << train->texts.size() << endl;
    cout << "Test dir: " << test->get_dir() << endl;
    cout << "Test size: " << test->texts.size() << endl;
    cout << "Dev dir: " << dev->get_dir() << endl;
    cout << "Dev size: " << dev->texts.size() << endl;
    cout << "Hyperparameter alpha: " << alpha << endl;
    cout << "Total labeled: " << test->texts.size() << endl;
    cout << "Total correctly labeled: " << numCorrect << endl;
    cout << "Proportion correctly labeled: " << numCorrect * 1.0 / test->texts.size() << endl;
}

Label Classifier::classify(const Text &text) {
    long double likelihood = 0;
    Label estimatedLabel;
 
    // multinomial_coefficient(text);
    // unecessary computation to find multinomial coefficient
    long double multiCoef = 0;
    // Equation 2.14
    for (auto itr = train->labels.begin(); itr != train->labels.end(); ++itr) {
        Label label = *itr;
        long double likelyTemp = label_likelihood(multiCoef, label, text);

        if (likelyTemp > likelihood || likelihood == 0) {
            likelihood = likelyTemp;
            estimatedLabel = label;
        }
    }

   return estimatedLabel;
}


long double Classifier::label_likelihood(long double mc, const Label& trainLabel, const Text& text) {
    long double total = mc; // log2(B(x))

    // Equation 2.17
    total += trainLog2Percent.at(trainLabel); // log2(u_y)
    for (size_t i = 0; i < text.size(); ++i) {
        int count = text.get_count(i).second;
        if (count) {
            total += count * log2(phi(trainLabel, text.get_count(i).first.c_str()));
        }
    }

   return total;
}


long double Classifier::phi(const Label &label, const char* word) {
    long double j = 0;
    long double all = 0;

    // Equation 2.21
    auto end = train->pmap.end(label);
    for (auto itr = train->pmap.begin(label); itr != end; ++itr) {
        string path = *itr;
        j += train->texts[path][word];
        all += train->texts[path].total();
    }

    // smoothing
    j += alpha;
    all += alpha * trainSize.at(label); // pmap.size() -> map

   return j / all;
}


long double Classifier::multinomial_coefficient(const Text &text) {
    long double numerator = 0;
    long double denominator = 1;

    // sigma
    for (size_t i = 0; i < text.size(); ++i) {
        numerator += text.get_count(i).second;
    }
    // factorial
    for (double i = numerator; i > 1; --i) {
        numerator *= i - 1;
    }
    
    for (size_t i = 0; i < text.size(); ++i) {
        long double temp = text.get_count(i).second;
        for (double i = temp; i > 1; --i) {
            temp *= i - 1;
        }
        denominator *= temp;
    }

    return numerator / denominator;
}


void Classifier::set_alpha() {
    size_t maxIndex = 0;
    size_t maxCorrect = 0;
    for (size_t i = 0; i < ALPHAS.size(); ++i) {
        int num = 0;
        alpha = ALPHAS[i];
        for (auto devItr = dev->pmap.begin(); devItr != dev->pmap.end(); ++devItr) {
            Label result = classify(dev->texts[*devItr]);
            if (result == devItr.get_key())
                ++num;
        }

        if (maxCorrect < num) {
            maxCorrect = num;
            maxIndex = i;
        }
    }
    alpha = ALPHAS[maxIndex];
}
