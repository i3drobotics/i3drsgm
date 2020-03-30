#ifndef MATCHERJRSGM_H
#define MATCHERJRSGM_H

#include <abstractstereomatcher.h>
#include <QDir>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define JR_IMPORTS
#include <DigVTKIntegration.hpp>

//!  Matcher JRSGM
/*!
  Stereo matcher using JR's SGM algorithm
*/

class MatcherJRSGM : public AbstractStereoMatcher {
  Q_OBJECT
 public:
  explicit MatcherJRSGM(QObject *parent = 0,
                           cv::Size image_size = cv::Size(0, 0))
      : AbstractStereoMatcher(parent, image_size) {
    init();
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

  double getP1(void){ return oParams.oPyramidParams[0].oSGMParams.fP1_E_W; }
  double getP2(void){ return oParams.oPyramidParams[0].oSGMParams.fP2_E_W; }
  int getDisparityRange(void){ return oParams.oPyramidParams[0].nMaximumNumberOfDisparities; }
  int getCensusSize(void){ return oParams.oPyramidParams[0].oMetricParams.nWindowSizeX; }
  bool getInterpolate(void){return oParams.oFinalSubPixelParameters.bInterpol; }
  bool getOcclusionDetect(void){ return oParams.oFinalSubPixelParameters.bOcclusionDetection; }
  bool getSubpixel(void){return oParams.oFinalSubPixelParameters.bCompute; }
  int getDisparityShift(void){return oParams.fTopPredictionShift * pow(2, oParams.nNumberOfPyramids-1) ; }

  void readConfig(const std::string &sConfigFile, SMatchingParametersInput &oParams);
  void writeConfig(const std::string &sConfigFile, const SMatchingParametersInput &oParams);

 private:
  void init(void);
  void initParameters(int nPyramids = 3);
  void initSGMParameters(void);

  SMatchingParametersInput oParams;
  SSGMParamsetersIn oSGMParams;
  SMatchingParametersPyramid oSubPix;
};

#endif  // MATCHERJRSGM_H