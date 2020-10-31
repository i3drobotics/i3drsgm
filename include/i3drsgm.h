#ifndef I3DRSGM_H
#define I3DRSGM_H

/**
 * @file i3drsgm.h
 * @authors Ben Knight (bknight@i3drobotics.com)
 * @brief
 * @version 1.0.0
 * @date 2020-07-07
 *
 * @copyright Copyright (c) I3D Robotics Ltd, 2020
 *
 */

#include "i3drsgmExport.h"
#include <PhobosIntegration/PhobosIntegration.hpp>
#include <iostream>
#include <fstream>
#include <mutex>          // std::mutex

#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#elif _linux__
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

//!  I3DRSGM
/*!
  Stereo matcher using I3DR's SGM algorithm
*/

class I3DRSGM_EXPORTS I3DRSGM 
{
public:
    //! Constructor
    /*! Matcher requires folder location to save a temperary parameter file
     *  and a parameter file to define the settings to use in the matcher.
     * \param tmp_param_file filepath where temporary file is saved.
     * \param param_file filepath to load parameter file.
    */
    I3DRSGM(std::string tmp_param_file, std::string param_file);

    //! Deconstructor
    /*! Clean up matcher handle */
    ~I3DRSGM(void){
        if (matcher_handle != nullptr){
            JR::Phobos::DestroyMatchStereoHandle(matcher_handle);
        }
    }

    //! Get hostid and hostname (useful for displaying to user)
    /*! \param hostname string variable to place hostname */
    /*! \param hostid string varaible to place hostid */
    static void getHostInfo(std::string & hostname, std::string & hostid){
        JR::Phobos::GetHostInfo(hostname,hostid);
    }

    //! Check if license is valid
    /*! 
        Looks for license file in the same folder as the application.
        Checks license against hostinfo and returns a valid matcher handle,
        if the license is valid. This matcher handle is checked to see if it
        is null, this is the check returned by the function.
    */
    /*! \return license validitiy */
    static bool isLicenseValid()
    {
        //checks license is by creating temperary matcher handle
        JR::Phobos::TSTEREOHANDLE tmp_matcher_handle = nullptr;
        JR::Phobos::SMatchingParametersInput tmp_params = JR::Phobos::SMatchingParametersInput();
        tmp_matcher_handle = JR::Phobos::CreateMatchStereoHandle(tmp_params);
        return (tmp_matcher_handle != nullptr);
    }

    //! Get error disparity
    /*! \return value used to represent a match error */
    int getErrorDisparity();
    //! Create matcher handle
    /*! \return success of matcher creation (0 = success / -1 = fail) */
    int createMatcher();

    //! Set disparity shift
    /*! \param shift define the disparity shift */
    void setDisparityShift(int shift);
    //! Set disparity range
    /*! \param n number of pixels of disparity range */
    void setDisparityRange(int n);
    //! Enable subpixel
    /*! \param enable use subpixel pyramid */
    void enableSubpixel(bool enable);
    //! Set penalty 1
    /*! \param P1 set penalty 1 value (between 0 - 1) */
    void setP1(float P1);
    //! Set penalty 2
    /*! \param P2 set penalty 2 value (between 0 - 1) */
    void setP2(float P2);
    //! Set match costs (shortcut for running setP1 and setP2)
    /*! \param P1 set penalty 1 value (between 0 - 1) */
    /*! \param P2 set penalty 2 value (between 0 - 1) */
    void setMatchCosts(float P1, float P2);
    //! Set window size
    /*! \param census_size size of search window */
    void setWindowSize(int census_size);
    //! Set speckle difference
    /*! \param diff speckle filter difference */
    void setSpeckleDifference(float diff);
    //! Set speckle size
    /*! \param size speckle filter search window size */
    void setSpeckleSize(int size);
    //! Enable interpolation
    /*! \param enable use interpolation */
    void enableInterpolation(bool enable);
    //! Enable occlusion detection
    /*! \param enable use occlusion detection */
    void enableOcclusionDetection(bool enable);
    //! Enable occlusion interpolation
    /*! \param enable use occlusion interpolation */
    void enableOccInterpol(bool enable);
    //! Enable texture DSI
    /*! \param enable use texture DSI */
    void enableTextureDSI(bool enable);
    //! Enable a pyramid
    /*! \param enable turn pyramid on or off */
    /*! \param pyramid_num pyramid index to set */
    void enablePyramid(bool enable, int pyramid_num);
    //! Maximum pyramid
    /*! \param pyramid_num turn off all pyramids above this index */
    void maxPyramid(int pyramid_num);

