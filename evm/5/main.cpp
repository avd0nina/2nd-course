#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char* argv[]) {
    VideoCapture cam(0);
    if (!cam. isOpened()) {
        std:: cerr << "Camera isn't open. \n";
        return -1;
    }
    int counter = 0;
    std::time_t start, read, process, end;
    double total_time = 0;
    double total_read_time = 0;
    double total_process_time = 0;
    double total_show_time = 0;
    while (true) {
        start = std::clock();
        Mat image;
        if (!cam.read(image)) {
            std:: cerr << "Camera is off. \n";
            return -1;
        }
        read = std::clock();
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                int blue = image.at<Vec3b>(i, j)[0];
                int green = image.at<Vec3b>(i, j)[1];
                int red = image.at<Vec3b>(i, j)[2];
                if (green < 100) {
                    image.at<Vec3b>(i, j)[1] = std::min(255, green * 10);
                }
            }
        }
        process = std::clock();
        imshow("Camera", image);
        char key = waitKey(33);
        if (key == 27) {
            break;
        }
        end = std::clock();
        total_time += end - start;
        total_read_time += read - start;
        total_process_time += process - read;
        total_show_time += end - process;
        counter++;
    }
    destroyAllWindows();
    std::cout << "FPS: " << counter / (total_time / CLOCKS_PER_SEC) << "\n";
    std::cout << "read: " << total_read_time / total_time * 100 << "%\n";
    std::cout << "process: " << total_process_time / total_time * 100 << "%\n";
    std::cout << "show: " << total_show_time / total_time * 100 << "%\n";
}
