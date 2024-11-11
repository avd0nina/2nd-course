#include "Parser.h"
#include <cctype>

using namespace std;

list<string> Parser::splitIntoWords(const string& line) {
    list<string> words;
    string word;
    for (char ch : line) {
        if (isalnum(ch)) {
            word += tolower(ch);
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}
