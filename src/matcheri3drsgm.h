#ifndef MATCHERI3DRSGM_H
#define MATCHERI3DRSGM_H

#include <abstractstereomatcher.h>
#include <QDir>
#include <QDebug>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <PhobosIntegration/PhobosIntegration.hpp>
#include <iostream>
#include <fstream>
#include <mutex>          // std::mutex

//!  Matcher I3DRSGM
/*!
  Stereo matcher using I3DR's SGM algorithm
*/

class MatcherI3DRSGM : public AbstractStereoMatcher {
    Q_OBJECT
public:
    explicit MatcherI3DRSGM(QObject *parent = 0,
                            cv::Size image_size = cv::Size(0, 0))
        : AbstractStereoMatcher(parent, image_size) {
        init();
    }

    ~MatcherI3DRSGM(void){
        if (matcher_handle != nullptr){
            JR::Phobos::DestroyMatchStereoHandle(matcher_handle);
        }
    }

    void parseConfig(std::string input_file);
    int getErrorDisparity();
    int createMatcher();

    void setDisparityShift(int shift);
    void setDisparityRange(int n);
    void enableSubpixel(bool enable);
    void setP1(float P1);
    void setP2(float P2);
    void setMatchCosts(float P1, float P2);
    void setWindowSize(int census_size);
    void setSpeckleDifference(float diff);
    void setSpeckleSize(int size);
    void enableInterpolation(bool enable);
    void enableOcclusionDetection(bool enable);
    void enableOccInterpol(bool enable);
    void enableTextureDSI(bool enable);
    void enablePyramid(bool enable, int pyramid_num);
    void maxPyramid(int pyramid_num);

    float getP1(void){ return params.oPyramidParams[0].oSGMParams.fP1_E_W; }
    float getP2(void){ return params.oPyramidParams[0].oSGMParams.fP2_E_W; }
    int getDisparityRange(void){ return params.oPyramidParams[0].nMaximumNumberOfDisparities; }
    int getCensusSize(void){ return params.oPyramidParams[0].oMetricParams.nWindowSizeX; }
    bool getInterpolate(void){return params.oFinalSubPixelParameters.bInterpol; }
    bool getOcclusionDetect(void){ return params.oFinalSubPixelParameters.bOcclusionDetection; }
    bool getSubpixel(void){return params.oFinalSubPixelParameters.bCompute; }
    int getDisparityShift(void){return params.fTopPredictionShift * pow(2, params.nNumberOfPyramids-1) ; }

    void setNoDataValue(int val);
    void enableCPU(bool enable);

    void forwardMatch();
    void backwardMatch();

    int getStatus();

private:
    JR::Phobos::TSTEREOHANDLE matcher_handle = nullptr;
    JR::Phobos::SMatchingParametersInput params;
    int min_disparity, disparity_range;

    int matcher_status = 0;
    int top_pyramid;

    std::mutex mtx;

    void init();
    std::vector<std::string> ReadFileRaw(std::string &sConfigFile);
    void WriteIniFileRaw(std::string &filename, std::vector<std::string> lines);
    void EditLineRaw(std::vector<std::string> *lines, std::string value, int line_num);
    bool EditParamRaw(std::vector<std::string> *lines, std::string param_name,std::string param_value);
    bool EditPyramidParamRaw(std::vector<std::string> *lines, int pyramid_num,std::string param_name,std::string param_value,bool is_subpix=false);

    std::vector<std::string> params_raw;
    std::string tmp_param_file;
};

#endif  // MATCHERI3DRSGM_H