    //! Get penalty 1
    /*! \return value of penalty 1 */
    float getP1(void){ return params.oPyramidParams[0].oSGMParams.fP1_E_W; }
    //! Get penalty 2
    /*! \return value of penalty 2 */
    float getP2(void){ return params.oPyramidParams[0].oSGMParams.fP2_E_W; }
    //! Get disparity range
    /*! \return value of disparity range */
    int getDisparityRange(void){ return params.oPyramidParams[0].nMaximumNumberOfDisparities; }
    //! Get census size
    /*! \return search window size for matcher */
    int getCensusSize(void){ return params.oPyramidParams[0].oMetricParams.nWindowSizeX; }
    //! Get interpolate
    /*! \return if interpolation is being used */
    bool getInterpolate(void){return params.oFinalSubPixelParameters.bInterpol; }
    //! Get occlusion detection
    /*! \return if occulusion detection is being used */
    bool getOcclusionDetect(void){ return params.oFinalSubPixelParameters.bOcclusionDetection; }
    //! Get subpixel
    /*! \return if subpixel pyramid is being used */
    bool getSubpixel(void){return params.oFinalSubPixelParameters.bCompute; }
    //! Get disparity shift
    /*! \return disparity shift value */
    int getDisparityShift(void){return params.fTopPredictionShift * pow(2, params.nNumberOfPyramids-1) ; }
    //! Get matcher status
    /*! \return status of matcher (0 == success / -1 == fail) */
    int getStatus();

    //! Set disparity error value
    /*! \param value to use to represent a disparity error */
    void setDisparityError(int val);
    //! Enable CPU
    /*! \param enable CPU matcher mode */
    void enableCPU(bool enable);

    //! Forward match
    /*! Calculate disparity of right image from left image
     * \param left image
     * \param right image
     * \return disparity image */
    cv::Mat forwardMatch(cv::Mat left, cv::Mat right);
    //! Backward match
    /*! Calculate disparity of left image from right image
     * \param left image
     * \param right image
     * \return disparity image */
    cv::Mat backwardMatch(cv::Mat left, cv::Mat right);

    static std::string getexepath();

private:
    //! Matcher handle
    /*! TSTEREOHANDLE from JR library */
    JR::Phobos::TSTEREOHANDLE matcher_handle = nullptr;
    //! Parameters for matcher
    /*! SMatchingParametersInput from JR library */
    JR::Phobos::SMatchingParametersInput params;

    //! Minimum disparity for matcher
    int min_disparity;
    //! Disparity range for matcher
    int disparity_range;
    //! Matcher status (0 == valid / -1 == invalid)
    int matcher_status = 0;
    //! Top pyramid to use in matcher
    int top_pyramid;

    //! Mutex to avoid variable collision
    std::mutex mtx;

    //! Matcher paramters is a vector of strings
    std::vector<std::string> params_raw;
    //! Filepath to temperary parameter file
    std::string tmp_param_file_;

    //! Read text file
    /*! Each line in file to vector of strings
     * \param sConfigFile filepath to read
     * \return file data as vector of string (each line is a string) */
    std::vector<std::string> ReadFileRaw(std::string &sConfigFile);
    //! Write text file
    /*! Vector of strings where each string is line to chosen file
     * \param filename filepath to write to
     * \param lines vector of string to write to file (each line is a string) */
    void WriteIniFileRaw(std::string &filename, std::vector<std::string> lines);
    //! Edit text file line
    /*! Edit a line from a text file represented as vector of strings
     * \param lines pointer to data from text file as vector of strings
     * \param value data to replace the line with
     * \param line index in file to replace */
    void EditLineRaw(std::vector<std::string> *lines, std::string value, int line_num);
    //! Edit parameter
    /*! Edit a parameter from a text file represented as vector of strings
     * \param lines pointer to data from text file as vector of strings
     * \param param_name parameter to search the document for
     * \param param_value paramter to set the value to */
    bool EditParamRaw(std::vector<std::string> *lines, std::string param_name,std::string param_value);
    //! Edit parameter for pyramid
    /*! Edit a parameter for a chosen pyramid in a text file represented as vector of strings
     * \param lines pointer to data from text file as vector of strings
     * \param pyramid_num pyramid number to set parameter inside
     * \param param_name parameter to search the document for
     * \param param_value paramter to set the value to
     * \param is_subpix is using subpixel in pyramid selection */
    bool EditPyramidParamRaw(std::vector<std::string> *lines, int pyramid_num,std::string param_name,std::string param_value,bool is_subpix=false);

};

// extern "C" {

//     I3DRSGM_EXPORTS const char* GetExePath(void);
//     I3DRSGM_EXPORTS bool TestLicense(void);
//     //I3DRSGM_EXPORTS cv::Mat* TestMatch(unsigned char* left_ptr, unsigned char* right_ptr ,int height, int width, int type);
//     I3DRSGM_EXPORTS cv::Mat* MatchFilename(I3DRSGM* matcher_ptr, char* left_filename, char* right_filename);
//     I3DRSGM_EXPORTS I3DRSGM* CreateMatcher();
//     I3DRSGM_EXPORTS cv::Mat* MatchBMFilename(cv::StereoBM* matcher_ptr, char* left_filename, char* right_filename);
//     I3DRSGM_EXPORTS cv::StereoBM* CreateMatcherBM();
// }

#endif  // I3DRSGM_H