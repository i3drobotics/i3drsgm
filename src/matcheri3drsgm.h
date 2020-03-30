#ifndef MATCHERI3DRSGM_H
#define MATCHERI3DRSGM_H

#include <abstractstereomatcher.h>
#include <QDir>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <PhobosIntegration/PhobosIntegration.hpp>
#include <iostream>
#include <fstream>

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

public slots:
    void forwardMatch(void);
    void backwardMatch(void);

    void enableSubpixel(bool enable);
    void enableInterpolation(bool enable);
    void enableOcclusionDetection(bool enable);
    void setWindowSize(int census_size);
    void setDisparityRange(int n);
    void setMatchCosts(double P1 = 0.5, double P2 = 1.5);
    void setDisparityShift(int shift);

    int getErrorDisparity(void);

    void saveParameters();
    void loadParameters();

    float getP1(void){ return 0; }
    float getP2(void){ return 0; }
    int getDisparityRange(void){ return 0; }
    int getCensusSize(void){ return 0; }
    bool getInterpolate(void){return false; }
    bool getOcclusionDetect(void){ return false; }
    bool getSubpixel(void){return false; }
    int getDisparityShift(void){return 0; }

    void readConfig(const std::string &sConfigFile, JR::Phobos::SMatchingParametersInput &oParams);
    void writeConfig(const std::string &sConfigFile, const JR::Phobos::SMatchingParametersInput &oParams);

private:
    void init(void);
    void initSGMParameters(void);
    void createMatcher();

    JR::Phobos::TSTEREOHANDLE matcher_handle = nullptr;
    JR::Phobos::SMatchingParametersInput params;
};

#endif  // MATCHERI3DRSGM_H
