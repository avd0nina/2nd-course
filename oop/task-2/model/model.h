#pragma once
#include <string>
#include <vector>

struct Point {
    int x;
    int y;
};

const std::string Default_name = "Conway's game of life";
const std::string Default_b_rule = "3";
const std::string Default_s_rule = "23";

const std::vector<Point> Default_frame = {
        {0, 0}, {1, 1}, {2, 1}, {2, 3}, {2, 4},
        {3, 2}, {4, 9}, {5, 4}, {5, 8}, {5, 9}, {6, 6},
};

class Field {
private:
    int size_ = 20;
    std::vector<unsigned char> field_;
public:
    Field();
    Field(std::vector<Point> coordinates);
    int getSize() const;
    bool getState(int row, int column) const;
    void setState(int row, int column, bool value);
};

class Game_model {
private:
    int global_iteration_ = 0;
    std::string name_;
    const std::vector<bool> b_rule_;
    const std::vector<bool> s_rule_;
    char current_field_ = 1;
    Field field_1_;
    Field field_2_;
public:
    Game_model();
    Game_model(
        const std::string& name,
        const std::string& brule,
        const std::string& srule,
        const std::vector<Point>& coords
    );
    int getGlobIteration () const;
    void computeIteration();
    const std::string& getName() const;
    const Field& getField() const;
};

