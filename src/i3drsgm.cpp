#include "i3drsgm.h"

/**
 * @file i3drsgm.cpp
 * @authors Ben Knight (bknight@i3drobotics.com)
 * @brief
 * @version 1.0.0
 * @date 2020-07-07
 *
 * @copyright Copyright (c) I3D Robotics Ltd, 2020
 *
 */



//Initialise matcher
I3DRSGM::I3DRSGM(std::string tmp_param_file, std::string param_file)
{
    tmp_param_file_ = tmp_param_file;
    //std::cout << param_file << std::endl;
    this->params_raw = ReadFileRaw(param_file);

    this->matcher_status = createMatcher();

    top_pyramid = 0;
}

int I3DRSGM::getStatus()
{
    return this->matcher_status;
}

bool I3DRSGM::EditParamRaw(std::vector<std::string> *lines, std::string param_name, std::string param_value)
{
    // find location of pyramid in file
    std::string full_parameters_string = param_name + " = " + param_value;
    bool param_found = false;
    for (std::vector<std::string>::iterator it = lines->begin(); it != lines->end(); ++it)
    {
        std::size_t found = it->find(param_name);
        if (found != std::string::npos)
        {
            param_found = true;
            int index = std::distance(lines->begin(), it);
            //std::cerr << "!!!!Parameter found @ " << index << "!!!" << std::endl;
            EditLineRaw(lines, full_parameters_string, index);
            break;
        }
    }
    return param_found;
}

bool I3DRSGM::EditPyramidParamRaw(std::vector<std::string> *lines, int pyramid_num, std::string param_name, std::string param_value, bool is_subpix)
{
    // find location of pyramid in file
    std::string pyramid_string;
    if (is_subpix)
    {
        pyramid_string = "[Pyramid " + std::to_string(pyramid_num) + " Subpix]";
    }
    else
    {
        pyramid_string = "[Pyramid " + std::to_string(pyramid_num) + "]";
    }
    std::string full_parameters_string = param_name + " = " + param_value;
    bool pyramid_found = false;
    bool param_found = false;
    for (std::vector<std::string>::iterator it = lines->begin(); it != lines->end(); ++it)
    {
        if (pyramid_found)
        {
            std::size_t found = it->find(param_name);
            if (found != std::string::npos)
            {
                param_found = true;
                int index = std::distance(lines->begin(), it);
                EditLineRaw(lines, full_parameters_string, index);
                break;
            }
        }
        else
        {
            if (it->find(pyramid_string) != std::string::npos)
            {
                pyramid_found = true;
            }
        }
    }
    return pyramid_found && param_found;
}

void I3DRSGM::EditLineRaw(std::vector<std::string> *lines, std::string value, int line_num)
{
    lines->at(line_num) = value + "\n";
}

std::vector<std::string> I3DRSGM::ReadFileRaw(std::string &filename)
{
    std::ifstream file(filename);
    std::vector<std::string> lines;
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file";
    }
    return lines;
}

void I3DRSGM::WriteIniFileRaw(std::string &filename, std::vector<std::string> lines)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
        {
            std::string line = *it;
            file << line << "\n";
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file";
    }
}

int I3DRSGM::getErrorDisparity(void)
{
    return -10000;
}

//compute disparity
cv::Mat I3DRSGM::forwardMatch(cv::Mat left, cv::Mat right)
{
    cv::Mat oDisparity = cv::Mat();
    //std::cout << "[I3DRSGM] Starting match..." << std::endl;
    const std::lock_guard<std::mutex> lock(mtx);
    if (matcher_handle != nullptr)
    {
        std::string sgm_log = "./sgm_log.txt";
        try
        {
            JR::Phobos::MatchStereo(matcher_handle,
                                    left,
                                    right,
                                    cv::Mat(),
                                    cv::Mat(),
                                    oDisparity,
                                    sgm_log,
                                    JR::Phobos::e_logError);
        }
        catch (...)
        {
            std::cerr << "FAILED TO COMPUTE" << std::endl;
        }
    }
    else
    {
        std::cerr << "Matcher handle not found" << std::endl;
        //this->matcher_status = createMatcher();
    }
    //std::cout << "[I3DRSGM] Match complete." << std::endl;
    return oDisparity;
}

//backward match disparity
cv::Mat I3DRSGM::backwardMatch(cv::Mat left, cv::Mat right)
{
    cv::Mat oDisparity;
    //std::cout << "[I3DRSGM] Starting match..." << std::endl;
    if (matcher_handle != nullptr)
    {
        std::string sgm_log = "./sgm_log.txt";
        try
        {
            JR::Phobos::MatchStereo(matcher_handle,
                                    left,
                                    right,
                                    cv::Mat(),
                                    cv::Mat(),
                                    oDisparity,
                                    sgm_log,
                                    JR::Phobos::e_logError);
        }
        catch (...)
        {
            std::cerr << "FAILED TO COMPUTE" << std::endl;
        }
    }
    else
    {
        std::cerr << "Matcher handle not found" << std::endl;
        //this->matcher_status = createMatcher();
    }
    //std::cout << "[I3DRSGM] Match complete." << std::endl;
    return oDisparity;
}

