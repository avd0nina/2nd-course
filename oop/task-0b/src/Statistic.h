#ifndef STATISTIC_H
#define STATISTIC_H

#include <map>
#include <string>
#include <list>

using namespace std;

class Statistic {
public:
    void processWords(const list<string>& words);
    const map<string, int>& getWordFrequency() const;
    int getTotalWords() const;

private:
    map<string, int> wordFrequency;
    int totalWords = 0;
};

#endif // STATISTIC_H
