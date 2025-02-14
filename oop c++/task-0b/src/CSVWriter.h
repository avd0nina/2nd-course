#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <vector>
#include <string>

using namespace std;

class CSVWriter {
public:
    CSVWriter(const string& filename);
    void write(const vector<vector<string>>& data);

private:
    string filename;
};

#endif // CSVWRITER_H
