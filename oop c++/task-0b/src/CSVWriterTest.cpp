#include <gtest/gtest.h>
#include "CSVWriter.h"
#include <fstream>
#include <vector>
#include <string>

using namespace std;

TEST(CSVWriter, WriteData) {
string outputFilename = "test_output.csv";

vector<vector<string>> data = {
        {"header1", "header2"},
        {"value1", "value2"},
        {"value3", "value4"}
};

CSVWriter writer(outputFilename);
writer.write(data);

ifstream file(outputFilename);
ASSERT_TRUE(file.is_open());

vector<string> expectedLines = {
        "header1,header2",
        "value1,value2",
        "value3,value4"
};

string line;
for (const auto& expectedLine : expectedLines) {
getline(file, line);
EXPECT_EQ(line, expectedLine);
}

file.close();
}
