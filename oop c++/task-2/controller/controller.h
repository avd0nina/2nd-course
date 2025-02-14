#pragma once
#include "../model/model.h"
#include "../view/view.h"
#include <memory>

class SimulationController{
private:
    std::unique_ptr<Game_model> model_;
    Game_view view_;
    std::string out_file;

public:
    SimulationController();
    SimulationController(const std::string& infile);
    SimulationController(
        const std::string& infile,
        const std::string& ticks,
        const std::string& outfile
    );

    void play_game();
};