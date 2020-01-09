#include "matcherjrsgm3.h"

void MatcherJRSGM::init(){
    /*
    // Load default config
    loadParameters();

    enableInterpolation(false);
    enableOcclusionDetection(false);
    setWindowSize(9);

    min_disparity = 0;
    disparity_range = 65;

    //setDisparityRange(21);
    //setMatchCosts(0.5, 1.5);
    */
    const std::string sParamFile =  QCoreApplication::applicationDirPath().toStdString() + "/param/match.param";
    JR::Phobos::ReadIniFile(params,sParamFile);

    matcher_handle = JR::Phobos::CreateMatchStereoHandle(params);

}

void MatcherJRSGM::saveParameters(void) {
    writeConfig(config_path);
}

void MatcherJRSGM::loadParameters(void) {
    if (QFile(config_path).exists()){
        qDebug() << "Config file found";
        readConfig(config_path);
    }else{
        qDebug() << "Failed to open config file";
    }
}

int MatcherJRSGM::getErrorDisparity(void){
    return -10000;
}

void MatcherJRSGM::backwardMatch()
{
    cv::Mat left_joint, right_joint;
    std::string sgm_log = "./sgm_log.txt";

    JR::Phobos::MatchStereo(matcher_handle,
                            *right,
                            *left,
                            left_joint,
                            right_joint,
                            disparity_rl,
                            sgm_log,
                            JR::Phobos::e_logLog);
}

void MatcherJRSGM::forwardMatch()
{
    cv::Mat left_joint, right_joint;
    std::string sgm_log = "./sgm_log.txt";

    JR::Phobos::MatchStereo(matcher_handle,
                            *left,
                            *right,
                            left_joint,
                            right_joint,
                            disparity_lr,
                            sgm_log,
                            JR::Phobos::e_logLog);
}

void MatcherJRSGM::setMatchCosts(float P1, float P2) {

  for (auto &pyramid : params.oPyramidParams) {
      pyramid.oSGMParams.fP1_E_W = P1;
      pyramid.oSGMParams.fP1_SE_NW = P1;
      pyramid.oSGMParams.fP1_SW_NE = P1;
      pyramid.oSGMParams.fP1_S_N = P1;

      pyramid.oSGMParams.fP2_E_W = P2;
      pyramid.oSGMParams.fP2_SE_NW = P2;
      pyramid.oSGMParams.fP2_SW_NE = P2;
      pyramid.oSGMParams.fP2_S_N = P2;
  }

  setConfig();
}

void MatcherJRSGM::setWindowSize(int census_size) {
  for (auto &pyramid : params.oPyramidParams) {
    pyramid.oMetricParams.nWindowSizeX = census_size;
    pyramid.oMetricParams.nWindowSizeY = census_size;
  }

  setConfig();
}


void MatcherJRSGM::setDisparityShift(int shift) {
  params.fTopPredictionShift = shift / pow(2, params.nNumberOfPyramids - 1);
  min_disparity = shift;

  setConfig();
}

void MatcherJRSGM::enableSubpixel(bool enable) {
  params.oFinalSubPixelParameters.bCompute = enable;

  setConfig();
}

void MatcherJRSGM::setDisparityRange(int n) {
  /* Set disparity range for all pyramids */
  if (n % 2) {
    disparity_range = n;
    params.oPyramidParams[0].nMaximumNumberOfDisparities = n;
    params.oPyramidParams[1].nMaximumNumberOfDisparities = n;
    params.oPyramidParams[2].nMaximumNumberOfDisparities = n;
    params.oPyramidParams[3].nMaximumNumberOfDisparities = n;
    params.oPyramidParams[4].nMaximumNumberOfDisparities = n;

    params.oFinalSubPixelParameters.nMaximumNumberOfDisparities = n;
  }

  setConfig();
}

void MatcherJRSGM::enableInterpolation(bool enable) {
  /* Toggle interpolation */
  for (auto &pyramid : params.oPyramidParams) {
    pyramid.bInterpol = enable;
  }

  params.oFinalSubPixelParameters.bInterpol = enable;

  setConfig();
}