void I3DRSGM::enableCPU(bool enable)
{

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Use CPU SGM";

    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setDisparityError(int val)
{
    std::string param_name = "DSI Nodata Value";
    std::string param_val = std::to_string(-val); //value is inverted in algorithm to make it apear behind camera (to be ignored)
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setSpeckleDifference(float diff)
{
    //std::cout << "Speckle difference: " << diff << std::endl;
    diff = diff / 10;
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.fSpeckleMaxDiff = diff;
  }
  */
    std::string param_name = "Disparity Speckle Filter Max Difference";
    std::string param_val = std::to_string(diff);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setSpeckleSize(int size)
{
    //std::cout << "Speckle size: " << size << std::endl;
    size = size / 10;
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.iSpeckleMaxSize = size;
  }
  */
    std::string param_name = "Disparity Speckle Filter Max Region Size";
    std::string param_val = std::to_string(size);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setMatchCosts(float P1, float P2){
    setP1(P1);
    setP2(P2);
}

void I3DRSGM::setP1(float P1)
{
    float P1_scaled = P1 / 1000;
    float P1_subpix = P1_scaled / 10;
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.oSGMParams.fP1_E_W = P1_scaled;
    pyramid.oSGMParams.fP1_SE_NW = P1_scaled;
    pyramid.oSGMParams.fP1_SW_NE = P1_scaled;
    pyramid.oSGMParams.fP1_S_N = P1_scaled;
  }
  */
    std::string param_name_1 = "SE-NW Penalty 1";
    std::string param_name_2 = "SN Penalty 1";
    std::string param_name_3 = "SW-NE Penalty 1";
    std::string param_name_4 = "WE Penalty 1";
    std::string param_val = std::to_string(P1_scaled);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name_1, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_2, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_3, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_4, param_val);
    }
    std::string param_subpix_val = std::to_string(P1_subpix);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_1, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_2, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_3, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_4, param_subpix_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setP2(float P2)
{
    float P2_scaled = P2 / 1000;
    float P2_subpix = P2_scaled / 10;
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.oSGMParams.fP2_E_W = P2_scaled;
    pyramid.oSGMParams.fP2_SE_NW = P2_scaled;
    pyramid.oSGMParams.fP2_SW_NE = P2_scaled;
    pyramid.oSGMParams.fP2_S_N = P2_scaled;
  }
  */

    std::string param_name_1 = "SE-NW Penalty 2";
    std::string param_name_2 = "SN Penalty 2";
    std::string param_name_3 = "SW-NE Penalty 2";
    std::string param_name_4 = "WE Penalty 2";
    std::string param_val = std::to_string(P2_scaled);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name_1, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_2, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_3, param_val);
        EditPyramidParamRaw(&this->params_raw, i, param_name_4, param_val);
    }
    std::string param_subpix_val = std::to_string(P2_subpix);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_1, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_2, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_3, param_subpix_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, param_name_4, param_subpix_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setWindowSize(int census_size)
{
    if (census_size % 2 == 0)
    {
        census_size++;
    }

    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.oMetricParams.nWindowSizeX = census_size;
    pyramid.oMetricParams.nWindowSizeY = census_size;
  }

  //params.oPyramidParams[0].oMetricParams.nWindowSizeX = census_size;
  //params.oPyramidParams[0].oMetricParams.nWindowSizeY = census_size;
  */

    std::string x_param_name = "Disparity Median Optimizer Kernel Size X";
    std::string y_param_name = "Disparity Median Optimizer Kernel Size Y";
    std::string param_val = std::to_string(census_size);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, x_param_name, param_val);
        EditPyramidParamRaw(&this->params_raw, i, y_param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, x_param_name, param_val, true);
    EditPyramidParamRaw(&this->params_raw, 0, y_param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::setDisparityShift(int shift)
{
    //params.fTopPredictionShift = shift_p;

    //std::cout << "shift: " << shift << std::endl;
    //double shift_p = (double)shift / 20;
    //std::cout << "shift_p: " << shift_p << std::endl;

    std::string param_val = std::to_string(int(shift));

    std::string param_name = "Top Prediction Shift";
    EditParamRaw(&this->params_raw, param_name, param_val);
    this->matcher_status = createMatcher();
}

void I3DRSGM::maxPyramid(int pyramid_num)
{
    std::string param_val_true = "true";
    std::string param_val_false = "false";
    std::string param_name = "Process This Pyramid";

    bool include_subpix = false;

    if (pyramid_num > params.nNumberOfPyramids)
    {
        if (pyramid_num >= params.nNumberOfPyramids + 1)
        { //enable subpixel if pyramid number is 1+ number of pyramids available
            include_subpix = true;
        }
        pyramid_num = params.nNumberOfPyramids;
    }

    top_pyramid = params.nNumberOfPyramids - pyramid_num;

    int j = params.nNumberOfPyramids - 1;
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        if (i < pyramid_num)
        {
            EditPyramidParamRaw(&this->params_raw, j, param_name, param_val_true);
            //std::cerr << "p: " << j << std::endl;
        }
        else
        {
            EditPyramidParamRaw(&this->params_raw, j, param_name, param_val_false);
        }
        //std::cerr << "i: " << i << " j: " << j << std::endl;
        j--;
    }

    if (include_subpix)
    {
        enableSubpixel(true);
    }
    else
    {
        enableSubpixel(false);
    }

    this->matcher_status = createMatcher();
}

void I3DRSGM::enablePyramid(bool enable, int pyramid_num)
{
    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Process This Pyramid";
    EditPyramidParamRaw(&this->params_raw, pyramid_num, param_name, param_val);

    this->matcher_status = createMatcher();
}

void I3DRSGM::enableSubpixel(bool enable)
{
    //params.oFinalSubPixelParameters.bCompute = enable;

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Process This Pyramid";
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);

    this->matcher_status = createMatcher();
}

void I3DRSGM::setDisparityRange(int n)
{
    disparity_range = n / 10;
    //force odd number
    if (disparity_range % 2 == 0)
    {
        disparity_range++;
    }
    /* Set disparity range for all pyramids */
    std::string param_name = "Number Of Disparities";
    std::string param_val = std::to_string(disparity_range);
    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::enableTextureDSI(bool enable)
{
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.oSGMParams.bUseDSITexture = enable;
  }
  */

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Use DSI  Texture Memory";

    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::enableInterpolation(bool enable)
{
    /* Toggle interpolation */
    //params.oPyramidParams[1].bInterpol = enable

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Interpolate Disparity";

    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, "true");
    }

    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, false);
    //EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::enableOcclusionDetection(bool enable)
{
    /* Toggle occlusion detection */
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.bOcclusionDetection = enable;
  }

  params.oFinalSubPixelParameters.bOcclusionDetection = enable;
  */

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Occlusion Detection";

    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

void I3DRSGM::enableOccInterpol(bool enable)
{
    /* Toggle occlusion interpolation */
    /*
  for (auto &pyramid : params.oPyramidParams)
  {
    pyramid.bOccInterpol = enable;
  }

  params.oFinalSubPixelParameters.bOccInterpol = enable;
  */

    std::string param_val;
    if (enable)
    {
        param_val = "true";
    }
    else
    {
        param_val = "false";
    }

    std::string param_name = "Interpolate Occlusions";

    for (int i = 0; i <= params.nNumberOfPyramids; i++)
    {
        EditPyramidParamRaw(&this->params_raw, i, param_name, param_val);
    }
    EditPyramidParamRaw(&this->params_raw, 0, param_name, param_val, true);
    this->matcher_status = createMatcher();
}

int I3DRSGM::createMatcher()
{
    //std::cout << "[I3DRSGM] Creating matcher..." << std::endl;
    try
    {
        const std::lock_guard<std::mutex> lock(mtx);
        if (matcher_handle != nullptr)
        {
            //std::cout << "[I3DRSGM] Destroying Stereo Handle..." << std::endl;
            JR::Phobos::DestroyMatchStereoHandle(matcher_handle);
        }
        //std::cout << "Re-creating matcher with new paramters..." << std::endl;
        WriteIniFileRaw(this->tmp_param_file_, this->params_raw);
        this->params = JR::Phobos::SMatchingParametersInput();
        JR::Phobos::ReadIniFile(this->params, this->tmp_param_file_);
        matcher_handle = JR::Phobos::CreateMatchStereoHandle(params);
        //std::cout << "Re-created matcher with new paramters." << std::endl;
        return 0;
    }
    catch (...)
    {
        std::cerr << "Failed to create I3DR matcher with chosen parameters" << std::endl;
    }
    return -1;
}

std::string I3DRSGM::getCWD(){
    char cCurrentPath[FILENAME_MAX];
    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    return std::string(cCurrentPath);
}

std::string I3DRSGM::getAppPath()
{
    std::string path;
    #ifdef _WIN32
        char path_exe[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, path_exe, MAX_PATH);
        path = std::string(path_exe);
    #else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        path = std::string(result, (count > 0) ? count : 0);
    #endif
    std::string folder = path.substr(0, path.find_last_of("\\/"));
    return folder;
}

