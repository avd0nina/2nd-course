#include <gtest/gtest.h>
#include "Parser.h"
#include <list>
#include <string>

using namespace std;

TEST(Parser, SplitIntoWords_Basic) {
Parser parser;
string line = "Hello, world!";
list<string> words = parser.splitIntoWords(line);

list<string> expected = {"hello", "world"};
EXPECT_EQ(words, expected);
}

TEST(Parser, SplitIntoWords_Empty) {
Parser parser;
string line = "";
list<string> words = parser.splitIntoWords(line);

EXPECT_TRUE(words.empty());
}

TEST(Parser, SplitIntoWords_SpecialChars) {
Parser parser;
string line = "C++ is great!";
list<string> words = parser.splitIntoWords(line);

list<string> expected = {"c", "is", "great"};
EXPECT_EQ(words, expected);
}


