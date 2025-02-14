#include "Statistic.h"

using namespace std;

void Statistic::processWords(const list<string>& words) {
    for (const string& word : words) {
        wordFrequency[word]++;
        totalWords++;
    }
}

const map<string, int>& Statistic::getWordFrequency() const {
    return wordFrequency;
}

int Statistic::getTotalWords() const {
    return totalWords;
}
