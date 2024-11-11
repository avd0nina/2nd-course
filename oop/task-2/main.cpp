#include <iostream>
#include <vector>
#include <string>
#include "controller/controller.h"
#include "model/model.h"
#include "view/view.h"
#include <gtest/gtest.h>

std::vector<std::string> EditArgs(int argc, char** argv) {
    std::vector<std::string> args(argc);
    for (int i = 0; i < argc; ++i) {
        args[i] = argv[i];
    }
    return args;
}

int main(int argc, char** argv) {
    std::unique_ptr<SimulationController> life;
    std::vector<std::string> args = EditArgs(argc, argv);
    try {
        if (argc == 1) {
            life = std::make_unique<SimulationController>();
        } else if (argc == 2) {
            life = std::make_unique<SimulationController>(args[1]);
        } else if (argc == 6) {
            int i_num = 1, o_num = 1;
            for (int i = 0; i < argc; ++i) {
                if (args[i] == "-o") {
                    o_num += i;
                } else if (args[i] == "-i") {
                    i_num += i;
                }
            }
            if (i_num > 2 && o_num > 2) {
                life = std::make_unique<SimulationController>(args[1], args[i_num], args[o_num]);
            } else {
                std::cout << "Wrong keys";
            }
        } else {
            std::cout << "Wrong keys";
            return 1;
        }
    } catch (std::invalid_argument) {
        return 1;
    }

    life->play_game();
    return 0;
}

//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
//
//
//TEST(FieldTest, CustomConstructor) {
//    std::vector<Point> coords = {{0, 0}, {1, 1}, {2, 2}};
//    Field field(coords);
//    EXPECT_EQ(field.getState(0, 0), true);
//    EXPECT_EQ(field.getState(1, 1), true);
//    EXPECT_EQ(field.getState(2, 2), true);
//}
//
//
//TEST(GameModelTest, GlobalIterationCount) {
//    Game_model model("GlobalIterationModel", "345", "26", {{0, 0}, {1, 1}});
//    model.computeIteration();
//    EXPECT_EQ(model.getGlobIteration(), 1);
//    model.computeIteration();
//    EXPECT_EQ(model.getGlobIteration(), 2);
//}
//
//
//TEST(GameViewTest, PrintHelp) {
//    Game_view view;
//    testing::internal::CaptureStdout();
//    view.printHelp();
//    std::string output = testing::internal::GetCapturedStdout();
//    EXPECT_EQ(output, " dump <filename> - сохранить вселенную в файл                           \n"
//                      " tick <n=1> (и сокращенно t <n=1>) - рассчитать n (по умолчанию 1)      \n"
//                      " exit - завершить игру                                                  \n"
//                      " help - распечатать справку о командах                                  \n\n");
//}
//
//
//TEST(GameViewTest, PrintErr) {
//    Game_view view;
//    testing::internal::CaptureStdout();
//    view.printErr("Test Error Message");
//    std::string output = testing::internal::GetCapturedStdout();
//    EXPECT_EQ(output, "Test Error Message\n");
//}