void MatcherJRSGM::enableOcclusionDetection(bool enable) {
  /* Toggle occlusion detection */
  for (auto &pyramid : params.oPyramidParams) {
    pyramid.bOcclusionDetection = enable;
  }

  params.oFinalSubPixelParameters.bOcclusionDetection = enable;

  setConfig();
}

void MatcherJRSGM::readConfig(QString &sConfigFile){
    /* Load a configuration file for the matcher */

    QSettings* settings = new QSettings(sConfigFile, QSettings::IniFormat);

    settings->beginGroup("Input");
    params.oGPUs = std::vector<int>(1, 0);
    params.bHasNodata = settings->value("Input Has Nodata", false).toBool();
    params.fNodataValue = settings->value("Nodata Value", -99999.000000).toFloat();
    settings->endGroup();

    settings->beginGroup("Output");
    params.bWriteBackDisp = settings->value("Write Back Disparity", false).toBool();
    params.bWriteRevBackMatchDistance = settings->value("Write Reverse Back Match Distance", false).toBool();
    params.bWriteBackMatchDistance = settings->value("Write Back Match Distance", false).toBool();
    settings->endGroup();

    settings->beginGroup("Parameter");
    params.nNumberOfPyramids = settings->value("Pyramid Levels", 5).toInt();
    params.strDispPredictionFile = settings->value("Back Disparity Prediction Input File Pattern", "").toString().toStdString();
    params.strBackDispPredictionFile = settings->value("Disparity Prediction Input File Pattern", "").toString().toStdString();
    params.fTopPredictionShift = settings->value("Top Prediction Shift", 0).toFloat();
    params.nNumberOfMorphologicalIter = settings->value("Morphological Iterations", 0).toInt();
    params.nDebugLevel = settings->value("Debug Output Mode", 0).toInt();
    params.strDebugOutput = settings->value("Debug Output Path", "./").toString().toStdString();
    settings->endGroup();

    params.oPyramidParams.resize(static_cast<size_t>(params.nNumberOfPyramids));

    for(int i=0; i < params.nNumberOfPyramids; i++){
        auto group = QString("Pyramid %1").arg(QString::number(i));
        params.oPyramidParams.at(i) = parsePyramid(settings, group);
    }

    auto group = QString("Pyramid %1").arg(QString::number(params.nNumberOfPyramids));
    params.oFinalSubPixelParameters = parsePyramid(settings, group);

}

