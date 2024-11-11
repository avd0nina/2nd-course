#include "FileReader.h"
#include "Parser.h"
#include "Statistic.h"
#include "CSVWriter.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int arguments_number, char* arguments[]) {
    if (arguments_number != 3) {
        cerr << "Usage: " << arguments[0] << " input.txt output.csv\n";
        return 1;
    }

    FileReader fileReader(arguments[1]);
    if (!fileReader.hasNext()) {
        cerr << "Error: Cannot open input file or file is empty.\n";
        return 1;
    }

    Parser parser;
    Statistic statistic;

    while (fileReader.hasNext()) {
        string line = fileReader.next();
        if (!line.empty()) {
            list<string> words = parser.splitIntoWords(line);
            statistic.processWords(words);
        }
    }

    vector<vector<string>> csvData;
    csvData.push_back({"Слово", "Частота", "Частота (%)"});

    const map<string, int>& wordFrequency = statistic.getWordFrequency();
    int totalWords = statistic.getTotalWords();

    for (const auto& entry : wordFrequency) {
        vector<string> row;
        row.push_back(entry.first);
        row.push_back(to_string(entry.second));
        row.push_back(to_string(static_cast<double>(entry.second) / totalWords * 100));
        csvData.push_back(row);
    }

    CSVWriter csvWriter(arguments[2]);
    csvWriter.write(csvData);

    return 0;
}
