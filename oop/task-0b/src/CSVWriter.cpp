#include "CSVWriter.h"
#include <fstream>
#include <iostream>
#include <iomanip>

CSVWriter::CSVWriter(const string& filename) : filename(filename) {}

void CSVWriter::write(const vector<vector<string>>& data) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot open output file.\n";
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            outputFile << row[i];
            if (i != row.size() - 1) {
                outputFile << ",";
            }
        }
        outputFile << "\n";
    }

    outputFile.close();
    cout << "Файл " << filename << " успешно создан.\n";
}
