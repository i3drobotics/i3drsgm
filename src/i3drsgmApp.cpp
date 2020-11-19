#include <i3drsgm.h>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <string>

int main(int argc, char *argv[]){
    //TODO: add command line options
    // default values
    std::string left_filepath = "./resources/left.png";
    std::string right_filepath = "./resources/right.png";
    std::string left_cal_filepath = "./resources/left.yaml"; //left.yaml or left.xml
    std::string right_cal_filepath = "./resources/right.yaml"; //right.yaml or right.xml
    std::string output_folder = "./output";
    bool preRectified = false;

    bool valid = I3DRSGM::forwardMatchFiles(left_filepath,right_filepath,left_cal_filepath,right_cal_filepath,output_folder,preRectified);
    if (valid){
        std::cout << "I3DRSGM match complete." << std::endl;
        return 0;
    } else {
        std::cout << "I3DRSGM match failed." << std::endl;
        return 1;
    }
}