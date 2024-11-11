#include <gtest/gtest.h>
#include <fstream>
#include "FileReader.h"

using namespace std;

TEST(FileReader, ReadLines) {
ofstream testFile("test.txt");
testFile << "line1\nline2\nline3";
testFile.close();

FileReader reader("test.txt");
vector<string> lines;

while (reader.hasNext()) {
lines.push_back(reader.next());
}

vector<string> expected = {"line1", "line2", "line3"};
EXPECT_EQ(lines, expected);
}

TEST(FileReader, FileNotFound) {
FileReader reader("nonexistent.txt");
EXPECT_FALSE(reader.hasNext());
}
