#include "view.h"
#include <unistd.h>
namespace {
    const std::string helpMessage =
    " dump <filename> - сохранить вселенную в файл                           \n"
    " tick <n=1> (и сокращенно t <n=1>) - рассчитать n (по умолчанию 1)      \n"
    " exit - завершить игру                                                  \n"
    " help - распечатать справку о командах                                  \n";
}

void Game_view::printHelp() {
    std::cout << helpMessage << std::endl;
}

void Game_view::printErr(const std::string& err_message) {
    std::cout << err_message << std::endl;
}

void Game_view::printFrame(
        const std::string& name,
        const Field& field,
        int glob_iteration,
        int curr_iteration
) {
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    std::cout << "Name: " + name + ";" << std::endl;
    std::cout << "Total iteration: " << glob_iteration << ";" << std::endl;
    std::cout << "Curr iteration: " << curr_iteration << ";" << std::endl;
    for (int i = -1; i <= field.getSize(); ++i) {
        for (int j = -1; j <= field.getSize(); ++j) {
            if (field.getState(i, j)) {
                std::cout << "* ";
            } else {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }
    //0.5 сек
    usleep(500000);
}