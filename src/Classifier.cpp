#include <cmath>
#include <iostream>
#include "Classifier.h"

using namespace std;

Classifier::Classifier(DataNavigator &train_nav, DataNavigator &test_nav, DataNavigator &dev_nav)
    : train(train_nav), test(test_nav), dev(dev_nav) {

    // label amount and percents
    size_t full_amount = train.pmap.size();
    for (auto itr = train.labels.begin(); itr != train.labels.end(); ++itr) {
        Label label = *itr;
        size_t label_amount = train.pmap.size(label);
        train_size[label] = label_amount;
        train_log2_percent[label] = double(label_amount) / full_amount;
    }

    alpha = 0.01;
    set_alpha();
}

void Classifier::run() {
    // testing
    int num_correct = 0;
    for (auto test_itr = test.pmap.begin(); test_itr != test.pmap.end(); ++test_itr) {
        Label expected = test_itr.get_key();
        Label actual = classify(test.texts[*test_itr]);
        if (actual == expected)
            ++num_correct;

        cout << "Expected: " <<  DataNavigator::printable(expected) << endl;
        cout << "Actual: " << DataNavigator::printable(actual) << endl << endl;
    }

    cout << "Train dir: " << train.get_dir() << endl;
    cout << "Train size: " << train.texts.size() << endl;
    cout << "Test dir: " << test.get_dir() << endl;
    cout << "Test size: " << test.texts.size() << endl;
    cout << "Dev dir: " << dev.get_dir() << endl;
    cout << "Dev size: " << dev.texts.size() << endl;
    cout << "Hyperparameter alpha: " << alpha << endl;
    cout << "Total labeled: " << test.texts.size() << endl;
    cout << "Total correctly labeled: " << num_correct << endl;
    cout << "Proportion correctly labeled: " << num_correct * 1.0 / test.texts.size() << endl;
}

Label Classifier::classify(const Text& text) {
    long double likelihood = 0;
    Label estimated_label;

    long double multi_coef = 0; //multinomial_coefficient(test_path);
    // Equation 2.14
    for (auto itr = train.labels.begin(); itr != train.labels.end(); ++itr) {
        Label label = *itr;
        long double likely_temp = label_likelihood(multi_coef, label, text);

        if (likely_temp > likelihood || likelihood == 0) {
            likelihood = likely_temp;
            estimated_label = label;
        }
    }

   return estimated_label;
}


long double Classifier::label_likelihood(long double mc, const Label& train_label, const Text& text) {
    long double total = mc; // log2(B(x))

    // Equation 2.17
    total += train_log2_percent.at(train_label); // log2(u_y)
    for (size_t i = 0; i < text.size(); ++i) {
        int count = text.get_count(i).second;
        if (count) {
            total += count * log2(phi(train_label, text.get_count(i).first.c_str()));
        }
    }

   return total;
}


long double Classifier::phi(const Label &label, const char* word) {
    long double j = 0;
    long double all = 0;

    // Equation 2.21
    auto end = train.pmap.end(label);
    for (auto itr = train.pmap.begin(label); itr != end; ++itr) {
        string path = *itr;
        j += train.texts[path][word];
        all += train.texts[path].total();
    }

    // smoothing
    j += alpha;
    all += alpha * train_size.at(label); // pmap.size() -> map

   return j / all;
}


long double Classifier::multinomial_coefficient(const string &test_path) {
    Text text(test_path);
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
    size_t max_index = 0;
    size_t max_correct = 0;
    for (size_t i = 0; i < ALPHAS.size(); ++i) {
        int num = 0;
        alpha = ALPHAS[i];
        for (auto dev_itr = dev.pmap.begin(); dev_itr != dev.pmap.end(); ++dev_itr) {
            Label result = classify(dev.texts[*dev_itr]);
            if (result == dev_itr.get_key())
                ++num;
        }

        if (max_correct < num) {
            max_correct = num;
            max_index = i;
        }
    }
    alpha = ALPHAS[max_index];
}
