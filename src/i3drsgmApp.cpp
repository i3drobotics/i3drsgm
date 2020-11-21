#include <i3drsgm.h>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <string>
#include <sstream>

int main(int argc, char *argv[]){
    if (!I3DRSGM::isLicenseValid()){return 1;}

    // default values
    std::string left_filepath = "./resources/left.png";
    std::string right_filepath = "./resources/right.png";
    std::string left_cal_filepath = "./resources/left.yaml"; //left.yaml or left.xml
    std::string right_cal_filepath = "./resources/right.yaml"; //right.yaml or right.xml
    std::string output_folder = "./output";
    bool preRectified = false;
    bool loop = false;
    bool api = false;

    // command line options
    if( argc == 1 ) {
        std::cout << "Using default options" << std::endl;
    }
    else if ( argc == 2){
        if (std::string(argv[1]) == "api"){
            api = true;
        } else {
            std::cout << "Not enough arguments given. Expected format: I3DRSGMApp.exe left.png right.png left.yaml right.yaml output_folder [preRectified:1/0] [loop:1/0]" << std::endl;
            std::cout << argc << " arguments" << std::endl;
            return 1;
        }
    }
    else if( argc == 8) {
        left_filepath = std::string(argv[1]);
        right_filepath = std::string(argv[2]);
        left_cal_filepath = std::string(argv[3]);
        right_cal_filepath = std::string(argv[4]);
        output_folder = std::string(argv[5]);
        preRectified = std::string(argv[6]) == "1";
        loop = std::string(argv[7]) == "1";
    }
    else {
        std::cout << "Not enough arguments given. Expected format: I3DRSGMApp.exe left.png right.png left.yaml right.yaml output_folder [preRectified:1/0] [loop:1/0]" << std::endl;
        std::cout << argc << " arguments" << std::endl;
        return 1;
    }
    std::string tmp_param_filepath = I3DRSGM::getAppPath()+"/i3drsgm_tmp.param";
    std::string param_filepath = I3DRSGM::getAppPath()+"/i3drsgm.param";
    std::cout << "App path:" << I3DRSGM::getAppPath() << std::endl;

    I3DRSGM * i3drsgm;

    if (api){
        while(true){
            std::cout<< "API_READY" << std::endl;
            std::string api_input;
            std::getline (std::cin, api_input);
            std::cout << api_input << std::endl;

            std::string delimiter = ",";
            size_t pos = 0;
            std::string token;
            std::vector<std::string> input_list;
            while ((pos = api_input.find(delimiter)) != std::string::npos) {
                token = api_input.substr(0, pos);
                std::cout << token << std::endl;
                api_input.erase(0, pos + delimiter.length());
                input_list.push_back(token);
            }
            std::cout << api_input << std::endl;
            input_list.push_back(api_input);

            if (input_list.size() > 0){
                if (input_list[0] == "INIT"){
                    i3drsgm = new I3DRSGM(tmp_param_filepath,param_filepath);
                    int range = 1696/10;
                    if (range % 2 == 0)
                    {
                        range++;
                    }
                    i3drsgm->setDisparityRange((16 * range) / 10);
                    float shift = 348/20;
                    i3drsgm->setDisparityShift(shift);
                    int census_size = 11/2;
                    if (census_size % 2 == 0)
                    {
                        census_size++;
                    }
                    i3drsgm->setWindowSize(census_size);
                    i3drsgm->maxPyramid(6);
                    i3drsgm->enableOcclusionDetection(false);
                    i3drsgm->enableOccInterpol(false);
                    i3drsgm->enableInterpolation(true);
                    std::cout << "API_RESPONSE:init success" << std::endl;
                } else if (input_list[0] == "SET_DISPARITY_RANGE"){
                    if (input_list.size() == 2){
                        int val = std::stoi(input_list[1]);
                        val /= 10;
                        if (val % 2 == 0)
                        {
                            val++;
                        }
                        i3drsgm->setDisparityRange((16 * val) / 10);
                        bool valid = true;
                        if (valid){
                            std::cout << "API_RESPONSE:parameter set" << std::endl;
                        } else {
                            std::cout << "API_RESPONSE:ERROR,failed to set parameter" << std::endl;
                        }
                    } else {
                        std::cout << "API_RESPONSE:ERROR,incorrect number of parameters" << std::endl;
                    }
                } else if (input_list[0] == "SET_MIN_DISPARITY"){
                    if (input_list.size() == 2){
                        int val = std::stoi(input_list[1]);
                        val /= 20;
                        i3drsgm->setDisparityShift(val);
                        bool valid = true;
                        if (valid){
                            std::cout << "API_RESPONSE:parameter set" << std::endl;
                        } else {
                            std::cout << "API_RESPONSE:ERROR,failed to set parameter" << std::endl;
                        }
                    } else {
                        std::cout << "API_RESPONSE:ERROR,incorrect number of parameters" << std::endl;
                    }
                } else if (input_list[0] == "FORWARD_MATCH"){
                    if (input_list.size() == 8){
                        left_filepath = input_list[1];
                        right_filepath = input_list[2];
                        left_cal_filepath = input_list[3];
                        right_cal_filepath = input_list[4];
                        output_folder = input_list[5];
                        preRectified = input_list[6] == "1";
                        loop = input_list[7] == "1";
                        std::cout << left_filepath << std::endl;
                        std::cout << right_filepath << std::endl;
                        bool valid = I3DRSGM::forwardMatchFiles(i3drsgm,left_filepath,right_filepath,left_cal_filepath,right_cal_filepath,output_folder,preRectified);
                        if (valid){
                            std::cout << "API_RESPONSE:match success" << std::endl;
                        } else {
                            std::cout << "API_RESPONSE:ERROR,match failed" << std::endl;
                        }
                    } else {
                        std::cout << "API_RESPONSE:ERROR,incorrect number of parameters" << std::endl;
                    }
                }
            }
        }
    } else {
        i3drsgm = new I3DRSGM(tmp_param_filepath,param_filepath);
        bool valid = I3DRSGM::forwardMatchFiles(i3drsgm,left_filepath,right_filepath,left_cal_filepath,right_cal_filepath,output_folder,preRectified);
        if (valid){
            std::cout << "I3DRSGM match complete." << std::endl;
            return 0;
        } else {
            std::cout << "I3DRSGM match failed." << std::endl;
            return 1;
        }
    }
}