bool I3DRSGM::forwardMatchFiles(I3DRSGM* i3drsgm,
    std::string left_image_filepath, std::string right_image_filepath, 
    std::string output_folder)
{
    cv::Mat left, right;
    // check image files exist
    if (cv::utils::fs::exists(left_image_filepath) && cv::utils::fs::exists(right_image_filepath)){
        // read left and right image from file to OpenCV Mat
        try {
            left = cv::imread(left_image_filepath);
            right = cv::imread(right_image_filepath);
        } catch( cv::Exception& e ) {
            const char* err_msg = e.what();
            std::cerr << "exception caught: " << err_msg << std::endl;
            return false;
        }
        if (left.empty() || right.empty()){
            std::cerr << "left or right image is empty" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Image file does not exist" << std::endl;
        return false;
    }

    #if defined(_WIN32)
        _mkdir(output_folder.c_str());
    #else 
        mkdir(output_folder.c_str(), 0777);
    #endif

    // Run stereo match on left and right images
    //std::cout << "Generating disparity from stereo pair..." << std::endl;
    cv::Mat disp = i3drsgm->forwardMatch(left,right);

    if (disp.empty()){
        std::cerr << "disparity image empty" << std::endl;
        return false;
    }

    try {
        cv::imwrite(output_folder+"/disparity.tif",disp);
        //std::cout << "Disparity saved to: " << output_folder+"/disparity.tif" << std::endl;
    } catch( cv::Exception& e ) {
        const char* err_msg = e.what();
        std::cerr << "exception caught: " << err_msg << std::endl;
        return false;
    }
    return true;
}

bool I3DRSGM::forwardMatchFiles(I3DRSGM* i3drsgm,
    std::string left_image_filepath, std::string right_image_filepath, 
    std::string left_yaml_cal_filepath, std::string right_yaml_cal_filepath,
    std::string output_folder, bool preRectified)
{
    cv::Mat left, right;
    // check image files exist
    if (cv::utils::fs::exists(left_image_filepath) && cv::utils::fs::exists(right_image_filepath)){
        // read left and right image from file to OpenCV Mat
        try {
            left = cv::imread(left_image_filepath);
            right = cv::imread(right_image_filepath);
        } catch( cv::Exception& e ) {
            const char* err_msg = e.what();
            std::cerr << "exception caught: " << err_msg << std::endl;
            return false;
        }
        if (left.empty() || right.empty()){
            std::cerr << "left or right image is empty" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Image file does not exist" << std::endl;
        return false;
    }
    // check calibration files exist (xml or yaml)
    cv::Mat l_camera_matrix, r_camera_matrix;
    cv::Mat l_dist_coef, r_dist_coef;
    cv::Mat l_rect_matrix, r_rect_matrix;
    cv::Mat l_proj_matrix, r_proj_matrix;
    cv::Mat l_rectmapx, r_rectmapx;
    cv::Mat l_rectmapy, r_rectmapy;
    cv::Mat Q;
    cv::Size cal_image_size;
    //std::cout << "Loading calibration files..." << std::endl;
    bool cal_valid = StereoSupport::loadCalibrationFromYamls(
        left_yaml_cal_filepath,right_yaml_cal_filepath,
        l_camera_matrix,l_dist_coef,l_rect_matrix,l_proj_matrix,l_rectmapx,l_rectmapy,
        r_camera_matrix,r_dist_coef,r_rect_matrix,r_proj_matrix,r_rectmapx,r_rectmapy,
        cal_image_size,Q);
    if (!cal_valid){
        std::cerr << "Invalid calibration file/s" << std::endl;
        return false;
    }

    #if defined(_WIN32)
        _mkdir(output_folder.c_str());
    #else 
        mkdir(output_folder.c_str(), 0777);
    #endif

    if (!preRectified){
        // rectify input images
        //std::cout << "Rectifying images..." << std::endl;
        cv::remap(left, left, l_rectmapx, l_rectmapy, cv::INTER_CUBIC);
        cv::remap(right, right, r_rectmapx, r_rectmapy, cv::INTER_CUBIC);
    }

    // Run stereo match on left and right images
    //std::cout << "Generating disparity from stereo pair..." << std::endl;
    cv::Mat disp = i3drsgm->forwardMatch(left,right);

    if (disp.empty()){
        std::cerr << "disparity image empty" << std::endl;
        return false;
    }

    try {
        cv::imwrite(output_folder+"/disparity.tif",disp);
        //std::cout << "Disparity saved to: " << output_folder+"/disparity.tif" << std::endl;
    } catch( cv::Exception& e ) {
        const char* err_msg = e.what();
        std::cerr << "exception caught: " << err_msg << std::endl;
        return false;
    }

    /*
    TODO
    if (output_norm_disp){
        // normalise disparity
        // apply colormap to normalised disparity
        // save normalised with colormap to png file
    }

    if (output_depth || output_points){
        // generate depth from disparity
        std::cout << "Generating depth from disparity..." << std::endl;
        cv::Mat depth;
        cv::reprojectImageTo3D(disp,depth,Q);
        // save depth image to tiff file
        if (output_points){
            // generate point cloud from depth image
            // save point cloud to file
        }
    }
    */

    return true;
}

bool I3DRSGM::forwardMatchFiles(
    std::string left_image_filepath, std::string right_image_filepath, 
    std::string left_yaml_cal_filepath, std::string right_yaml_cal_filepath,
    std::string output_folder, bool preRectified)
{
    if (!I3DRSGM::isLicenseValid()){return false;}

    std::string tmp_param_filepath = getAppPath()+"/i3drsgm_tmp.param";
    std::string param_filepath = getAppPath()+"/i3drsgm.param";
    // initalise matcher with param files
    I3DRSGM * i3drsgm = new I3DRSGM(tmp_param_filepath,param_filepath);

    return forwardMatchFiles(i3drsgm,left_image_filepath,right_image_filepath,left_yaml_cal_filepath,right_yaml_cal_filepath,output_folder,preRectified);
}

void I3DRSGM::printParams() {
    bool bHasNodata = params.bHasNodata;
    std::cout << "params.bHasNodata = " << bHasNodata << ";" << std::endl;
    float fNodataValue = params.fNodataValue;
    std::cout << "params.fNodataValue = " << fNodataValue << ";" << std::endl;


    //Output
    bool bWriteBackDisp = params.bWriteBackDisp;
    std::cout << "params.bWriteBackDisp = " << bWriteBackDisp << ";" << std::endl;

    //parameter
    bool bWriteBackMatchDistance = params.bWriteBackMatchDistance;
    std::cout << "params.bWriteBackMatchDistance = " << bWriteBackMatchDistance << ";" << std::endl;
    bool bWriteRevBackMatchDistance = params.bWriteRevBackMatchDistance;
    std::cout << "params.bWriteRevBackMatchDistance = " << bWriteRevBackMatchDistance << ";" << std::endl;
    std::string strBackMatchDistanceOutputFile = params.strBackMatchDistanceOutputFile;
    std::cout << "params.strBackMatchDistanceOutputFile = \"" << strBackMatchDistanceOutputFile << "\";" << std::endl;
    std::string strRevBackMatchDistanceOutputFile = params.strRevBackMatchDistanceOutputFile;
    std::cout << "params.strRevBackMatchDistanceOutputFile = \"" << strRevBackMatchDistanceOutputFile << "\";" << std::endl;

    int nNumberOfPyramids = params.nNumberOfPyramids;
    std::cout << "params.nNumberOfPyramids = " << nNumberOfPyramids << ";" << std::endl;
    int nPredictor = params.nPredictor;
    std::cout << "params.nPredictor = " << nPredictor << ";" << std::endl;
    std::string strDispPredictionFile = params.strDispPredictionFile;
    std::cout << "params.strDispPredictionFile = \"" << strDispPredictionFile << "\";" << std::endl;
    std::string strBackDispPredictionFile = params.strBackDispPredictionFile;
    std::cout << "params.strBackDispPredictionFile = \"" << strBackDispPredictionFile << "\";" << std::endl;
    float fTopPredictionShift = params.fTopPredictionShift;
    std::cout << "params.fTopPredictionShift = " << fTopPredictionShift << ";" << std::endl;
    int nJointMode = params.nJointMode;
    std::cout << "params.nJointMode = " << nJointMode << ";" << std::endl;
    int nJointModeKernelSize = params.nJointModeKernelSize;
    std::cout << "params.nJointModeKernelSize = " << nJointModeKernelSize << ";" << std::endl;
    int nNumberOfMorphologicalIter = params.nNumberOfMorphologicalIter;
    std::cout << "params.nNumberOfMorphologicalIter = " << nNumberOfMorphologicalIter << ";" << std::endl;

    std::vector<int> oGPUs = params.oGPUs;
    int i = 0;
    for (int oGPU : oGPUs) {
        std::cout << "params.oGPUs[" << i << "] = " << oGPU << ";" << std::endl;
        i++;
    }
    
    std::vector<JR::Phobos::SMatchingParameters> oPyramidParams = params.oPyramidParams;
    i = 0;
    for (JR::Phobos::SMatchingParameters oPyramidParam : oPyramidParams) {
        bool bCompute = oPyramidParam.bCompute;
        std::cout << "params.oPyramidParams[" << i << "].bCompute = " << bCompute << ";" << std::endl;
        int nMetric = oPyramidParam.nMetric;
        std::cout << "params.oPyramidParams[" << i << "].nMetric = " << nMetric << ";" << std::endl;

        JR::Phobos::SMetricParameters oMetricParams = oPyramidParam.oMetricParams;
        int nWindowSizeX = oMetricParams.nWindowSizeX;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nWindowSizeX = " << nWindowSizeX << ";" << std::endl;
        int nWindowSizeY = oMetricParams.nWindowSizeY;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nWindowSizeY = " << nWindowSizeY << ";" << std::endl;
        float fMetricDeltaX = oMetricParams.fMetricDeltaX;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fMetricDeltaX = " << fMetricDeltaX << ";" << std::endl;
        float fMetricDeltaY = oMetricParams.fMetricDeltaY;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fMetricDeltaY = " << fMetricDeltaY << ";" << std::endl;
        int nCensusCenterFunction = oMetricParams.nCensusCenterFunction;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nCensusCenterFunction = " << nCensusCenterFunction << ";" << std::endl;
        bool bUseCensusTexture = oMetricParams.bUseCensusTexture;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.bUseCensusTexture = " << bUseCensusTexture << ";" << std::endl;
        int nRotatedWindow = oMetricParams.nRotatedWindow;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nRotatedWindow = " << nRotatedWindow << ";" << std::endl;

        int nNumberOfScales = oMetricParams.nNumberOfScales;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nNumberOfScales = " << nNumberOfScales << ";" << std::endl;
        float fScaleStepSize = oMetricParams.fScaleStepSize;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fScaleStepSize = " << fScaleStepSize << ";" << std::endl;
        int nNumberOfSlants = oMetricParams.nNumberOfSlants;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.nNumberOfSlants = " << nNumberOfSlants << ";" << std::endl;
        float fSlantStepSize = oMetricParams.fSlantStepSize;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fSlantStepSize = " << fSlantStepSize << ";" << std::endl;

        bool bUseProximityWeight = oMetricParams.bUseProximityWeight;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.bUseProximityWeight = " << bUseProximityWeight << ";" << std::endl;
        float fProximityGamma = oMetricParams.fProximityGamma;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fProximityGamma = " << fProximityGamma << ";" << std::endl;

        bool bUseSimilarityWeight = oMetricParams.bUseSimilarityWeight;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.bUseSimilarityWeight = " << bUseSimilarityWeight << ";" << std::endl;
        float fSimilarityGamma = oMetricParams.fSimilarityGamma;
        std::cout << "params.oPyramidParams[" << i << "].oMetricParams.fSimilarityGamma = " << fSimilarityGamma << ";" << std::endl;



        int nMaximumNumberOfDisparities = oPyramidParam.nMaximumNumberOfDisparities;
        std::cout << "params.oPyramidParams[" << i << "].nMaximumNumberOfDisparities = " << nMaximumNumberOfDisparities << ";" << std::endl;

        float fDisparityStepSize = oPyramidParam.fDisparityStepSize;
        std::cout << "params.oPyramidParams[" << i << "].fDisparityStepSize = " << fDisparityStepSize << ";" << std::endl;
        int nSubpixelFnc = oPyramidParam.nSubpixelFnc;
        std::cout << "params.oPyramidParams[" << i << "].nSubpixelFnc = " << nSubpixelFnc << ";" << std::endl;
        float fMaxCost = oPyramidParam.fMaxCost;
        std::cout << "params.oPyramidParams[" << i << "].fMaxCost = " << fMaxCost << ";" << std::endl;
        bool bDoSubpixelNormalization = oPyramidParam.bDoSubpixelNormalization;
        std::cout << "params.oPyramidParams[" << i << "].bDoSubpixelNormalization = " << bDoSubpixelNormalization << ";" << std::endl;
        
        JR::Phobos::SSGMParameters oSGMParams = oPyramidParam.oSGMParams;
        float fCostNodataValue = oSGMParams.fCostNodataValue;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fCostNodataValue = " << fCostNodataValue << ";" << std::endl;
        int nAllowedOutsideImageMatchCount = oSGMParams.nAllowedOutsideImageMatchCount;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAllowedOutsideImageMatchCount = " << nAllowedOutsideImageMatchCount << ";" << std::endl;
        bool bDSICountCorrection = oSGMParams.bDSICountCorrection;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bDSICountCorrection = " << bDSICountCorrection << ";" << std::endl;
        int nDSICountCorrectionNoDataCheckMode = oSGMParams.nDSICountCorrectionNoDataCheckMode;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nDSICountCorrectionNoDataCheckMode = " << nDSICountCorrectionNoDataCheckMode << ";" << std::endl;

        bool bMatch_S_N = oSGMParams.bMatch_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bMatch_S_N = " << bMatch_S_N << ";" << std::endl;
        float fP1_S_N = oSGMParams.fP1_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP1_S_N = " << fP1_S_N << ";" << std::endl;
        float fP2_S_N = oSGMParams.fP2_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP2_S_N = " << fP2_S_N << ";" << std::endl;
        bool bP2EqualP1_S_N = oSGMParams.bP2EqualP1_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bP2EqualP1_S_N = " << bP2EqualP1_S_N << ";" << std::endl;
        int nAdaptiveP1Fnc_S_N = oSGMParams.nAdaptiveP1Fnc_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP1Fnc_S_N = " << nAdaptiveP1Fnc_S_N << ";" << std::endl;
        int nAdaptiveP2Fnc_S_N = oSGMParams.nAdaptiveP2Fnc_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP2Fnc_S_N = " << nAdaptiveP2Fnc_S_N << ";" << std::endl;
        bool bAdditive_S_N = oSGMParams.bAdditive_S_N;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bAdditive_S_N = " << bAdditive_S_N << ";" << std::endl;

        bool bMatch_E_W = oSGMParams.bMatch_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bMatch_E_W = " << bMatch_E_W << ";" << std::endl;
        float fP1_E_W = oSGMParams.fP1_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP1_E_W = " << fP1_E_W << ";" << std::endl;
        float fP2_E_W = oSGMParams.fP2_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP2_E_W = " << fP2_E_W << ";" << std::endl;
        bool bP2EqualP1_E_W = oSGMParams.bP2EqualP1_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bP2EqualP1_E_W = " << bP2EqualP1_E_W << ";" << std::endl;
        int nAdaptiveP1Fnc_E_W = oSGMParams.nAdaptiveP1Fnc_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP1Fnc_E_W = " << nAdaptiveP1Fnc_E_W << ";" << std::endl;
        int nAdaptiveP2Fnc_E_W = oSGMParams.nAdaptiveP2Fnc_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP2Fnc_E_W = " << nAdaptiveP2Fnc_E_W << ";" << std::endl;
        bool bAdditive_E_W = oSGMParams.bAdditive_E_W;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bAdditive_E_W = " << bAdditive_E_W << ";" << std::endl;

        bool bMatch_SW_NE = oSGMParams.bMatch_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bMatch_SW_NE = " << bMatch_SW_NE << ";" << std::endl;
        float fP1_SW_NE = oSGMParams.fP1_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP1_SW_NE = " << fP1_SW_NE << ";" << std::endl;
        float fP2_SW_NE = oSGMParams.fP2_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP2_SW_NE = " << fP2_SW_NE << ";" << std::endl;
        bool bP2EqualP1_SW_NE = oSGMParams.bP2EqualP1_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bP2EqualP1_SW_NE = " << bP2EqualP1_SW_NE << ";" << std::endl;
        int nAdaptiveP1Fnc_SW_NE = oSGMParams.nAdaptiveP1Fnc_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP1Fnc_SW_NE = " << nAdaptiveP1Fnc_SW_NE << ";" << std::endl;
        int nAdaptiveP2Fnc_SW_NE = oSGMParams.nAdaptiveP2Fnc_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP2Fnc_SW_NE = " << nAdaptiveP2Fnc_SW_NE << ";" << std::endl;
        bool bAdditive_SW_NE = oSGMParams.bAdditive_SW_NE;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bAdditive_SW_NE = " << bAdditive_SW_NE << ";" << std::endl;

        bool bMatch_SE_NW = oSGMParams.bMatch_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bMatch_SE_NW = " << bMatch_SE_NW << ";" << std::endl;
        float fP1_SE_NW = oSGMParams.fP1_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP1_SE_NW = " << fP1_SE_NW << ";" << std::endl;
        float fP2_SE_NW = oSGMParams.fP2_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.fP2_SE_NW = " << fP2_SE_NW << ";" << std::endl;
        bool bP2EqualP1_SE_NW = oSGMParams.bP2EqualP1_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bP2EqualP1_SE_NW = " << bP2EqualP1_SE_NW << ";" << std::endl;
        int nAdaptiveP1Fnc_SE_NW = oSGMParams.nAdaptiveP1Fnc_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP1Fnc_SE_NW = " << nAdaptiveP1Fnc_SE_NW << ";" << std::endl;
        int nAdaptiveP2Fnc_SE_NW = oSGMParams.nAdaptiveP2Fnc_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.nAdaptiveP2Fnc_SE_NW = " << nAdaptiveP2Fnc_SE_NW << ";" << std::endl;
        bool bAdditive_SE_NW = oSGMParams.bAdditive_SE_NW;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bAdditive_SE_NW = " << bAdditive_SE_NW << ";" << std::endl;


        bool bUseDSITexture = oSGMParams.bUseDSITexture;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bUseDSITexture = " << bUseDSITexture << ";" << std::endl;
        bool bUseAggregatedCostTexture = oSGMParams.bUseAggregatedCostTexture;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bUseAggregatedCostTexture = " << bUseAggregatedCostTexture << ";" << std::endl;
        bool bUseCPUSgm = oSGMParams.bUseCPUSgm;
        std::cout << "params.oPyramidParams[" << i << "].oSGMParams.bUseCPUSgm = " << bUseCPUSgm << ";" << std::endl;

        bool bMultiGPU_AfterDSI = oPyramidParam.bMultiGPU_AfterDSI;
        std::cout << "params.oPyramidParams[" << i << "].bMultiGPU_AfterDSI = " << bMultiGPU_AfterDSI << ";" << std::endl;

        bool bMedian = oPyramidParam.bMedian;
        std::cout << "params.oPyramidParams[" << i << "].bMedian = " << bMedian << ";" << std::endl;
        int iMedX = oPyramidParam.iMedX;
        std::cout << "params.oPyramidParams[" << i << "].iMedX = " << iMedX << ";" << std::endl;
        int iMedY = oPyramidParam.iMedY;
        std::cout << "params.oPyramidParams[" << i << "].iMedY = " << iMedY << ";" << std::endl;

        bool bSpeckle = oPyramidParam.bSpeckle;
        std::cout << "params.oPyramidParams[" << i << "].bSpeckle = " << bSpeckle << ";" << std::endl;

        int iNodataPol = oPyramidParam.iNodataPol;
        std::cout << "params.oPyramidParams[" << i << "].iNodataPol = " << iNodataPol << ";" << std::endl;
        int iBorderPol = oPyramidParam.iBorderPol;
        std::cout << "params.oPyramidParams[" << i << "].iBorderPol = " << iBorderPol << ";" << std::endl;
        float fMedianThresh = oPyramidParam.fMedianThresh;
        std::cout << "params.oPyramidParams[" << i << "].fMedianThresh = " << fMedianThresh << ";" << std::endl;
        bool bSplittedMedian = oPyramidParam.bSplittedMedian;
        std::cout << "params.oPyramidParams[" << i << "].bSplittedMedian = " << bSplittedMedian << ";" << std::endl;

        int iSpeckleMaxSize = oPyramidParam.iSpeckleMaxSize;
        std::cout << "params.oPyramidParams[" << i << "].iSpeckleMaxSize = " << iSpeckleMaxSize << ";" << std::endl;
        float fSpeckleMaxDiff = oPyramidParam.fSpeckleMaxDiff;
        std::cout << "params.oPyramidParams[" << i << "].fSpeckleMaxDiff = " << fSpeckleMaxDiff << ";" << std::endl;
        //back matching parameters:
        bool bBackMatch = oPyramidParam.bBackMatch;
        std::cout << "params.oPyramidParams[" << i << "].bBackMatch = " << bBackMatch << ";" << std::endl;
        bool bComputeBackDisparity = oPyramidParam.bComputeBackDisparity;
        std::cout << "params.oPyramidParams[" << i << "].bComputeBackDisparity = " << bComputeBackDisparity << ";" << std::endl;

        float fMaxBackMatchingDist = oPyramidParam.fMaxBackMatchingDist;
        std::cout << "params.oPyramidParams[" << i << "].fMaxBackMatchingDist = " << fMaxBackMatchingDist << ";" << std::endl;
        bool bKeepValueOnNodataInBackmatch = oPyramidParam.bKeepValueOnNodataInBackmatch;
        std::cout << "params.oPyramidParams[" << i << "].bKeepValueOnNodataInBackmatch = " << bKeepValueOnNodataInBackmatch << ";" << std::endl;
        bool bKeepValueOnOutsideImage = oPyramidParam.bKeepValueOnOutsideImage;
        std::cout << "params.oPyramidParams[" << i << "].bKeepValueOnOutsideImage = " << bKeepValueOnOutsideImage << ";" << std::endl;

        bool bOcclusionDetection = oPyramidParam.bOcclusionDetection;
        std::cout << "params.oPyramidParams[" << i << "].bOcclusionDetection = " << bOcclusionDetection << ";" << std::endl;
        float fOcclusionDispGapSize = oPyramidParam.fOcclusionDispGapSize;
        std::cout << "params.oPyramidParams[" << i << "].fOcclusionDispGapSize = " << fOcclusionDispGapSize << ";" << std::endl;
        int nRegionGrowingMode = oPyramidParam.nRegionGrowingMode;
        std::cout << "params.oPyramidParams[" << i << "].nRegionGrowingMode = " << nRegionGrowingMode << ";" << std::endl;
        int nRegionGrowingIterations = oPyramidParam.nRegionGrowingIterations;
        std::cout << "params.oPyramidParams[" << i << "].nRegionGrowingIterations = " << nRegionGrowingIterations << ";" << std::endl;

        bool bInterpol = oPyramidParam.bInterpol;
        std::cout << "params.oPyramidParams[" << i << "].bInterpol = " << bInterpol << ";" << std::endl;
        int nDir = oPyramidParam.nDir;
        std::cout << "params.oPyramidParams[" << i << "].nDir = " << nDir << ";" << std::endl;
        int nNth = oPyramidParam.nNth;
        std::cout << "params.oPyramidParams[" << i << "].nNth = " << nNth << ";" << std::endl;
        int nMinNth = oPyramidParam.nMinNth;
        std::cout << "params.oPyramidParams[" << i << "].nMinNth = " << nMinNth << ";" << std::endl;
        float fRadius = oPyramidParam.fRadius;
        std::cout << "params.oPyramidParams[" << i << "].fRadius = " << fRadius << ";" << std::endl;
        int nMode = oPyramidParam.nMode;
        std::cout << "params.oPyramidParams[" << i << "].nMode = " << nMode << ";" << std::endl;

        bool bOccInterpol = oPyramidParam.bOccInterpol;       
        std::cout << "params.oPyramidParams[" << i << "].bOccInterpol = " << bOccInterpol << ";" << std::endl;
        int nOccDir = oPyramidParam.nOccDir;
        std::cout << "params.oPyramidParams[" << i << "].nOccDir = " << nOccDir << ";" << std::endl;
        int nOccNth = oPyramidParam.nOccNth;
        std::cout << "params.oPyramidParams[" << i << "].nOccNth = " << nOccNth << ";" << std::endl;
        int nOccMinNth = oPyramidParam.nOccMinNth;
        std::cout << "params.oPyramidParams[" << i << "].nOccMinNth = " << nOccMinNth << ";" << std::endl;
        float fOccRadius = oPyramidParam.fOccRadius;
        std::cout << "params.oPyramidParams[" << i << "].fOccRadius = " << fOccRadius << ";" << std::endl;
        int nOccMode = oPyramidParam.nOccMode;
        std::cout << "params.oPyramidParams[" << i << "].nOccMode = " << nOccMode << ";" << std::endl;
        i++;
    }
    JR::Phobos::SMatchingParameters oFinalSubPixelParameters = params.oFinalSubPixelParameters;
    bool bCompute = oFinalSubPixelParameters.bCompute;
    std::cout << "params.oFinalSubPixelParameters.bCompute = " << bCompute << ";" << std::endl;
    int nMetric = oFinalSubPixelParameters.nMetric;
    std::cout << "params.oFinalSubPixelParameters.nMetric = " << nMetric << ";" << std::endl;

    JR::Phobos::SMetricParameters oMetricParams = oFinalSubPixelParameters.oMetricParams;
    int nWindowSizeX = oMetricParams.nWindowSizeX;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nWindowSizeX = " << nWindowSizeX << ";" << std::endl;
    int nWindowSizeY = oMetricParams.nWindowSizeY;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nWindowSizeY = " << nWindowSizeY << ";" << std::endl;
    float fMetricDeltaX = oMetricParams.fMetricDeltaX;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fMetricDeltaX = " << fMetricDeltaX << ";" << std::endl;
    float fMetricDeltaY = oMetricParams.fMetricDeltaY;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fMetricDeltaY = " << fMetricDeltaY << ";" << std::endl;
    int nCensusCenterFunction = oMetricParams.nCensusCenterFunction;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nCensusCenterFunction = " << nCensusCenterFunction << ";" << std::endl;
    bool bUseCensusTexture = oMetricParams.bUseCensusTexture;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.bUseCensusTexture = " << bUseCensusTexture << ";" << std::endl;
    int nRotatedWindow = oMetricParams.nRotatedWindow;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nRotatedWindow = " << nRotatedWindow << ";" << std::endl;

    int nNumberOfScales = oMetricParams.nNumberOfScales;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nNumberOfScales = " << nNumberOfScales << ";" << std::endl;
    float fScaleStepSize = oMetricParams.fScaleStepSize;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fScaleStepSize = " << fScaleStepSize << ";" << std::endl;
    int nNumberOfSlants = oMetricParams.nNumberOfSlants;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.nNumberOfSlants = " << nNumberOfSlants << ";" << std::endl;
    float fSlantStepSize = oMetricParams.fSlantStepSize;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fSlantStepSize = " << fSlantStepSize << ";" << std::endl;

    bool bUseProximityWeight = oMetricParams.bUseProximityWeight;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.bUseProximityWeight = " << bUseProximityWeight << ";" << std::endl;
    float fProximityGamma = oMetricParams.fProximityGamma;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fProximityGamma = " << fProximityGamma << ";" << std::endl;

    bool bUseSimilarityWeight = oMetricParams.bUseSimilarityWeight;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.bUseSimilarityWeight = " << bUseSimilarityWeight << ";" << std::endl;
    float fSimilarityGamma = oMetricParams.fSimilarityGamma;
    std::cout << "params.oFinalSubPixelParameters.oMetricParams.fSimilarityGamma = " << fSimilarityGamma << ";" << std::endl;



    int nMaximumNumberOfDisparities = oFinalSubPixelParameters.nMaximumNumberOfDisparities;
    std::cout << "params.oFinalSubPixelParameters.nMaximumNumberOfDisparities = " << nMaximumNumberOfDisparities << ";" << std::endl;

    float fDisparityStepSize = oFinalSubPixelParameters.fDisparityStepSize;
    std::cout << "params.oFinalSubPixelParameters.fDisparityStepSize = " << fDisparityStepSize << ";" << std::endl;
    int nSubpixelFnc = oFinalSubPixelParameters.nSubpixelFnc;
    std::cout << "params.oFinalSubPixelParameters.nSubpixelFnc = " << nSubpixelFnc << ";" << std::endl;
    float fMaxCost = oFinalSubPixelParameters.fMaxCost;
    std::cout << "params.oFinalSubPixelParameters.fMaxCost = " << fMaxCost << ";" << std::endl;
    bool bDoSubpixelNormalization = oFinalSubPixelParameters.bDoSubpixelNormalization;
    std::cout << "params.oFinalSubPixelParameters.bDoSubpixelNormalization = " << bDoSubpixelNormalization << ";" << std::endl;

    JR::Phobos::SSGMParameters oSGMParams = oFinalSubPixelParameters.oSGMParams;
    float fCostNodataValue = oSGMParams.fCostNodataValue;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fCostNodataValue = " << fCostNodataValue << ";" << std::endl;
    int nAllowedOutsideImageMatchCount = oSGMParams.nAllowedOutsideImageMatchCount;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAllowedOutsideImageMatchCount = " << nAllowedOutsideImageMatchCount << ";" << std::endl;
    bool bDSICountCorrection = oSGMParams.bDSICountCorrection;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bDSICountCorrection = " << bDSICountCorrection << ";" << std::endl;
    int nDSICountCorrectionNoDataCheckMode = oSGMParams.nDSICountCorrectionNoDataCheckMode;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nDSICountCorrectionNoDataCheckMode = " << nDSICountCorrectionNoDataCheckMode << ";" << std::endl;

    bool bMatch_S_N = oSGMParams.bMatch_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bMatch_S_N = " << bMatch_S_N << ";" << std::endl;
    float fP1_S_N = oSGMParams.fP1_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP1_S_N = " << fP1_S_N << ";" << std::endl;
    float fP2_S_N = oSGMParams.fP2_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP2_S_N = " << fP2_S_N << ";" << std::endl;
    bool bP2EqualP1_S_N = oSGMParams.bP2EqualP1_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bP2EqualP1_S_N = " << bP2EqualP1_S_N << ";" << std::endl;
    int nAdaptiveP1Fnc_S_N = oSGMParams.nAdaptiveP1Fnc_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP1Fnc_S_N = " << nAdaptiveP1Fnc_S_N << ";" << std::endl;
    int nAdaptiveP2Fnc_S_N = oSGMParams.nAdaptiveP2Fnc_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP2Fnc_S_N = " << nAdaptiveP2Fnc_S_N << ";" << std::endl;
    bool bAdditive_S_N = oSGMParams.bAdditive_S_N;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bAdditive_S_N = " << bAdditive_S_N << ";" << std::endl;

    bool bMatch_E_W = oSGMParams.bMatch_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bMatch_E_W = " << bMatch_E_W << ";" << std::endl;
    float fP1_E_W = oSGMParams.fP1_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP1_E_W = " << fP1_E_W << ";" << std::endl;
    float fP2_E_W = oSGMParams.fP2_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP2_E_W = " << fP2_E_W << ";" << std::endl;
    bool bP2EqualP1_E_W = oSGMParams.bP2EqualP1_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bP2EqualP1_E_W = " << bP2EqualP1_E_W << ";" << std::endl;
    int nAdaptiveP1Fnc_E_W = oSGMParams.nAdaptiveP1Fnc_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP1Fnc_E_W = " << nAdaptiveP1Fnc_E_W << ";" << std::endl;
    int nAdaptiveP2Fnc_E_W = oSGMParams.nAdaptiveP2Fnc_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP2Fnc_E_W = " << nAdaptiveP2Fnc_E_W << ";" << std::endl;
    bool bAdditive_E_W = oSGMParams.bAdditive_E_W;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bAdditive_E_W = " << bAdditive_E_W << ";" << std::endl;

    bool bMatch_SW_NE = oSGMParams.bMatch_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bMatch_SW_NE = " << bMatch_SW_NE << ";" << std::endl;
    float fP1_SW_NE = oSGMParams.fP1_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP1_SW_NE = " << fP1_SW_NE << ";" << std::endl;
    float fP2_SW_NE = oSGMParams.fP2_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP2_SW_NE = " << fP2_SW_NE << ";" << std::endl;
    bool bP2EqualP1_SW_NE = oSGMParams.bP2EqualP1_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bP2EqualP1_SW_NE = " << bP2EqualP1_SW_NE << ";" << std::endl;
    int nAdaptiveP1Fnc_SW_NE = oSGMParams.nAdaptiveP1Fnc_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP1Fnc_SW_NE = " << nAdaptiveP1Fnc_SW_NE << ";" << std::endl;
    int nAdaptiveP2Fnc_SW_NE = oSGMParams.nAdaptiveP2Fnc_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP2Fnc_SW_NE = " << nAdaptiveP2Fnc_SW_NE << ";" << std::endl;
    bool bAdditive_SW_NE = oSGMParams.bAdditive_SW_NE;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bAdditive_SW_NE = " << bAdditive_SW_NE << ";" << std::endl;

    bool bMatch_SE_NW = oSGMParams.bMatch_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bMatch_SE_NW = " << bMatch_SE_NW << ";" << std::endl;
    float fP1_SE_NW = oSGMParams.fP1_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP1_SE_NW = " << fP1_SE_NW << ";" << std::endl;
    float fP2_SE_NW = oSGMParams.fP2_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.fP2_SE_NW = " << fP2_SE_NW << ";" << std::endl;
    bool bP2EqualP1_SE_NW = oSGMParams.bP2EqualP1_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bP2EqualP1_SE_NW = " << bP2EqualP1_SE_NW << ";" << std::endl;
    int nAdaptiveP1Fnc_SE_NW = oSGMParams.nAdaptiveP1Fnc_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP1Fnc_SE_NW = " << nAdaptiveP1Fnc_SE_NW << ";" << std::endl;
    int nAdaptiveP2Fnc_SE_NW = oSGMParams.nAdaptiveP2Fnc_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.nAdaptiveP2Fnc_SE_NW = " << nAdaptiveP2Fnc_SE_NW << ";" << std::endl;
    bool bAdditive_SE_NW = oSGMParams.bAdditive_SE_NW;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bAdditive_SE_NW = " << bAdditive_SE_NW << ";" << std::endl;


    bool bUseDSITexture = oSGMParams.bUseDSITexture;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bUseDSITexture = " << bUseDSITexture << ";" << std::endl;
    bool bUseAggregatedCostTexture = oSGMParams.bUseAggregatedCostTexture;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bUseAggregatedCostTexture = " << bUseAggregatedCostTexture << ";" << std::endl;
    bool bUseCPUSgm = oSGMParams.bUseCPUSgm;
    std::cout << "params.oFinalSubPixelParameters.oSGMParams.bUseCPUSgm = " << bUseCPUSgm << ";" << std::endl;


    bool bMultiGPU_AfterDSI = oFinalSubPixelParameters.bMultiGPU_AfterDSI;
    std::cout << "params.oFinalSubPixelParameters.bMultiGPU_AfterDSI = " << bMultiGPU_AfterDSI << ";" << std::endl;

    bool bMedian = oFinalSubPixelParameters.bMedian;
    std::cout << "params.oFinalSubPixelParameters.bMedian = " << bMedian << ";" << std::endl;
    int iMedX = oFinalSubPixelParameters.iMedX;
    std::cout << "params.oFinalSubPixelParameters.iMedX = " << iMedX << ";" << std::endl;
    int iMedY = oFinalSubPixelParameters.iMedY;
    std::cout << "params.oFinalSubPixelParameters.iMedY = " << iMedY << ";" << ";" << std::endl;

    bool bSpeckle = oFinalSubPixelParameters.bSpeckle;
    std::cout << "params.oFinalSubPixelParameters.bSpeckle = " << bSpeckle << ";" << std::endl;

    int iNodataPol = oFinalSubPixelParameters.iNodataPol;
    std::cout << "params.oFinalSubPixelParameters.iNodataPol = " << iNodataPol << ";" << std::endl;
    int iBorderPol = oFinalSubPixelParameters.iBorderPol;
    std::cout << "params.oFinalSubPixelParameters.iBorderPol = " << iBorderPol << ";" << std::endl;
    float fMedianThresh = oFinalSubPixelParameters.fMedianThresh;    
    std::cout << "params.oFinalSubPixelParameters.fMedianThresh = " << fMedianThresh << ";" << std::endl;
    bool bSplittedMedian = oFinalSubPixelParameters.bSplittedMedian;
    std::cout << "params.oFinalSubPixelParameters.bSplittedMedian = " << bSplittedMedian << ";" << std::endl;

    int iSpeckleMaxSize = oFinalSubPixelParameters.iSpeckleMaxSize;
    std::cout << "params.oFinalSubPixelParameters.iSpeckleMaxSize = " << iSpeckleMaxSize << ";" << std::endl;
    float fSpeckleMaxDiff = oFinalSubPixelParameters.fSpeckleMaxDiff;
    std::cout << "params.oFinalSubPixelParameters.fSpeckleMaxDiff = " << fSpeckleMaxDiff << ";" << std::endl;
    //back matching parameters:
    bool bBackMatch = oFinalSubPixelParameters.bBackMatch;
    std::cout << "params.oFinalSubPixelParameters.bBackMatch = " << bBackMatch << ";" << std::endl;
    bool bComputeBackDisparity = oFinalSubPixelParameters.bComputeBackDisparity;
    std::cout << "params.oFinalSubPixelParameters.bComputeBackDisparity = " << bComputeBackDisparity << ";" << std::endl;

    float fMaxBackMatchingDist = oFinalSubPixelParameters.fMaxBackMatchingDist;
    std::cout << "params.oFinalSubPixelParameters.fMaxBackMatchingDist = " << fMaxBackMatchingDist << ";" << std::endl;
    bool bKeepValueOnNodataInBackmatch = oFinalSubPixelParameters.bKeepValueOnNodataInBackmatch;
    std::cout << "params.oFinalSubPixelParameters.bKeepValueOnNodataInBackmatch = " << bKeepValueOnNodataInBackmatch << ";" << std::endl;
    bool bKeepValueOnOutsideImage = oFinalSubPixelParameters.bKeepValueOnOutsideImage;
    std::cout << "params.oFinalSubPixelParameters.bKeepValueOnOutsideImage = " << bKeepValueOnOutsideImage << ";" << std::endl;

    bool bOcclusionDetection = oFinalSubPixelParameters.bOcclusionDetection;
    std::cout << "params.oFinalSubPixelParameters.bOcclusionDetection = " << bOcclusionDetection << ";" << std::endl;
    float fOcclusionDispGapSize = oFinalSubPixelParameters.fOcclusionDispGapSize;
    std::cout << "params.oFinalSubPixelParameters.fOcclusionDispGapSize = " << fOcclusionDispGapSize << ";" << std::endl;
    int nRegionGrowingMode = oFinalSubPixelParameters.nRegionGrowingMode;
    std::cout << "params.oFinalSubPixelParameters.nRegionGrowingMode = " << nRegionGrowingMode << ";" << std::endl;
    int nRegionGrowingIterations = oFinalSubPixelParameters.nRegionGrowingIterations;
    std::cout << "params.oFinalSubPixelParameters.nRegionGrowingIterations = " << nRegionGrowingIterations << ";" << std::endl;

    bool bInterpol = oFinalSubPixelParameters.bInterpol;
    std::cout << "params.oFinalSubPixelParameters.bInterpol = " << bInterpol << ";" << std::endl;
    int nDir = oFinalSubPixelParameters.nDir;
    std::cout << "params.oFinalSubPixelParameters.nDir = " << nDir << ";" << std::endl;
    int nNth = oFinalSubPixelParameters.nNth;
    std::cout << "params.oFinalSubPixelParameters.nNth = " << nNth << ";" << std::endl;
    int nMinNth = oFinalSubPixelParameters.nMinNth;
    std::cout << "params.oFinalSubPixelParameters.nMinNth = " << nMinNth << ";" << std::endl;
    float fRadius = oFinalSubPixelParameters.fRadius;     
    std::cout << "params.oFinalSubPixelParameters.fRadius = " << fRadius << ";" << std::endl;
    int nMode = oFinalSubPixelParameters.nMode;
    std::cout << "params.oFinalSubPixelParameters.nMode = " << nMode << ";" << std::endl;

    bool bOccInterpol = oFinalSubPixelParameters.bOccInterpol;                      
    std::cout << "params.oFinalSubPixelParameters.bOccInterpol = " << bOccInterpol << ";" << std::endl;
    int nOccDir = oFinalSubPixelParameters.nOccDir; 
    std::cout << "params.oFinalSubPixelParameters.nOccDir = " << nOccDir << ";" << std::endl;
    int nOccNth = oFinalSubPixelParameters.nOccNth;        
    std::cout << "params.oFinalSubPixelParameters.nOccNth = " << nOccNth << ";" << std::endl;
    int nOccMinNth = oFinalSubPixelParameters.nOccMinNth;
    std::cout << "params.oFinalSubPixelParameters.nOccMinNth = " << nOccMinNth << ";" << std::endl;
    float fOccRadius = oFinalSubPixelParameters.fOccRadius;              
    std::cout << "params.oFinalSubPixelParameters.fOccRadius = " << fOccRadius << ";" << std::endl;
    int nOccMode = oFinalSubPixelParameters.nOccMode;     
    std::cout << "params.oFinalSubPixelParameters.nOccMode = " << nOccMode << ";" << std::endl;

    int nDebugLevel = params.nDebugLevel;
    std::cout << "params.nDebugLevel = " << nDebugLevel << ";" << std::endl;
    std::string  strDebugOutput = params.strDebugOutput;
    std::cout << "params.strDebugOutput = \"" << strDebugOutput << "\";" << std::endl;
}

extern "C" {
    I3DRSGM_EXPORTS void cvinout(
            int rows_in, int cols_in, const int cvtype_in, void *buffer_in, 
            int rows_out, int cols_out, const int cvtype_out, void *buffer_out
        ) {
        cv::Mat img_in(cv::Size(rows_in, cols_in), cvtype_in, buffer_in);
        cv::Mat img_out(cv::Size(rows_out, cols_out), cvtype_out, buffer_out);
        img_in.convertTo(img_out, cvtype_out);
    }

    I3DRSGM_EXPORTS bool TestLicense()
    {
         bool valid = I3DRSGM::isLicenseValid();
         return valid;
    }
}