JR::Phobos::SMatchingParameters MatcherJRSGM::parsePyramid(QSettings *settings, QString group){
    /* Load in matching parameters for a particular pyramid */
    settings->beginGroup(group);

    JR::Phobos::SMatchingParameters pyramid;

    pyramid.bCompute = settings->value("Process This Pyramid", true).toBool();
    pyramid.nMetric = 0; // Census cost
    pyramid.nMaximumNumberOfDisparities = settings->value("Number Of Disparities", 31).toInt();
    pyramid.fDisparityStepSize = settings->value("Disparity Step Size", 0.5).toFloat();
    pyramid.nSubpixelFnc = 2;
    pyramid.fMaxCost = settings->value("Maximum Feature Distance", 6).toFloat();
    pyramid.bDoSubpixelNormalization = settings->value("Do Subpixel Normalization", true).toBool();
    pyramid.bBackMatch = settings->value("Compute Backmatching", false).toBool();
    pyramid.bComputeBackDisparity = settings->value("Compute Back Disparity", false).toBool();

    pyramid.fMaxBackMatchingDist = settings->value("Maximum Backmatching Distance", 1.5).toFloat();
    pyramid.bKeepValueOnNodataInBackmatch = settings->value("Maximum Backmatching Distance - Keep Value On Nodata In Backmatch", false).toBool();
    pyramid.bKeepValueOnOutsideImage = settings->value("Maximum Backmatching Distance - Keep Value On Outside Image In Backmatch", false).toBool();
    pyramid.bMultiGPU_AfterDSI = settings->value("MultiGPU Implementation After DSI", false).toBool();

    pyramid.nRegionGrowingMode = 1;
    pyramid.nRegionGrowingIterations = 1;

    // manually added from GUI
    pyramid.bMedian = settings->value("Disparity Median Optimizer", true).toBool();  // use median optimizer
    pyramid.iMedX = settings->value("Disparity Median Optimizer Kernel Size X", 3).toInt();
    pyramid.iMedY = settings->value("Disparity Median Optimizer Kernel Size Y", 3).toInt();
    pyramid.iNodataPol = 1;      //"Disparity Median Optimizer Nodata Policy"
    pyramid.iBorderPol = 2;      //"Disparity Median Optimizer Border Policy"
    pyramid.fMedianThresh = settings->value("Disparity Median Optimizer Threshold", -1).toFloat();
    pyramid.bSplittedMedian = settings->value("Disparity Median Optimizer Splitted Version", false).toBool();  //"Disparity Median Optimizer Splitted Version"

    pyramid.bSpeckle = settings->value("Disparity Speckle Filter Optimizer", true).toBool();      // use speckle optimizer
    pyramid.iSpeckleMaxSize = settings->value("Disparity Speckle Filter Max Region Size", 200).toInt();  //"Disparity Speckle Filter Max Region Size"
    pyramid.fSpeckleMaxDiff = settings->value("Disparity Speckle Filter Max Difference", 0.5).toFloat();  //"Disparity Speckle Filter Max Difference"

    pyramid.bInterpol = settings->value("Interpolate Disparity", true).toBool();    // "Interpolate Disparity"
    pyramid.nDir = settings->value("Interpolator Number Of Directions", 32).toInt();            // "Interpolator Number Of Directions"
    pyramid.nNth = settings->value("Interpolator Number Of Nth Elements", 32).toInt();            // "Interpolator Number Nth Elements"
    pyramid.nMinNth = settings->value("Interpolator Minimum Number Of Elements", 0).toInt();         // "Interpolator Minimum Number Of Elements"
    pyramid.fRadius = settings->value("Match Radius", -1).toInt();         // "Interpolator Match Radius"
    pyramid.nMode = 2;        // "Interpolator Mode" (Gauss);

    pyramid.bOcclusionDetection = settings->value("Occlusion Detection", true).toBool();
    pyramid.fOcclusionDispGapSize = settings->value("Occlusion Detector Step Size", 1.1).toFloat();
    pyramid.bOccInterpol = settings->value("Interpolate Occlusions", true).toBool();  // "Interpolate Occlusions"
    pyramid.nOccDir = settings->value("Occlusion Interpolator Number Of Directions", 32).toInt();  // "Occlusion Interpolator Number Of Directions"
    pyramid.nOccNth = settings->value("Occlusion Interpolator Number Nth Elements", 32).toInt();  // "Occlusion Interpolator Number Nth Elements"
    pyramid.nOccMinNth = settings->value("Occlusion Interpolator Minimum Number Of Elements", 0).toInt();
    pyramid.fOccRadius = settings->value("Occlusion Interpolator Match Radius", -1).toFloat();  // "Occlusion Interpolator Match Radius"
    pyramid.nOccMode = 2;     // "Occlusion Interpolator Mode" (Percentile 10)

    pyramid.nRegionGrowingMode = 1;      //0 no, 1 iterations, 2//full
    pyramid.nRegionGrowingIterations = settings->value("Occlusion Region Growing Iterations", 5).toInt();

    pyramid.oMetricParams.nWindowSizeX = settings->value("Feature Set Size X", 9).toInt();
    pyramid.oMetricParams.nWindowSizeY = settings->value("Feature Set Size Y", 9).toInt();
    pyramid.oMetricParams.fMetricDeltaX = settings->value("Feature Set Delta Step Size X", 1).toFloat();
    pyramid.oMetricParams.fMetricDeltaY = settings->value("Feature Set Delta Step Size Y", 1).toFloat();
    pyramid.oMetricParams.nCensusCenterFunction = 1; // Census Center Function
    pyramid.oMetricParams.nNumberOfScales = settings->value("Feature Set Slanted Window Number Of Scales", 0).toInt();
    pyramid.oMetricParams.fScaleStepSize = settings->value("Feature Set Slanted Window Shift Step Size", 0.250000).toFloat();
    pyramid.oMetricParams.nNumberOfSlants = settings->value("Feature Set Slanted Window Number Of Shifts", 0).toInt();
    pyramid.oMetricParams.fSlantStepSize = settings->value("Feature Set Slanted Window Slant Step Size", 0.250000).toFloat();
    pyramid.oMetricParams.bUseProximityWeight = 0;
    pyramid.oMetricParams.fProximityGamma = settings->value("Feature Set Proximity Weight", 2).toFloat();
    pyramid.oMetricParams.bUseSimilarityWeight = 0;
    pyramid.oMetricParams.fSimilarityGamma = settings->value("Feature Set Similarity Weight", 2).toFloat();

    JR::Phobos::SSGMParameters oSGMParams;

    oSGMParams.nAllowedOutsideImageMatchCount = settings->value("DSI Allowed Outside Image Disparities", 2147483647).toInt();
    oSGMParams.bDSICountCorrection = settings->value("DSI Count Correction", true).toBool();
    oSGMParams.nDSICountCorrectionNoDataCheckMode = 1;

    // Up/Down
    oSGMParams.bMatch_S_N = settings->value("SGM South-North Optimization", true).toBool();
    oSGMParams.fP1_S_N = settings->value("SN Penalty 1", 0.100000f).toFloat();
    oSGMParams.fP2_S_N = settings->value("SN Penalty 2", 0.800000f).toFloat();
    oSGMParams.bP2EqualP1_S_N = 0;
    oSGMParams.nAdaptiveP1Fnc_S_N = 0;
    oSGMParams.nAdaptiveP2Fnc_S_N = 0;
    oSGMParams.bAdditive_S_N = settings->value("SN Additive", true).toBool();

    // Left/Right
    oSGMParams.bMatch_E_W = settings->value("SGM West-East Optimization", true).toBool();
    oSGMParams.fP1_E_W = settings->value("WE Penalty 1", 0.1).toFloat();
    oSGMParams.fP2_E_W = settings->value("WE Penalty 1", 0.8).toFloat();
    oSGMParams.bP2EqualP1_E_W = 0;
    oSGMParams.nAdaptiveP1Fnc_E_W = 0; // None
    oSGMParams.nAdaptiveP2Fnc_E_W = 0; // Inverse gradient
    oSGMParams.bAdditive_E_W = settings->value("WE Additive", true).toBool();

    // Diagonal
    oSGMParams.bMatch_SW_NE = settings->value("SGM SouthWest-NorthEast Optimization", true).toBool();
    oSGMParams.fP1_SW_NE = settings->value("SW-NE Penalty 1", 0.1).toFloat();
    oSGMParams.fP2_SW_NE = settings->value("SW-NE Penalty 2", 0.8).toFloat();
    oSGMParams.bP2EqualP1_SW_NE = 0;
    oSGMParams.nAdaptiveP1Fnc_SW_NE = 0;
    oSGMParams.nAdaptiveP2Fnc_SW_NE = 0;
    oSGMParams.bAdditive_SW_NE = settings->value("SW-NE Additive", true).toBool();

    // Diagonal
    oSGMParams.bMatch_SE_NW = settings->value("SGM SouthEast-NorthWest Optimization", true).toBool();
    oSGMParams.fP1_SE_NW = settings->value("SE-NW Penalty 1", 0.1).toFloat();
    oSGMParams.fP2_SE_NW = settings->value("SE-NW Penalty 2", 0.8).toFloat();
    oSGMParams.bP2EqualP1_SE_NW = 0;
    oSGMParams.nAdaptiveP1Fnc_SE_NW = 0;
    oSGMParams.nAdaptiveP2Fnc_SE_NW = 0;
    oSGMParams.bAdditive_SE_NW = settings->value("SE-NW Additive", true).toBool();

    oSGMParams.bUseDSITexture = settings->value("Use DSI Texture Memory", false).toBool();
    oSGMParams.bUseAggregatedCostTexture = settings->value("Use Aggregated Cost Texture Memory", true).toBool();

    pyramid.oSGMParams = oSGMParams;
    settings->endGroup();

    return pyramid;
}


void MatcherJRSGM::writeConfig(QString &sConfigFile) {
    //TODO
}

void MatcherJRSGM::setConfig(){
    /* Call to update the current configuration */
    if(matcher_handle != nullptr){
        JR::Phobos::DestroyMatchStereoHandle(matcher_handle);
    }

    matcher_handle = JR::Phobos::CreateMatchStereoHandle(params);
}
