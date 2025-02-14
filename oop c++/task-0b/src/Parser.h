#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <list>

using namespace std;

class Parser {
public:
    list<string> splitIntoWords(const string& line);
};

#endif // PARSER_H
