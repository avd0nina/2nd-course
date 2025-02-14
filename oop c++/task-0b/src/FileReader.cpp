#include "FileReader.h"
#include <iostream>

FileReader::FileReader(const string& filename) {
    inputFile.open(filename);
    if (!inputFile.is_open()) {
        cerr << "Error: Не удалось открыть файл " << filename << endl;
    }
}

FileReader::~FileReader() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

bool FileReader::hasNext() {
    return inputFile.is_open() && !inputFile.eof();
}

string FileReader::next() {
    string line;
    if (hasNext()) {
        getline(inputFile, line);
    }
    return line;
}
