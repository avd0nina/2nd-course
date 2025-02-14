#include <gtest/gtest.h>
#include "Statistic.h"
#include <map>
#include <list>
#include <string>

using namespace std;

TEST(Statistic, ProcessWords) {
Statistic statistic;
list<string> words = {"apple", "banana", "apple", "banana", "cherry"};
statistic.processWords(words);

map<string, int> expected = {{"apple", 2}, {"banana", 2}, {"cherry", 1}};
EXPECT_EQ(statistic.getWordFrequency(), expected);
EXPECT_EQ(statistic.getTotalWords(), 5);
}

TEST(Statistic, EmptyWordList) {
Statistic statistic;
list<string> words = {};
statistic.processWords(words);

EXPECT_TRUE(statistic.getWordFrequency().empty());
EXPECT_EQ(statistic.getTotalWords(), 0);
}
