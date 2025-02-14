#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <fstream>

using namespace std;

class FileReader {
public:
    FileReader(const string& filename);
    ~FileReader();
    string next();
    bool hasNext();

private:
    ifstream inputFile;
};

#endif // FILEREADER_H
