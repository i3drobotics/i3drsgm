#include "matcherjrsgm.h"

void MatcherJRSGM::init(void) {
  initSGMParameters();
  initParameters(5);

  enableInterpolation(false);
  enableOcclusionDetection(false);
  setWindowSize(9);

  min_disparity = 0;
  disparity_range = 65;

  setDisparityRange(21);
  setMatchCosts(0.5, 1.5);
}

int MatcherJRSGM::getErrorDisparity(void){
    return -10000;
}

void MatcherJRSGM::saveParameters(void) {
  writeConfig("./params/jr_impact_config.json", oParams);
}

void MatcherJRSGM::loadParameters(void) {
  if (QFile("./params/jr_impact_config.json").exists())
    readConfig("./params/jr_impact_config.json", oParams);
  else
    qDebug() << "Failed to open config file";
}

void MatcherJRSGM::initSGMParameters() {
  oSGMParams.nAllowedOutsideImageMatchCount = 2147483647;
  oSGMParams.bDSICountCorrection = 1;
  oSGMParams.nDSICountCorrectionNoDataCheckMode = 1;

  // Up/Down
  oSGMParams.bMatch_S_N = 1;
  oSGMParams.fP1_S_N = 0.100000f;
  oSGMParams.fP2_S_N = 1.500000f;
  oSGMParams.bP2EqualP1_S_N = 0;
  oSGMParams.nAdaptiveP1Fnc_S_N = 0;
  oSGMParams.nAdaptiveP2Fnc_S_N = 0;
  oSGMParams.bAdditive_S_N = 1;

  // Left/Right
  oSGMParams.bMatch_E_W = 1;
  oSGMParams.fP1_E_W = 0.100000f;
  oSGMParams.fP2_E_W = 1.500000f;
  oSGMParams.bP2EqualP1_E_W = 0;
  oSGMParams.nAdaptiveP1Fnc_E_W = 0;
  oSGMParams.nAdaptiveP2Fnc_E_W = 0;
  oSGMParams.bAdditive_E_W = 1;

  // Diagonal
  oSGMParams.bMatch_SW_NE = 1;
  oSGMParams.fP1_SW_NE = 0.100000f;
  oSGMParams.fP2_SW_NE = 1.500000f;
  oSGMParams.bP2EqualP1_SW_NE = 0;
  oSGMParams.nAdaptiveP1Fnc_SW_NE = 0;
  oSGMParams.nAdaptiveP2Fnc_SW_NE = 0;
  oSGMParams.bAdditive_SW_NE = 1;

  // Diagonal
  oSGMParams.bMatch_SE_NW = 1;
  oSGMParams.fP1_SE_NW = 0.100000f;
  oSGMParams.fP2_SE_NW = 1.500000f;
  oSGMParams.bP2EqualP1_SE_NW = 0;
  oSGMParams.nAdaptiveP1Fnc_SE_NW = 0;
  oSGMParams.nAdaptiveP2Fnc_SE_NW = 0;
  oSGMParams.bAdditive_SE_NW = 1;

  oSGMParams.bUseDSITexture = 0;
  oSGMParams.bUseAggregatedCostTexture = 1;
}

void MatcherJRSGM::initParameters(int nPyramids) {
  oParams.bHasNodata = true;
  oParams.fNodataValue = -99999.000000;

  oParams.bWriteBackDisp = 0;
  oParams.bWriteRevBackMatchDistance = 0;
  oParams.strRevBackMatchDistanceOutputFile = "backmatch.tiff";
  oParams.bWriteBackMatchDistance = 0;
  oParams.strBackMatchDistanceOutputFile = "backmatch_dist";

  oParams.nNumberOfPyramids = nPyramids;
  oParams.nPredictor = 0;
  oParams.strDispPredictionFile = "";
  oParams.strBackDispPredictionFile = "";
  oParams.fTopPredictionShift = 0.000000;
  oParams.nAdaptivePrefilter = 0;
  oParams.nAdaptivePrefilterSize = 3;
  oParams.nNumberOfMorphologicalIter = 1;
  oParams.nDebugLevel = 0;
  oParams.strDebugOutput = "";
  oParams.oProcessAffinity = std::set<int>();
  oParams.oGPUs = std::vector<int>(1, 0);

  oParams.oPyramidParams.resize(oParams.nNumberOfPyramids);

  for (auto &pyramid : oParams.oPyramidParams) {
    pyramid.bCompute = 1;
    pyramid.nMetric = 0;
    pyramid.nMaximumNumberOfDisparities = 21;
    pyramid.fDisparityStepSize = 1.000000;
    pyramid.nSubpixelFnc = 2;
    pyramid.fMaxCost = 6.000000;
    pyramid.bDoSubpixelNormalization = 1;
    pyramid.bBackMatch = 1;
    pyramid.bComputeBackDisparity = 1;
    pyramid.fMaxBackMatchingDist = 1.50000;
    pyramid.bKeepValueOnNodataInBackmatch = 0;
    pyramid.bKeepValueOnOutsideImage = 0;
    pyramid.bOcclusionDetection = 0;
    pyramid.fOcclusionDispGapSize = 1.100000f;
    pyramid.nRegionGrowingMode = 1;
    pyramid.nRegionGrowingIterations = 1;

    // manually added from GUI
    pyramid.bMedian = true;  // use median optimizer
    pyramid.iMedX = 3;       // median size
    pyramid.iMedY = 3;
    pyramid.iNodataPol = 1;      //"Disparity Median Optimizer Nodata Policy"
    pyramid.iBorderPol = 2;      //"Disparity Median Optimizer Border Policy"
    pyramid.fMedianThresh = -1;  //"Disparity Median Optimizer Threshold"
    pyramid.bSplittedMedian =
        false;  //"Disparity Median Optimizer Splitted Version"

    pyramid.bSpeckle = true;        // use speckle optimizer
    pyramid.iSpeckleMaxSize = 200;  //"Disparity Speckle Filter Max Region Size"
    pyramid.fSpeckleMaxDiff = 0.5;  //"Disparity Speckle Filter Max Difference"

    pyramid.bInterpol = true;     // "Interpolate Disparity"
    pyramid.nDir = 32;            // "Interpolator Number Of Directions"
    pyramid.nNth = 32;            // "Interpolator Number Nth Elements"
    pyramid.nMinNth = 0;          // "Interpolator Minimum Number Of Elements"
    pyramid.fRadius = -1;         // "Interpolator Match Radius"
    pyramid.nMode = 2;            // "Interpolator Mode"
    pyramid.bOccInterpol = true;  // "Interpolate Occlusions"
    pyramid.nOccDir = 32;  // "Occlusion Interpolator Number Of Directions"
    pyramid.nOccNth = 32;  // "Occlusion Interpolator Number Nth Elements"
    pyramid.nOccMinNth =
        0;  // "Occlusion Interpolator Minimum Number Of Elements"
    pyramid.fOccRadius = -1;  // "Occlusion Interpolator Match Radius"
    pyramid.nOccMode = 2;     // "Occlusion Interpolator Mode"

    // oPyramidParams 0 oMetricParams
    pyramid.oMetricParams.nWindowSizeX = 13;
    pyramid.oMetricParams.nWindowSizeY = 13;
    pyramid.oMetricParams.fMetricDeltaX = 1.000000;
    pyramid.oMetricParams.fMetricDeltaY = 1.000000;
    pyramid.oMetricParams.nCensusCenterFunction = 1;
    pyramid.oMetricParams.nNumberOfScales = 1;
    pyramid.oMetricParams.fScaleStepSize = 0.250000;
    pyramid.oMetricParams.nNumberOfSlants = 0;
    pyramid.oMetricParams.fSlantStepSize = 0.250000;
    pyramid.oMetricParams.bUseProximityWeight = 0;
    pyramid.oMetricParams.fProximityGamma = 2.000000;
    pyramid.oMetricParams.bUseSimilarityWeight = 0;
    pyramid.oMetricParams.fSimilarityGamma = 2.000000;

    // oPyramidParams 0 oSGMParams
    pyramid.oSGMParams = oSGMParams;
  }

  // oFinalSubPixelParameters
  oSubPix.bCompute = 0;
  oSubPix.nMetric = 8;
  oSubPix.nMaximumNumberOfDisparities = 21;
  oSubPix.fDisparityStepSize = 0.100000f;
  oSubPix.nSubpixelFnc = 2;
  oSubPix.fMaxCost = 6.000000;
  oSubPix.bDoSubpixelNormalization = 1;
  oSubPix.bBackMatch = 1;
  oSubPix.bComputeBackDisparity = 0;
  oSubPix.fMaxBackMatchingDist = 1.500000;
  oSubPix.bKeepValueOnNodataInBackmatch = 0;
  oSubPix.bKeepValueOnOutsideImage = 0;
  oSubPix.bOcclusionDetection = 1;
  oSubPix.fOcclusionDispGapSize = 1.100000f;
  oSubPix.nRegionGrowingMode = 1;
  oSubPix.nRegionGrowingIterations = 1;
  oSubPix.bInterpol = 0;
  oParams.oFinalSubPixelParameters = oSubPix;
}

void MatcherJRSGM::setMatchCosts(double P1, double P2) {
  oSGMParams.fP1_E_W = P1;
  oSGMParams.fP1_SE_NW = P1;
  oSGMParams.fP1_SW_NE = P1;
  oSGMParams.fP1_S_N = P1;

  oSGMParams.fP2_E_W = P2;
  oSGMParams.fP2_SE_NW = P2;
  oSGMParams.fP2_SW_NE = P2;
  oSGMParams.fP2_S_N = P2;

  for (auto &pyramid : oParams.oPyramidParams) {
    pyramid.oSGMParams = oSGMParams;
  }
}

void MatcherJRSGM::setWindowSize(int census_size) {
  for (auto &pyramid : oParams.oPyramidParams) {
    pyramid.oMetricParams.nWindowSizeX = census_size;
    pyramid.oMetricParams.nWindowSizeY = census_size;
  }
}

void MatcherJRSGM::setDisparityShift(int shift) {
  oParams.fTopPredictionShift = shift / pow(2, oParams.nNumberOfPyramids - 1);
  min_disparity = shift;
}

void MatcherJRSGM::enableSubpixel(bool enable) {
  oParams.oFinalSubPixelParameters.bCompute = enable;
}

void MatcherJRSGM::setDisparityRange(int n) {
  if (n % 2) {
    disparity_range = n;
    oParams.oPyramidParams[0].nMaximumNumberOfDisparities = n;
    oParams.oPyramidParams[1].nMaximumNumberOfDisparities = n;
    oParams.oPyramidParams[2].nMaximumNumberOfDisparities = n;
    oParams.oPyramidParams[3].nMaximumNumberOfDisparities = n;
    oParams.oPyramidParams[4].nMaximumNumberOfDisparities = n;

    oSubPix.nMaximumNumberOfDisparities = n;
    oParams.oFinalSubPixelParameters = oSubPix;
  }
}

void MatcherJRSGM::enableInterpolation(bool enable) {
  for (auto &pyramid : oParams.oPyramidParams) {
    pyramid.bInterpol = enable;
  }

  oParams.oFinalSubPixelParameters.bInterpol = enable;
}

void MatcherJRSGM::enableOcclusionDetection(bool enable) {
  for (auto &pyramid : oParams.oPyramidParams) {
    pyramid.bOccInterpol = enable;
    // pyramid.bOcclusionDetection = enable;
  }

  oParams.oFinalSubPixelParameters.bOcclusionDetection = enable;
}

void MatcherJRSGM::forwardMatch() {
  try {
    cv::Mat oDisparity;
    MatchStereo(oParams, *left, *right, oDisparity, "./logfile.txt", e_logNone);

    oDisparity.convertTo(disparity_lr, CV_32F, -16);

   // filterSpeckles(disparity_lr, -10000, 5, 5);

  } catch (...) {
    qDebug() << "Impact matcher parameter error";
  }
}

void MatcherJRSGM::backwardMatch() {}

void MatcherJRSGM::writeConfig(const std::string &sConfigFile,
                                  const SMatchingParametersInput &oParams) {
  boost::property_tree::ptree oConfig;
  oConfig.put("MatchingParameters.bHasNodata", oParams.bHasNodata);
  oConfig.put("MatchingParameters.fNodataValue", oParams.fNodataValue);
  oConfig.put("MatchingParameters.bWriteBackDisp", oParams.bWriteBackDisp);
  oConfig.put("MatchingParameters.bWriteRevBackMatchDistance",
              oParams.bWriteRevBackMatchDistance);
  oConfig.put("MatchingParameters.strRevBackMatchDistanceOutputFile",
              oParams.strRevBackMatchDistanceOutputFile);
  oConfig.put("MatchingParameters.bWriteBackMatchDistance",
              oParams.bWriteBackMatchDistance);
  oConfig.put("MatchingParameters.strBackMatchDistanceOutputFile",
              oParams.strBackMatchDistanceOutputFile);
  oConfig.put("MatchingParameters.nNumberOfPyramids",
              oParams.nNumberOfPyramids);
  oConfig.put("MatchingParameters.nPredictor", oParams.nPredictor);
  oConfig.put("MatchingParameters.strDispPredictionFile",
              oParams.strDispPredictionFile);
  oConfig.put("MatchingParameters.strBackDispPredictionFile",
              oParams.strBackDispPredictionFile);
  oConfig.put("MatchingParameters.fTopPredictionShift",
              oParams.fTopPredictionShift);
  oConfig.put("MatchingParameters.nAdaptivePrefilter",
              oParams.nAdaptivePrefilter);
  oConfig.put("MatchingParameters.nAdaptivePrefilterSize",
              oParams.nAdaptivePrefilterSize);
  oConfig.put("MatchingParameters.nNumberOfMorphologicalIter",
              oParams.nNumberOfMorphologicalIter);
  oConfig.put("MatchingParameters.nDebugLevel", oParams.nDebugLevel);
  oConfig.put("MatchingParameters.strDebugOutput", oParams.strDebugOutput);
  // oConfig.put("MatchingParameters.ProcessAffinity", std::set<int>() );
  // oConfig.put("MatchingParameters.GPUs", std::vector<int>(1, 0) );

  // oPyramidParams 0
  int i = 0;
  oConfig.put("PyramidParams[0].bCompute", oParams.oPyramidParams[i].bCompute);
  oConfig.put("PyramidParams[0].nMetric", oParams.oPyramidParams[i].nMetric);
  oConfig.put("PyramidParams[0].nMaximumNumberOfDisparities",
              oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oConfig.put("PyramidParams[0].fDisparityStepSize",
              oParams.oPyramidParams[i].fDisparityStepSize);
  oConfig.put("PyramidParams[0].nSubpixelFnc",
              oParams.oPyramidParams[i].nSubpixelFnc);
  oConfig.put("PyramidParams[0].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oConfig.put("PyramidParams[0].bDoSubpixelNormalization",
              oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oConfig.put("PyramidParams[0].bBackMatch",
              oParams.oPyramidParams[i].bBackMatch);
  oConfig.put("PyramidParams[0].bComputeBackDisparity",
              oParams.oPyramidParams[i].bComputeBackDisparity);
  oConfig.put("PyramidParams[0].fMaxBackMatchingDist",
              oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oConfig.put("PyramidParams[0].bKeepValueOnNodataInBackmatch",
              oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oConfig.put("PyramidParams[0].bKeepValueOnOutsideImage",
              oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oConfig.put("PyramidParams[0].bOcclusionDetection",
              oParams.oPyramidParams[i].bOcclusionDetection);
  oConfig.put("PyramidParams[0].fOcclusionDispGapSize",
              oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oConfig.put("PyramidParams[0].nRegionGrowingMode",
              oParams.oPyramidParams[i].nRegionGrowingMode);
  oConfig.put("PyramidParams[0].nRegionGrowingIterations",
              oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oConfig.put("PyramidParams[0].bMedian", oParams.oPyramidParams[i].bMedian);
  oConfig.put("PyramidParams[0].iMedX", oParams.oPyramidParams[i].iMedX);
  oConfig.put("PyramidParams[0].iMedY", oParams.oPyramidParams[i].iMedY);
  oConfig.put("PyramidParams[0].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oConfig.put("PyramidParams[0].iNodataPol",
              oParams.oPyramidParams[i].iNodataPol);
  oConfig.put("PyramidParams[0].iBorderPol",
              oParams.oPyramidParams[i].iBorderPol);
  oConfig.put("PyramidParams[0].fMedianThresh",
              oParams.oPyramidParams[i].fMedianThresh);
  oConfig.put("PyramidParams[0].bSplittedMedian",
              oParams.oPyramidParams[i].bSplittedMedian);
  oConfig.put("PyramidParams[0].iSpeckleMaxSize",
              oParams.oPyramidParams[i].iSpeckleMaxSize);
  oConfig.put("PyramidParams[0].fSpeckleMaxDiff",
              oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oConfig.put("PyramidParams[0].bInterpol",
              oParams.oPyramidParams[i].bInterpol);
  oConfig.put("PyramidParams[0].nDir", oParams.oPyramidParams[i].nDir);
  oConfig.put("PyramidParams[0].nNth", oParams.oPyramidParams[i].nNth);
  oConfig.put("PyramidParams[0].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oConfig.put("PyramidParams[0].fRadius", oParams.oPyramidParams[i].fRadius);
  oConfig.put("PyramidParams[0].nMode", oParams.oPyramidParams[i].nMode);
  oConfig.put("PyramidParams[0].bOccInterpol",
              oParams.oPyramidParams[i].bOccInterpol);
  oConfig.put("PyramidParams[0].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oConfig.put("PyramidParams[0].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oConfig.put("PyramidParams[0].nOccMinNth",
              oParams.oPyramidParams[i].nOccMinNth);
  oConfig.put("PyramidParams[0].fOccRadius",
              oParams.oPyramidParams[i].fOccRadius);
  oConfig.put("PyramidParams[0].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oConfig.put("PyramidParams[0].oMetricParams.nWindowSizeX",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oConfig.put("PyramidParams[0].oMetricParams.nWindowSizeY",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oConfig.put("PyramidParams[0].oMetricParams.fMetricDeltaX",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oConfig.put("PyramidParams[0].oMetricParams.fMetricDeltaY",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oConfig.put("PyramidParams[0].oMetricParams.nCensusCenterFunction",
              oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oConfig.put("PyramidParams[0].oMetricParams.nNumberOfScales",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oConfig.put("PyramidParams[0].oMetricParams.fScaleStepSize",
              oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oConfig.put("PyramidParams[0].oMetricParams.nNumberOfSlants",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oConfig.put("PyramidParams[0].oMetricParams.fSlantStepSize",
              oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oConfig.put("PyramidParams[0].oMetricParams.bUseProximityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oConfig.put("PyramidParams[0].oMetricParams.fProximityGamma",
              oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oConfig.put("PyramidParams[0].oMetricParams.bUseSimilarityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oConfig.put("PyramidParams[0].oMetricParams.fSimilarityGamma",
              oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oConfig.put("PyramidParams[0].oSGMParams.fCostNodataValue",
              oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oConfig.put(
      "PyramidParams[0].oSGMParams.nAllowedOutsideImageMatchCount",
      oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oConfig.put("PyramidParams[0].oSGMParams.bDSICountCorrection",
              oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oConfig.put(
      "PyramidParams[0].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oConfig.put("PyramidParams[0].oSGMParams.bMatch_S_N",
              oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.fP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.fP2_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.bP2EqualP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.bAdditive_S_N",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oConfig.put("PyramidParams[0].oSGMParams.bMatch_E_W",
              oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.fP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.fP2_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.bP2EqualP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.bAdditive_E_W",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oConfig.put("PyramidParams[0].oSGMParams.bMatch_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.fP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.fP2_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.bP2EqualP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.bAdditive_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oConfig.put("PyramidParams[0].oSGMParams.bMatch_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.fP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.fP2_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.bP2EqualP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.bAdditive_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oConfig.put("PyramidParams[0].oSGMParams.bUseDSITexture",
              oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oConfig.put("PyramidParams[0].oSGMParams.bUseAggregatedCostTexture",
              oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oPyramidParams 1
  i = 1;
  oConfig.put("PyramidParams[1].bCompute", oParams.oPyramidParams[i].bCompute);
  oConfig.put("PyramidParams[1].nMetric", oParams.oPyramidParams[i].nMetric);
  oConfig.put("PyramidParams[1].nMaximumNumberOfDisparities",
              oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oConfig.put("PyramidParams[1].fDisparityStepSize",
              oParams.oPyramidParams[i].fDisparityStepSize);
  oConfig.put("PyramidParams[1].nSubpixelFnc",
              oParams.oPyramidParams[i].nSubpixelFnc);
  oConfig.put("PyramidParams[1].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oConfig.put("PyramidParams[1].bDoSubpixelNormalization",
              oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oConfig.put("PyramidParams[1].bBackMatch",
              oParams.oPyramidParams[i].bBackMatch);
  oConfig.put("PyramidParams[1].bComputeBackDisparity",
              oParams.oPyramidParams[i].bComputeBackDisparity);
  oConfig.put("PyramidParams[1].fMaxBackMatchingDist",
              oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oConfig.put("PyramidParams[1].bKeepValueOnNodataInBackmatch",
              oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oConfig.put("PyramidParams[1].bKeepValueOnOutsideImage",
              oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oConfig.put("PyramidParams[1].bOcclusionDetection",
              oParams.oPyramidParams[i].bOcclusionDetection);
  oConfig.put("PyramidParams[1].fOcclusionDispGapSize",
              oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oConfig.put("PyramidParams[1].nRegionGrowingMode",
              oParams.oPyramidParams[i].nRegionGrowingMode);
  oConfig.put("PyramidParams[1].nRegionGrowingIterations",
              oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oConfig.put("PyramidParams[1].bMedian", oParams.oPyramidParams[i].bMedian);
  oConfig.put("PyramidParams[1].iMedX", oParams.oPyramidParams[i].iMedX);
  oConfig.put("PyramidParams[1].iMedY", oParams.oPyramidParams[i].iMedY);
  oConfig.put("PyramidParams[1].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oConfig.put("PyramidParams[1].iNodataPol",
              oParams.oPyramidParams[i].iNodataPol);
  oConfig.put("PyramidParams[1].iBorderPol",
              oParams.oPyramidParams[i].iBorderPol);
  oConfig.put("PyramidParams[1].fMedianThresh",
              oParams.oPyramidParams[i].fMedianThresh);
  oConfig.put("PyramidParams[1].bSplittedMedian",
              oParams.oPyramidParams[i].bSplittedMedian);
  oConfig.put("PyramidParams[1].iSpeckleMaxSize",
              oParams.oPyramidParams[i].iSpeckleMaxSize);
  oConfig.put("PyramidParams[1].fSpeckleMaxDiff",
              oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oConfig.put("PyramidParams[1].bInterpol",
              oParams.oPyramidParams[i].bInterpol);
  oConfig.put("PyramidParams[1].nDir", oParams.oPyramidParams[i].nDir);
  oConfig.put("PyramidParams[1].nNth", oParams.oPyramidParams[i].nNth);
  oConfig.put("PyramidParams[1].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oConfig.put("PyramidParams[1].fRadius", oParams.oPyramidParams[i].fRadius);
  oConfig.put("PyramidParams[1].nMode", oParams.oPyramidParams[i].nMode);
  oConfig.put("PyramidParams[1].bOccInterpol",
              oParams.oPyramidParams[i].bOccInterpol);
  oConfig.put("PyramidParams[1].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oConfig.put("PyramidParams[1].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oConfig.put("PyramidParams[1].nOccMinNth",
              oParams.oPyramidParams[i].nOccMinNth);
  oConfig.put("PyramidParams[1].fOccRadius",
              oParams.oPyramidParams[i].fOccRadius);
  oConfig.put("PyramidParams[1].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 1 oMetricParcParams , //oPyramidParams 0 oMetricParams
  oConfig.put("PyramidParams[1].oMetricParams.nWindowSizeX",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oConfig.put("PyramidParams[1].oMetricParams.nWindowSizeY",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oConfig.put("PyramidParams[1].oMetricParams.fMetricDeltaX",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oConfig.put("PyramidParams[1].oMetricParams.fMetricDeltaY",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oConfig.put("PyramidParams[1].oMetricParams.nCensusCenterFunction",
              oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oConfig.put("PyramidParams[1].oMetricParams.nNumberOfScales",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oConfig.put("PyramidParams[1].oMetricParams.fScaleStepSize",
              oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oConfig.put("PyramidParams[1].oMetricParams.nNumberOfSlants",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oConfig.put("PyramidParams[1].oMetricParams.fSlantStepSize",
              oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oConfig.put("PyramidParams[1].oMetricParams.bUseProximityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oConfig.put("PyramidParams[1].oMetricParams.fProximityGamma",
              oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oConfig.put("PyramidParams[1].oMetricParams.bUseSimilarityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oConfig.put("PyramidParams[1].oMetricParams.fSimilarityGamma",
              oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 1 oSGMParamsrams , //oPyramidParams 0 oSGMParams
  oConfig.put("PyramidParams[1].oSGMParams.fCostNodataValue",
              oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oConfig.put(
      "PyramidParams[1].oSGMParams.nAllowedOutsideImageMatchCount",
      oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oConfig.put("PyramidParams[1].oSGMParams.bDSICountCorrection",
              oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oConfig.put(
      "PyramidParams[1].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oConfig.put("PyramidParams[1].oSGMParams.bMatch_S_N",
              oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.fP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.fP2_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.bP2EqualP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.bAdditive_S_N",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oConfig.put("PyramidParams[1].oSGMParams.bMatch_E_W",
              oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.fP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.fP2_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.bP2EqualP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.bAdditive_E_W",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oConfig.put("PyramidParams[1].oSGMParams.bMatch_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.fP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.fP2_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.bP2EqualP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.bAdditive_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oConfig.put("PyramidParams[1].oSGMParams.bMatch_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.fP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.fP2_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.bP2EqualP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.bAdditive_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oConfig.put("PyramidParams[1].oSGMParams.bUseDSITexture",
              oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oConfig.put("PyramidParams[1].oSGMParams.bUseAggregatedCostTexture",
              oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oPyramidParams 2
  i = 2;
  oConfig.put("PyramidParams[2].bCompute", oParams.oPyramidParams[i].bCompute);
  oConfig.put("PyramidParams[2].nMetric", oParams.oPyramidParams[i].nMetric);
  oConfig.put("PyramidParams[2].nMaximumNumberOfDisparities",
              oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oConfig.put("PyramidParams[2].fDisparityStepSize",
              oParams.oPyramidParams[i].fDisparityStepSize);
  oConfig.put("PyramidParams[2].nSubpixelFnc",
              oParams.oPyramidParams[i].nSubpixelFnc);
  oConfig.put("PyramidParams[2].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oConfig.put("PyramidParams[2].bDoSubpixelNormalization",
              oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oConfig.put("PyramidParams[2].bBackMatch",
              oParams.oPyramidParams[i].bBackMatch);
  oConfig.put("PyramidParams[2].bComputeBackDisparity",
              oParams.oPyramidParams[i].bComputeBackDisparity);
  oConfig.put("PyramidParams[2].fMaxBackMatchingDist",
              oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oConfig.put("PyramidParams[2].bKeepValueOnNodataInBackmatch",
              oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oConfig.put("PyramidParams[2].bKeepValueOnOutsideImage",
              oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oConfig.put("PyramidParams[2].bOcclusionDetection",
              oParams.oPyramidParams[i].bOcclusionDetection);
  oConfig.put("PyramidParams[2].fOcclusionDispGapSize",
              oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oConfig.put("PyramidParams[2].nRegionGrowingMode",
              oParams.oPyramidParams[i].nRegionGrowingMode);
  oConfig.put("PyramidParams[2].nRegionGrowingIterations",
              oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oConfig.put("PyramidParams[2].bMedian", oParams.oPyramidParams[i].bMedian);
  oConfig.put("PyramidParams[2].iMedX", oParams.oPyramidParams[i].iMedX);
  oConfig.put("PyramidParams[2].iMedY", oParams.oPyramidParams[i].iMedY);
  oConfig.put("PyramidParams[2].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oConfig.put("PyramidParams[2].iNodataPol",
              oParams.oPyramidParams[i].iNodataPol);
  oConfig.put("PyramidParams[2].iBorderPol",
              oParams.oPyramidParams[i].iBorderPol);
  oConfig.put("PyramidParams[2].fMedianThresh",
              oParams.oPyramidParams[i].fMedianThresh);
  oConfig.put("PyramidParams[2].bSplittedMedian",
              oParams.oPyramidParams[i].bSplittedMedian);
  oConfig.put("PyramidParams[2].iSpeckleMaxSize",
              oParams.oPyramidParams[i].iSpeckleMaxSize);
  oConfig.put("PyramidParams[2].fSpeckleMaxDiff",
              oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oConfig.put("PyramidParams[2].bInterpol",
              oParams.oPyramidParams[i].bInterpol);
  oConfig.put("PyramidParams[2].nDir", oParams.oPyramidParams[i].nDir);
  oConfig.put("PyramidParams[2].nNth", oParams.oPyramidParams[i].nNth);
  oConfig.put("PyramidParams[2].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oConfig.put("PyramidParams[2].fRadius", oParams.oPyramidParams[i].fRadius);
  oConfig.put("PyramidParams[2].nMode", oParams.oPyramidParams[i].nMode);
  oConfig.put("PyramidParams[2].bOccInterpol",
              oParams.oPyramidParams[i].bOccInterpol);
  oConfig.put("PyramidParams[2].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oConfig.put("PyramidParams[2].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oConfig.put("PyramidParams[2].nOccMinNth",
              oParams.oPyramidParams[i].nOccMinNth);
  oConfig.put("PyramidParams[2].fOccRadius",
              oParams.oPyramidParams[i].fOccRadius);
  oConfig.put("PyramidParams[2].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 2 oMetricParcParams , //oPyramidParams 0 oMetricParams
  oConfig.put("PyramidParams[2].oMetricParams.nWindowSizeX",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oConfig.put("PyramidParams[2].oMetricParams.nWindowSizeY",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oConfig.put("PyramidParams[2].oMetricParams.fMetricDeltaX",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oConfig.put("PyramidParams[2].oMetricParams.fMetricDeltaY",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oConfig.put("PyramidParams[2].oMetricParams.nCensusCenterFunction",
              oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oConfig.put("PyramidParams[2].oMetricParams.nNumberOfScales",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oConfig.put("PyramidParams[2].oMetricParams.fScaleStepSize",
              oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oConfig.put("PyramidParams[2].oMetricParams.nNumberOfSlants",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oConfig.put("PyramidParams[2].oMetricParams.fSlantStepSize",
              oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oConfig.put("PyramidParams[2].oMetricParams.bUseProximityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oConfig.put("PyramidParams[2].oMetricParams.fProximityGamma",
              oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oConfig.put("PyramidParams[2].oMetricParams.bUseSimilarityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oConfig.put("PyramidParams[2].oMetricParams.fSimilarityGamma",
              oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 2 oSGMParamsrams , //oPyramidParams 0 oSGMParams
  oConfig.put("PyramidParams[2].oSGMParams.fCostNodataValue",
              oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oConfig.put(
      "PyramidParams[2].oSGMParams.nAllowedOutsideImageMatchCount",
      oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oConfig.put("PyramidParams[2].oSGMParams.bDSICountCorrection",
              oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oConfig.put(
      "PyramidParams[2].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oConfig.put("PyramidParams[2].oSGMParams.bMatch_S_N",
              oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.fP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.fP2_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.bP2EqualP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.bAdditive_S_N",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oConfig.put("PyramidParams[2].oSGMParams.bMatch_E_W",
              oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.fP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.fP2_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.bP2EqualP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.bAdditive_E_W",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oConfig.put("PyramidParams[2].oSGMParams.bMatch_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.fP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.fP2_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.bP2EqualP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.bAdditive_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oConfig.put("PyramidParams[2].oSGMParams.bMatch_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.fP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.fP2_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.bP2EqualP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.bAdditive_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oConfig.put("PyramidParams[2].oSGMParams.bUseDSITexture",
              oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oConfig.put("PyramidParams[2].oSGMParams.bUseAggregatedCostTexture",
              oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oPyramidParams 3
  i = 3;
  oConfig.put("PyramidParams[3].bCompute", oParams.oPyramidParams[i].bCompute);
  oConfig.put("PyramidParams[3].nMetric", oParams.oPyramidParams[i].nMetric);
  oConfig.put("PyramidParams[3].nMaximumNumberOfDisparities",
              oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oConfig.put("PyramidParams[3].fDisparityStepSize",
              oParams.oPyramidParams[i].fDisparityStepSize);
  oConfig.put("PyramidParams[3].nSubpixelFnc",
              oParams.oPyramidParams[i].nSubpixelFnc);
  oConfig.put("PyramidParams[3].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oConfig.put("PyramidParams[3].bDoSubpixelNormalization",
              oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oConfig.put("PyramidParams[3].bBackMatch",
              oParams.oPyramidParams[i].bBackMatch);
  oConfig.put("PyramidParams[3].bComputeBackDisparity",
              oParams.oPyramidParams[i].bComputeBackDisparity);
  oConfig.put("PyramidParams[3].fMaxBackMatchingDist",
              oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oConfig.put("PyramidParams[3].bKeepValueOnNodataInBackmatch",
              oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oConfig.put("PyramidParams[3].bKeepValueOnOutsideImage",
              oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oConfig.put("PyramidParams[3].bOcclusionDetection",
              oParams.oPyramidParams[i].bOcclusionDetection);
  oConfig.put("PyramidParams[3].fOcclusionDispGapSize",
              oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oConfig.put("PyramidParams[3].nRegionGrowingMode",
              oParams.oPyramidParams[i].nRegionGrowingMode);
  oConfig.put("PyramidParams[3].nRegionGrowingIterations",
              oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oConfig.put("PyramidParams[3].bMedian", oParams.oPyramidParams[i].bMedian);
  oConfig.put("PyramidParams[3].iMedX", oParams.oPyramidParams[i].iMedX);
  oConfig.put("PyramidParams[3].iMedY", oParams.oPyramidParams[i].iMedY);
  oConfig.put("PyramidParams[3].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oConfig.put("PyramidParams[3].iNodataPol",
              oParams.oPyramidParams[i].iNodataPol);
  oConfig.put("PyramidParams[3].iBorderPol",
              oParams.oPyramidParams[i].iBorderPol);
  oConfig.put("PyramidParams[3].fMedianThresh",
              oParams.oPyramidParams[i].fMedianThresh);
  oConfig.put("PyramidParams[3].bSplittedMedian",
              oParams.oPyramidParams[i].bSplittedMedian);
  oConfig.put("PyramidParams[3].iSpeckleMaxSize",
              oParams.oPyramidParams[i].iSpeckleMaxSize);
  oConfig.put("PyramidParams[3].fSpeckleMaxDiff",
              oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oConfig.put("PyramidParams[3].bInterpol",
              oParams.oPyramidParams[i].bInterpol);
  oConfig.put("PyramidParams[3].nDir", oParams.oPyramidParams[i].nDir);
  oConfig.put("PyramidParams[3].nNth", oParams.oPyramidParams[i].nNth);
  oConfig.put("PyramidParams[3].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oConfig.put("PyramidParams[3].fRadius", oParams.oPyramidParams[i].fRadius);
  oConfig.put("PyramidParams[3].nMode", oParams.oPyramidParams[i].nMode);
  oConfig.put("PyramidParams[3].bOccInterpol",
              oParams.oPyramidParams[i].bOccInterpol);
  oConfig.put("PyramidParams[3].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oConfig.put("PyramidParams[3].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oConfig.put("PyramidParams[3].nOccMinNth",
              oParams.oPyramidParams[i].nOccMinNth);
  oConfig.put("PyramidParams[3].fOccRadius",
              oParams.oPyramidParams[i].fOccRadius);
  oConfig.put("PyramidParams[3].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 3 oMetricParcParams , //oPyramidParams 0 oMetricParams
  oConfig.put("PyramidParams[3].oMetricParams.nWindowSizeX",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oConfig.put("PyramidParams[3].oMetricParams.nWindowSizeY",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oConfig.put("PyramidParams[3].oMetricParams.fMetricDeltaX",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oConfig.put("PyramidParams[3].oMetricParams.fMetricDeltaY",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oConfig.put("PyramidParams[3].oMetricParams.nCensusCenterFunction",
              oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oConfig.put("PyramidParams[3].oMetricParams.nNumberOfScales",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oConfig.put("PyramidParams[3].oMetricParams.fScaleStepSize",
              oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oConfig.put("PyramidParams[3].oMetricParams.nNumberOfSlants",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oConfig.put("PyramidParams[3].oMetricParams.fSlantStepSize",
              oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oConfig.put("PyramidParams[3].oMetricParams.bUseProximityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oConfig.put("PyramidParams[3].oMetricParams.fProximityGamma",
              oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oConfig.put("PyramidParams[3].oMetricParams.bUseSimilarityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oConfig.put("PyramidParams[3].oMetricParams.fSimilarityGamma",
              oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 3 oSGMParamsrams , //oPyramidParams 0 oSGMParams
  oConfig.put("PyramidParams[3].oSGMParams.fCostNodataValue",
              oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oConfig.put(
      "PyramidParams[3].oSGMParams.nAllowedOutsideImageMatchCount",
      oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oConfig.put("PyramidParams[3].oSGMParams.bDSICountCorrection",
              oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oConfig.put(
      "PyramidParams[3].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oConfig.put("PyramidParams[3].oSGMParams.bMatch_S_N",
              oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.fP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.fP2_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.bP2EqualP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.bAdditive_S_N",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oConfig.put("PyramidParams[3].oSGMParams.bMatch_E_W",
              oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.fP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.fP2_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.bP2EqualP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.bAdditive_E_W",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oConfig.put("PyramidParams[3].oSGMParams.bMatch_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.fP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.fP2_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.bP2EqualP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.bAdditive_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oConfig.put("PyramidParams[3].oSGMParams.bMatch_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.fP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.fP2_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.bP2EqualP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.bAdditive_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oConfig.put("PyramidParams[3].oSGMParams.bUseDSITexture",
              oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oConfig.put("PyramidParams[3].oSGMParams.bUseAggregatedCostTexture",
              oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oPyramidParams 4
  i = 4;
  oConfig.put("PyramidParams[4].bCompute", oParams.oPyramidParams[i].bCompute);
  oConfig.put("PyramidParams[4].nMetric", oParams.oPyramidParams[i].nMetric);
  oConfig.put("PyramidParams[4].nMaximumNumberOfDisparities",
              oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oConfig.put("PyramidParams[4].fDisparityStepSize",
              oParams.oPyramidParams[i].fDisparityStepSize);
  oConfig.put("PyramidParams[4].nSubpixelFnc",
              oParams.oPyramidParams[i].nSubpixelFnc);
  oConfig.put("PyramidParams[4].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oConfig.put("PyramidParams[4].bDoSubpixelNormalization",
              oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oConfig.put("PyramidParams[4].bBackMatch",
              oParams.oPyramidParams[i].bBackMatch);
  oConfig.put("PyramidParams[4].bComputeBackDisparity",
              oParams.oPyramidParams[i].bComputeBackDisparity);
  oConfig.put("PyramidParams[4].fMaxBackMatchingDist",
              oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oConfig.put("PyramidParams[4].bKeepValueOnNodataInBackmatch",
              oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oConfig.put("PyramidParams[4].bKeepValueOnOutsideImage",
              oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oConfig.put("PyramidParams[4].bOcclusionDetection",
              oParams.oPyramidParams[i].bOcclusionDetection);
  oConfig.put("PyramidParams[4].fOcclusionDispGapSize",
              oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oConfig.put("PyramidParams[4].nRegionGrowingMode",
              oParams.oPyramidParams[i].nRegionGrowingMode);
  oConfig.put("PyramidParams[4].nRegionGrowingIterations",
              oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oConfig.put("PyramidParams[4].bMedian", oParams.oPyramidParams[i].bMedian);
  oConfig.put("PyramidParams[4].iMedX", oParams.oPyramidParams[i].iMedX);
  oConfig.put("PyramidParams[4].iMedY", oParams.oPyramidParams[i].iMedY);
  oConfig.put("PyramidParams[4].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oConfig.put("PyramidParams[4].iNodataPol",
              oParams.oPyramidParams[i].iNodataPol);
  oConfig.put("PyramidParams[4].iBorderPol",
              oParams.oPyramidParams[i].iBorderPol);
  oConfig.put("PyramidParams[4].fMedianThresh",
              oParams.oPyramidParams[i].fMedianThresh);
  oConfig.put("PyramidParams[4].bSplittedMedian",
              oParams.oPyramidParams[i].bSplittedMedian);
  oConfig.put("PyramidParams[4].iSpeckleMaxSize",
              oParams.oPyramidParams[i].iSpeckleMaxSize);
  oConfig.put("PyramidParams[4].fSpeckleMaxDiff",
              oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oConfig.put("PyramidParams[4].bInterpol",
              oParams.oPyramidParams[i].bInterpol);
  oConfig.put("PyramidParams[4].nDir", oParams.oPyramidParams[i].nDir);
  oConfig.put("PyramidParams[4].nNth", oParams.oPyramidParams[i].nNth);
  oConfig.put("PyramidParams[4].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oConfig.put("PyramidParams[4].fRadius", oParams.oPyramidParams[i].fRadius);
  oConfig.put("PyramidParams[4].nMode", oParams.oPyramidParams[i].nMode);
  oConfig.put("PyramidParams[4].bOccInterpol",
              oParams.oPyramidParams[i].bOccInterpol);
  oConfig.put("PyramidParams[4].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oConfig.put("PyramidParams[4].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oConfig.put("PyramidParams[4].nOccMinNth",
              oParams.oPyramidParams[i].nOccMinNth);
  oConfig.put("PyramidParams[4].fOccRadius",
              oParams.oPyramidParams[i].fOccRadius);
  oConfig.put("PyramidParams[4].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams , //oPyramidParams 0 oMetricParams
  oConfig.put("PyramidParams[4].oMetricParams.nWindowSizeX",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oConfig.put("PyramidParams[4].oMetricParams.nWindowSizeY",
              oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oConfig.put("PyramidParams[4].oMetricParams.fMetricDeltaX",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oConfig.put("PyramidParams[4].oMetricParams.fMetricDeltaY",
              oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oConfig.put("PyramidParams[4].oMetricParams.nCensusCenterFunction",
              oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oConfig.put("PyramidParams[4].oMetricParams.nNumberOfScales",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oConfig.put("PyramidParams[4].oMetricParams.fScaleStepSize",
              oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oConfig.put("PyramidParams[4].oMetricParams.nNumberOfSlants",
              oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oConfig.put("PyramidParams[4].oMetricParams.fSlantStepSize",
              oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oConfig.put("PyramidParams[4].oMetricParams.bUseProximityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oConfig.put("PyramidParams[4].oMetricParams.fProximityGamma",
              oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oConfig.put("PyramidParams[4].oMetricParams.bUseSimilarityWeight",
              oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oConfig.put("PyramidParams[4].oMetricParams.fSimilarityGamma",
              oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams , //oPyramidParams 0 oSGMParams
  oConfig.put("PyramidParams[4].oSGMParams.fCostNodataValue",
              oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oConfig.put(
      "PyramidParams[4].oSGMParams.nAllowedOutsideImageMatchCount",
      oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oConfig.put("PyramidParams[4].oSGMParams.bDSICountCorrection",
              oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oConfig.put(
      "PyramidParams[4].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oConfig.put("PyramidParams[4].oSGMParams.bMatch_S_N",
              oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.fP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.fP2_S_N",
              oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.bP2EqualP1_S_N",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_S_N",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.bAdditive_S_N",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oConfig.put("PyramidParams[4].oSGMParams.bMatch_E_W",
              oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.fP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.fP2_E_W",
              oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.bP2EqualP1_E_W",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_E_W",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.bAdditive_E_W",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oConfig.put("PyramidParams[4].oSGMParams.bMatch_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.fP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.fP2_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.bP2EqualP1_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.bAdditive_SW_NE",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oConfig.put("PyramidParams[4].oSGMParams.bMatch_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.fP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.fP2_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.bP2EqualP1_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.bAdditive_SE_NW",
              oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oConfig.put("PyramidParams[4].oSGMParams.bUseDSITexture",
              oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oConfig.put("PyramidParams[4].oSGMParams.bUseAggregatedCostTexture",
              oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oFinalSubPixelParameters
  oConfig.put("SubpixelPyramidParams.bCompute",
              oParams.oFinalSubPixelParameters.bCompute);
  oConfig.put("SubpixelPyramidParams.nMetric",
              oParams.oFinalSubPixelParameters.nMetric);
  oConfig.put("SubpixelPyramidParams.nMaximumNumberOfDisparities",
              oParams.oFinalSubPixelParameters.nMaximumNumberOfDisparities);
  oConfig.put("SubpixelPyramidParams.fDisparityStepSize",
              oParams.oFinalSubPixelParameters.fDisparityStepSize);
  oConfig.put("SubpixelPyramidParams.nSubpixelFnc",
              oParams.oFinalSubPixelParameters.nSubpixelFnc);
  oConfig.put("SubpixelPyramidParams.fMaxCost",
              oParams.oFinalSubPixelParameters.fMaxCost);
  oConfig.put("SubpixelPyramidParams.bDoSubpixelNormalization",
              oParams.oFinalSubPixelParameters.bDoSubpixelNormalization);
  oConfig.put("SubpixelPyramidParams.bBackMatch",
              oParams.oFinalSubPixelParameters.bBackMatch);
  oConfig.put("SubpixelPyramidParams.bComputeBackDisparity",
              oParams.oFinalSubPixelParameters.bComputeBackDisparity);
  oConfig.put("SubpixelPyramidParams.fMaxBackMatchingDist",
              oParams.oFinalSubPixelParameters.fMaxBackMatchingDist);
  oConfig.put("SubpixelPyramidParams.bKeepValueOnNodataInBackmatch",
              oParams.oFinalSubPixelParameters.bKeepValueOnNodataInBackmatch);
  oConfig.put("SubpixelPyramidParams.bKeepValueOnOutsideImage",
              oParams.oFinalSubPixelParameters.bKeepValueOnOutsideImage);
  oConfig.put("SubpixelPyramidParams.bOcclusionDetection",
              oParams.oFinalSubPixelParameters.bOcclusionDetection);
  oConfig.put("SubpixelPyramidParams.fOcclusionDispGapSize",
              oParams.oFinalSubPixelParameters.fOcclusionDispGapSize);
  oConfig.put("SubpixelPyramidParams.nRegionGrowingMode",
              oParams.oFinalSubPixelParameters.nRegionGrowingMode);
  oConfig.put("SubpixelPyramidParams.nRegionGrowingIterations",
              oParams.oFinalSubPixelParameters.nRegionGrowingIterations);

  boost::property_tree::json_parser::write_json(sConfigFile, oConfig);
};

void MatcherJRSGM::readConfig(const std::string &sConfigFile,
                                 SMatchingParametersInput &oParams) {
  boost::property_tree::ptree oConfig;
  boost::property_tree::json_parser::read_json(sConfigFile, oConfig);
  oParams.bHasNodata =
      oConfig.get<bool>("MatchingParameters.bHasNodata", oParams.bHasNodata);
  oParams.fNodataValue = oConfig.get<float>("MatchingParameters.fNodataValue",
                                            oParams.fNodataValue);
  oParams.bWriteBackDisp = oConfig.get<bool>(
      "MatchingParameters.bWriteBackDisp", oParams.bWriteBackDisp);
  oParams.bWriteRevBackMatchDistance =
      oConfig.get<bool>("MatchingParameters.bWriteRevBackMatchDistance",
                        oParams.bWriteRevBackMatchDistance);
  oParams.strRevBackMatchDistanceOutputFile = oConfig.get<std::string>(
      "MatchingParameters.strRevBackMatchDistanceOutputFile",
      oParams.strRevBackMatchDistanceOutputFile);
  oParams.bWriteBackMatchDistance =
      oConfig.get<bool>("MatchingParameters.bWriteBackMatchDistance",
                        oParams.bWriteBackMatchDistance);
  oParams.strBackMatchDistanceOutputFile = oConfig.get<std::string>(
      "MatchingParameters.strBackMatchDistanceOutputFile",
      oParams.strBackMatchDistanceOutputFile);
  oParams.nNumberOfPyramids = oConfig.get<int>(
      "MatchingParameters.nNumberOfPyramids", oParams.nNumberOfPyramids);
  oParams.nPredictor =
      oConfig.get<int>("MatchingParameters.nPredictor", oParams.nPredictor);
  oParams.strDispPredictionFile =
      oConfig.get<std::string>("MatchingParameters.strDispPredictionFile",
                               oParams.strDispPredictionFile);
  oParams.strBackDispPredictionFile =
      oConfig.get<std::string>("MatchingParameters.strBackDispPredictionFile",
                               oParams.strBackDispPredictionFile);
  oParams.fTopPredictionShift = oConfig.get<float>(
      "MatchingParameters.fTopPredictionShift", oParams.fTopPredictionShift);
  oParams.nAdaptivePrefilter = oConfig.get<int>(
      "MatchingParameters.nAdaptivePrefilter", oParams.nAdaptivePrefilter);
  oParams.nAdaptivePrefilterSize =
      oConfig.get<int>("MatchingParameters.nAdaptivePrefilterSize",
                       oParams.nAdaptivePrefilterSize);
  oParams.nNumberOfMorphologicalIter =
      oConfig.get<int>("MatchingParameters.nNumberOfMorphologicalIter",
                       oParams.nNumberOfMorphologicalIter);
  oParams.nDebugLevel =
      oConfig.get<int>("MatchingParameters.nDebugLevel", oParams.nDebugLevel);
  oParams.strDebugOutput = oConfig.get<std::string>(
      "MatchingParameters.strDebugOutput", oParams.strDebugOutput);

  oParams.bHasNodata =
      oConfig.get<bool>("MatchingParameters.HasNodata", oParams.bHasNodata);
  oParams.fNodataValue = oConfig.get<float>("MatchingParameters.NodataValue",
                                            oParams.fNodataValue);
  oParams.bWriteBackDisp = oConfig.get<bool>("MatchingParameters.WriteBackDisp",
                                             oParams.bWriteBackDisp);
  oParams.bWriteRevBackMatchDistance =
      oConfig.get<bool>("MatchingParameters.WriteRevBackMatchDistance",
                        oParams.bWriteRevBackMatchDistance);
  oParams.strRevBackMatchDistanceOutputFile = oConfig.get<std::string>(
      "MatchingParameters.RevBackMatchDistanceOutputFile",
      oParams.strRevBackMatchDistanceOutputFile);
  oParams.bWriteBackMatchDistance =
      oConfig.get<bool>("MatchingParameters.WriteBackMatchDistance",
                        oParams.bWriteBackMatchDistance);
  oParams.strBackMatchDistanceOutputFile =
      oConfig.get<std::string>("MatchingParameters.BackMatchDistanceOutputFile",
                               oParams.strBackMatchDistanceOutputFile);
  oParams.nNumberOfPyramids = oConfig.get<int>(
      "MatchingParameters.NumberOfPyramids", oParams.nNumberOfPyramids);
  oParams.nPredictor =
      oConfig.get<int>("MatchingParameters.Predictor", oParams.nPredictor);
  oParams.strDispPredictionFile = oConfig.get<std::string>(
      "MatchingParameters.DispPredictionFile", oParams.strDispPredictionFile);
  oParams.strBackDispPredictionFile =
      oConfig.get<std::string>("MatchingParameters.BackDispPredictionFile",
                               oParams.strBackDispPredictionFile);
  oParams.fTopPredictionShift = oConfig.get<float>(
      "MatchingParameters.TopPredictionShift", oParams.fTopPredictionShift);
  oParams.nAdaptivePrefilter = oConfig.get<int>(
      "MatchingParameters.AdaptivePrefilter", oParams.nAdaptivePrefilter);
  oParams.nAdaptivePrefilterSize =
      oConfig.get<int>("MatchingParameters.AdaptivePrefilterSize",
                       oParams.nAdaptivePrefilterSize);
  oParams.nNumberOfMorphologicalIter =
      oConfig.get<int>("MatchingParameters.NumberOfMorphologicalIter",
                       oParams.nNumberOfMorphologicalIter);
  oParams.nDebugLevel =
      oConfig.get<int>("MatchingParameters.DebugLevel", oParams.nDebugLevel);
  oParams.strDebugOutput = oConfig.get<std::string>(
      "MatchingParameters.DebugOutput", oParams.strDebugOutput);

  // oPyramidParams 0
  int i = 0;
  oParams.oPyramidParams[i].bCompute = oConfig.get<bool>(
      "PyramidParams[0].bCompute", oParams.oPyramidParams[i].bCompute);
  oParams.oPyramidParams[i].nMetric = oConfig.get<int>(
      "PyramidParams[0].nMetric", oParams.oPyramidParams[i].nMetric);
  oParams.oPyramidParams[i].nMaximumNumberOfDisparities =
      oConfig.get<int>("PyramidParams[0].nMaximumNumberOfDisparities",
                       oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oParams.oPyramidParams[i].fDisparityStepSize =
      oConfig.get<float>("PyramidParams[0].fDisparityStepSize",
                         oParams.oPyramidParams[i].fDisparityStepSize);
  oParams.oPyramidParams[i].nSubpixelFnc = oConfig.get<int>(
      "PyramidParams[0].nSubpixelFnc", oParams.oPyramidParams[i].nSubpixelFnc);
  oParams.oPyramidParams[i].fMaxCost = oConfig.get<float>(
      "PyramidParams[0].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oParams.oPyramidParams[i].bDoSubpixelNormalization =
      oConfig.get<bool>("PyramidParams[0].bDoSubpixelNormalization",
                        oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oParams.oPyramidParams[i].bBackMatch = oConfig.get<bool>(
      "PyramidParams[0].bBackMatch", oParams.oPyramidParams[i].bBackMatch);
  oParams.oPyramidParams[i].bComputeBackDisparity =
      oConfig.get<bool>("PyramidParams[0].bComputeBackDisparity",
                        oParams.oPyramidParams[i].bComputeBackDisparity);
  oParams.oPyramidParams[i].fMaxBackMatchingDist =
      oConfig.get<float>("PyramidParams[0].fMaxBackMatchingDist",
                         oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch = oConfig.get<bool>(
      "PyramidParams[0].bKeepValueOnNodataInBackmatch",
      oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oParams.oPyramidParams[i].bKeepValueOnOutsideImage =
      oConfig.get<bool>("PyramidParams[0].bKeepValueOnOutsideImage",
                        oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oParams.oPyramidParams[i].bOcclusionDetection =
      oConfig.get<bool>("PyramidParams[0].bOcclusionDetection",
                        oParams.oPyramidParams[i].bOcclusionDetection);
  oParams.oPyramidParams[i].fOcclusionDispGapSize =
      oConfig.get<float>("PyramidParams[0].fOcclusionDispGapSize",
                         oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oParams.oPyramidParams[i].nRegionGrowingMode =
      oConfig.get<int>("PyramidParams[0].nRegionGrowingMode",
                       oParams.oPyramidParams[i].nRegionGrowingMode);
  oParams.oPyramidParams[i].nRegionGrowingIterations =
      oConfig.get<int>("PyramidParams[0].nRegionGrowingIterations",
                       oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oParams.oPyramidParams[i].bMedian = oConfig.get<bool>(
      "PyramidParams[0].bMedian", oParams.oPyramidParams[i].bMedian);
  oParams.oPyramidParams[i].iMedX = oConfig.get<int>(
      "PyramidParams[0].iMedX", oParams.oPyramidParams[i].iMedX);
  oParams.oPyramidParams[i].iMedY = oConfig.get<int>(
      "PyramidParams[0].iMedY", oParams.oPyramidParams[i].iMedY);
  oParams.oPyramidParams[i].bSpeckle = oConfig.get<bool>(
      "PyramidParams[0].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oParams.oPyramidParams[i].iNodataPol = oConfig.get<int>(
      "PyramidParams[0].iNodataPol", oParams.oPyramidParams[i].iNodataPol);
  oParams.oPyramidParams[i].iBorderPol = oConfig.get<int>(
      "PyramidParams[0].iBorderPol", oParams.oPyramidParams[i].iBorderPol);
  oParams.oPyramidParams[i].fMedianThresh =
      oConfig.get<float>("PyramidParams[0].fMedianThresh",
                         oParams.oPyramidParams[i].fMedianThresh);
  oParams.oPyramidParams[i].bSplittedMedian =
      oConfig.get<bool>("PyramidParams[0].bSplittedMedian",
                        oParams.oPyramidParams[i].bSplittedMedian);
  oParams.oPyramidParams[i].iSpeckleMaxSize =
      oConfig.get<int>("PyramidParams[0].iSpeckleMaxSize",
                       oParams.oPyramidParams[i].iSpeckleMaxSize);
  oParams.oPyramidParams[i].fSpeckleMaxDiff =
      oConfig.get<float>("PyramidParams[0].fSpeckleMaxDiff",
                         oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oParams.oPyramidParams[i].bInterpol = oConfig.get<bool>(
      "PyramidParams[0].bInterpol", oParams.oPyramidParams[i].bInterpol);
  oParams.oPyramidParams[i].nDir =
      oConfig.get<int>("PyramidParams[0].nDir", oParams.oPyramidParams[i].nDir);
  oParams.oPyramidParams[i].nNth =
      oConfig.get<int>("PyramidParams[0].nNth", oParams.oPyramidParams[i].nNth);
  oParams.oPyramidParams[i].nMinNth = oConfig.get<int>(
      "PyramidParams[0].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oParams.oPyramidParams[i].fRadius = oConfig.get<float>(
      "PyramidParams[0].fRadius", oParams.oPyramidParams[i].fRadius);
  oParams.oPyramidParams[i].nMode = oConfig.get<int>(
      "PyramidParams[0].nMode", oParams.oPyramidParams[i].nMode);
  oParams.oPyramidParams[i].bOccInterpol = oConfig.get<bool>(
      "PyramidParams[0].bOccInterpol", oParams.oPyramidParams[i].bOccInterpol);
  oParams.oPyramidParams[i].nOccDir = oConfig.get<int>(
      "PyramidParams[0].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oParams.oPyramidParams[i].nOccNth = oConfig.get<int>(
      "PyramidParams[0].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oParams.oPyramidParams[i].nOccMinNth = oConfig.get<int>(
      "PyramidParams[0].nOccMinNth", oParams.oPyramidParams[i].nOccMinNth);
  oParams.oPyramidParams[i].fOccRadius = oConfig.get<float>(
      "PyramidParams[0].fOccRadius", oParams.oPyramidParams[i].fOccRadius);
  oParams.oPyramidParams[i].nOccMode = oConfig.get<int>(
      "PyramidParams[0].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeX =
      oConfig.get<int>("PyramidParams[0].oMetricParams.nWindowSizeX",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeY =
      oConfig.get<int>("PyramidParams[0].oMetricParams.nWindowSizeY",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX =
      oConfig.get<float>("PyramidParams[0].oMetricParams.fMetricDeltaX",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY =
      oConfig.get<float>("PyramidParams[0].oMetricParams.fMetricDeltaY",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction =
      oConfig.get<int>(
          "PyramidParams[0].oMetricParams.nCensusCenterFunction",
          oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfScales =
      oConfig.get<int>("PyramidParams[0].oMetricParams.nNumberOfScales",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oParams.oPyramidParams[i].oMetricParams.fScaleStepSize = oConfig.get<float>(
      "PyramidParams[0].oMetricParams.fScaleStepSize",
      oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants =
      oConfig.get<int>("PyramidParams[0].oMetricParams.nNumberOfSlants",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oParams.oPyramidParams[i].oMetricParams.fSlantStepSize = oConfig.get<float>(
      "PyramidParams[0].oMetricParams.fSlantStepSize",
      oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight =
      oConfig.get<bool>(
          "PyramidParams[0].oMetricParams.bUseProximityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oParams.oPyramidParams[i].oMetricParams.fProximityGamma = oConfig.get<float>(
      "PyramidParams[0].oMetricParams.fProximityGamma",
      oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight =
      oConfig.get<bool>(
          "PyramidParams[0].oMetricParams.bUseSimilarityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma = oConfig.get<float>(
      "PyramidParams[0].oMetricParams.fSimilarityGamma",
      oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oParams.oPyramidParams[i].oSGMParams.fCostNodataValue =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fCostNodataValue",
                         oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount =
      oConfig.get<int>(
          "PyramidParams[0].oSGMParams.nAllowedOutsideImageMatchCount",
          oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection = oConfig.get<bool>(
      "PyramidParams[0].oSGMParams.bDSICountCorrection",
      oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oParams.oPyramidParams[i]
      .oSGMParams.nDSICountCorrectionNoDataCheckMode = oConfig.get<int>(
      "PyramidParams[0].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oParams.oPyramidParams[i].oSGMParams.bMatch_S_N =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bMatch_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP1_S_N =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP1_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP2_S_N =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP2_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bP2EqualP1_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N =
      oConfig.get<int>("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N =
      oConfig.get<int>("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bAdditive_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oParams.oPyramidParams[i].oSGMParams.bMatch_E_W =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bMatch_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP1_E_W =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP1_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP2_E_W =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP2_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bP2EqualP1_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W =
      oConfig.get<int>("PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W =
      oConfig.get<int>("PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bAdditive_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bMatch_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP1_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP2_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bP2EqualP1_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bAdditive_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bMatch_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP1_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW =
      oConfig.get<float>("PyramidParams[0].oSGMParams.fP2_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bP2EqualP1_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[0].oSGMParams.nAdaptiveP1Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[0].oSGMParams.nAdaptiveP2Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bAdditive_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bUseDSITexture =
      oConfig.get<bool>("PyramidParams[0].oSGMParams.bUseDSITexture",
                        oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture =
      oConfig.get<bool>(
          "PyramidParams[0].oSGMParams.bUseAggregatedCostTexture",
          oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  i = 1;
  oParams.oPyramidParams[i].bCompute = oConfig.get<bool>(
      "PyramidParams[1].bCompute", oParams.oPyramidParams[i].bCompute);
  oParams.oPyramidParams[i].nMetric = oConfig.get<int>(
      "PyramidParams[1].nMetric", oParams.oPyramidParams[i].nMetric);
  oParams.oPyramidParams[i].nMaximumNumberOfDisparities =
      oConfig.get<int>("PyramidParams[1].nMaximumNumberOfDisparities",
                       oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oParams.oPyramidParams[i].fDisparityStepSize =
      oConfig.get<float>("PyramidParams[1].fDisparityStepSize",
                         oParams.oPyramidParams[i].fDisparityStepSize);
  oParams.oPyramidParams[i].nSubpixelFnc = oConfig.get<int>(
      "PyramidParams[1].nSubpixelFnc", oParams.oPyramidParams[i].nSubpixelFnc);
  oParams.oPyramidParams[i].fMaxCost = oConfig.get<float>(
      "PyramidParams[1].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oParams.oPyramidParams[i].bDoSubpixelNormalization =
      oConfig.get<bool>("PyramidParams[1].bDoSubpixelNormalization",
                        oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oParams.oPyramidParams[i].bBackMatch = oConfig.get<bool>(
      "PyramidParams[1].bBackMatch", oParams.oPyramidParams[i].bBackMatch);
  oParams.oPyramidParams[i].bComputeBackDisparity =
      oConfig.get<bool>("PyramidParams[1].bComputeBackDisparity",
                        oParams.oPyramidParams[i].bComputeBackDisparity);
  oParams.oPyramidParams[i].fMaxBackMatchingDist =
      oConfig.get<float>("PyramidParams[1].fMaxBackMatchingDist",
                         oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch = oConfig.get<bool>(
      "PyramidParams[1].bKeepValueOnNodataInBackmatch",
      oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oParams.oPyramidParams[i].bKeepValueOnOutsideImage =
      oConfig.get<bool>("PyramidParams[1].bKeepValueOnOutsideImage",
                        oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oParams.oPyramidParams[i].bOcclusionDetection =
      oConfig.get<bool>("PyramidParams[1].bOcclusionDetection",
                        oParams.oPyramidParams[i].bOcclusionDetection);
  oParams.oPyramidParams[i].fOcclusionDispGapSize =
      oConfig.get<float>("PyramidParams[1].fOcclusionDispGapSize",
                         oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oParams.oPyramidParams[i].nRegionGrowingMode =
      oConfig.get<int>("PyramidParams[1].nRegionGrowingMode",
                       oParams.oPyramidParams[i].nRegionGrowingMode);
  oParams.oPyramidParams[i].nRegionGrowingIterations =
      oConfig.get<int>("PyramidParams[1].nRegionGrowingIterations",
                       oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oParams.oPyramidParams[i].bMedian = oConfig.get<bool>(
      "PyramidParams[1].bMedian", oParams.oPyramidParams[i].bMedian);
  oParams.oPyramidParams[i].iMedX = oConfig.get<int>(
      "PyramidParams[1].iMedX", oParams.oPyramidParams[i].iMedX);
  oParams.oPyramidParams[i].iMedY = oConfig.get<int>(
      "PyramidParams[1].iMedY", oParams.oPyramidParams[i].iMedY);
  oParams.oPyramidParams[i].bSpeckle = oConfig.get<bool>(
      "PyramidParams[1].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oParams.oPyramidParams[i].iNodataPol = oConfig.get<int>(
      "PyramidParams[1].iNodataPol", oParams.oPyramidParams[i].iNodataPol);
  oParams.oPyramidParams[i].iBorderPol = oConfig.get<int>(
      "PyramidParams[1].iBorderPol", oParams.oPyramidParams[i].iBorderPol);
  oParams.oPyramidParams[i].fMedianThresh =
      oConfig.get<float>("PyramidParams[1].fMedianThresh",
                         oParams.oPyramidParams[i].fMedianThresh);
  oParams.oPyramidParams[i].bSplittedMedian =
      oConfig.get<bool>("PyramidParams[1].bSplittedMedian",
                        oParams.oPyramidParams[i].bSplittedMedian);
  oParams.oPyramidParams[i].iSpeckleMaxSize =
      oConfig.get<int>("PyramidParams[1].iSpeckleMaxSize",
                       oParams.oPyramidParams[i].iSpeckleMaxSize);
  oParams.oPyramidParams[i].fSpeckleMaxDiff =
      oConfig.get<float>("PyramidParams[1].fSpeckleMaxDiff",
                         oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oParams.oPyramidParams[i].bInterpol = oConfig.get<bool>(
      "PyramidParams[1].bInterpol", oParams.oPyramidParams[i].bInterpol);
  oParams.oPyramidParams[i].nDir =
      oConfig.get<int>("PyramidParams[1].nDir", oParams.oPyramidParams[i].nDir);
  oParams.oPyramidParams[i].nNth =
      oConfig.get<int>("PyramidParams[1].nNth", oParams.oPyramidParams[i].nNth);
  oParams.oPyramidParams[i].nMinNth = oConfig.get<int>(
      "PyramidParams[1].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oParams.oPyramidParams[i].fRadius = oConfig.get<float>(
      "PyramidParams[1].fRadius", oParams.oPyramidParams[i].fRadius);
  oParams.oPyramidParams[i].nMode = oConfig.get<int>(
      "PyramidParams[1].nMode", oParams.oPyramidParams[i].nMode);
  oParams.oPyramidParams[i].bOccInterpol = oConfig.get<bool>(
      "PyramidParams[1].bOccInterpol", oParams.oPyramidParams[i].bOccInterpol);
  oParams.oPyramidParams[i].nOccDir = oConfig.get<int>(
      "PyramidParams[1].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oParams.oPyramidParams[i].nOccNth = oConfig.get<int>(
      "PyramidParams[1].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oParams.oPyramidParams[i].nOccMinNth = oConfig.get<int>(
      "PyramidParams[1].nOccMinNth", oParams.oPyramidParams[i].nOccMinNth);
  oParams.oPyramidParams[i].fOccRadius = oConfig.get<float>(
      "PyramidParams[1].fOccRadius", oParams.oPyramidParams[i].fOccRadius);
  oParams.oPyramidParams[i].nOccMode = oConfig.get<int>(
      "PyramidParams[1].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeX =
      oConfig.get<int>("PyramidParams[1].oMetricParams.nWindowSizeX",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeY =
      oConfig.get<int>("PyramidParams[1].oMetricParams.nWindowSizeY",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX =
      oConfig.get<float>("PyramidParams[1].oMetricParams.fMetricDeltaX",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY =
      oConfig.get<float>("PyramidParams[1].oMetricParams.fMetricDeltaY",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction =
      oConfig.get<int>(
          "PyramidParams[1].oMetricParams.nCensusCenterFunction",
          oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfScales =
      oConfig.get<int>("PyramidParams[1].oMetricParams.nNumberOfScales",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oParams.oPyramidParams[i].oMetricParams.fScaleStepSize = oConfig.get<float>(
      "PyramidParams[1].oMetricParams.fScaleStepSize",
      oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants =
      oConfig.get<int>("PyramidParams[1].oMetricParams.nNumberOfSlants",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oParams.oPyramidParams[i].oMetricParams.fSlantStepSize = oConfig.get<float>(
      "PyramidParams[1].oMetricParams.fSlantStepSize",
      oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight =
      oConfig.get<bool>(
          "PyramidParams[1].oMetricParams.bUseProximityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oParams.oPyramidParams[i].oMetricParams.fProximityGamma = oConfig.get<float>(
      "PyramidParams[1].oMetricParams.fProximityGamma",
      oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight =
      oConfig.get<bool>(
          "PyramidParams[1].oMetricParams.bUseSimilarityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma = oConfig.get<float>(
      "PyramidParams[1].oMetricParams.fSimilarityGamma",
      oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oParams.oPyramidParams[i].oSGMParams.fCostNodataValue =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fCostNodataValue",
                         oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount =
      oConfig.get<int>(
          "PyramidParams[1].oSGMParams.nAllowedOutsideImageMatchCount",
          oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection = oConfig.get<bool>(
      "PyramidParams[1].oSGMParams.bDSICountCorrection",
      oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oParams.oPyramidParams[i]
      .oSGMParams.nDSICountCorrectionNoDataCheckMode = oConfig.get<int>(
      "PyramidParams[1].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oParams.oPyramidParams[i].oSGMParams.bMatch_S_N =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bMatch_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP1_S_N =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP1_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP2_S_N =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP2_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bP2EqualP1_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N =
      oConfig.get<int>("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N =
      oConfig.get<int>("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bAdditive_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oParams.oPyramidParams[i].oSGMParams.bMatch_E_W =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bMatch_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP1_E_W =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP1_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP2_E_W =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP2_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bP2EqualP1_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W =
      oConfig.get<int>("PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W =
      oConfig.get<int>("PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bAdditive_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bMatch_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP1_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP2_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bP2EqualP1_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bAdditive_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bMatch_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP1_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW =
      oConfig.get<float>("PyramidParams[1].oSGMParams.fP2_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bP2EqualP1_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[1].oSGMParams.nAdaptiveP1Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[1].oSGMParams.nAdaptiveP2Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bAdditive_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bUseDSITexture =
      oConfig.get<bool>("PyramidParams[1].oSGMParams.bUseDSITexture",
                        oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture =
      oConfig.get<bool>(
          "PyramidParams[1].oSGMParams.bUseAggregatedCostTexture",
          oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  i = 2;
  oParams.oPyramidParams[i].bCompute = oConfig.get<bool>(
      "PyramidParams[2].bCompute", oParams.oPyramidParams[i].bCompute);
  oParams.oPyramidParams[i].nMetric = oConfig.get<int>(
      "PyramidParams[2].nMetric", oParams.oPyramidParams[i].nMetric);
  oParams.oPyramidParams[i].nMaximumNumberOfDisparities =
      oConfig.get<int>("PyramidParams[2].nMaximumNumberOfDisparities",
                       oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oParams.oPyramidParams[i].fDisparityStepSize =
      oConfig.get<float>("PyramidParams[2].fDisparityStepSize",
                         oParams.oPyramidParams[i].fDisparityStepSize);
  oParams.oPyramidParams[i].nSubpixelFnc = oConfig.get<int>(
      "PyramidParams[2].nSubpixelFnc", oParams.oPyramidParams[i].nSubpixelFnc);
  oParams.oPyramidParams[i].fMaxCost = oConfig.get<float>(
      "PyramidParams[2].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oParams.oPyramidParams[i].bDoSubpixelNormalization =
      oConfig.get<bool>("PyramidParams[2].bDoSubpixelNormalization",
                        oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oParams.oPyramidParams[i].bBackMatch = oConfig.get<bool>(
      "PyramidParams[2].bBackMatch", oParams.oPyramidParams[i].bBackMatch);
  oParams.oPyramidParams[i].bComputeBackDisparity =
      oConfig.get<bool>("PyramidParams[2].bComputeBackDisparity",
                        oParams.oPyramidParams[i].bComputeBackDisparity);
  oParams.oPyramidParams[i].fMaxBackMatchingDist =
      oConfig.get<float>("PyramidParams[2].fMaxBackMatchingDist",
                         oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch = oConfig.get<bool>(
      "PyramidParams[2].bKeepValueOnNodataInBackmatch",
      oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oParams.oPyramidParams[i].bKeepValueOnOutsideImage =
      oConfig.get<bool>("PyramidParams[2].bKeepValueOnOutsideImage",
                        oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oParams.oPyramidParams[i].bOcclusionDetection =
      oConfig.get<bool>("PyramidParams[2].bOcclusionDetection",
                        oParams.oPyramidParams[i].bOcclusionDetection);
  oParams.oPyramidParams[i].fOcclusionDispGapSize =
      oConfig.get<float>("PyramidParams[2].fOcclusionDispGapSize",
                         oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oParams.oPyramidParams[i].nRegionGrowingMode =
      oConfig.get<int>("PyramidParams[2].nRegionGrowingMode",
                       oParams.oPyramidParams[i].nRegionGrowingMode);
  oParams.oPyramidParams[i].nRegionGrowingIterations =
      oConfig.get<int>("PyramidParams[2].nRegionGrowingIterations",
                       oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oParams.oPyramidParams[i].bMedian = oConfig.get<bool>(
      "PyramidParams[2].bMedian", oParams.oPyramidParams[i].bMedian);
  oParams.oPyramidParams[i].iMedX = oConfig.get<int>(
      "PyramidParams[2].iMedX", oParams.oPyramidParams[i].iMedX);
  oParams.oPyramidParams[i].iMedY = oConfig.get<int>(
      "PyramidParams[2].iMedY", oParams.oPyramidParams[i].iMedY);
  oParams.oPyramidParams[i].bSpeckle = oConfig.get<bool>(
      "PyramidParams[2].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oParams.oPyramidParams[i].iNodataPol = oConfig.get<int>(
      "PyramidParams[2].iNodataPol", oParams.oPyramidParams[i].iNodataPol);
  oParams.oPyramidParams[i].iBorderPol = oConfig.get<int>(
      "PyramidParams[2].iBorderPol", oParams.oPyramidParams[i].iBorderPol);
  oParams.oPyramidParams[i].fMedianThresh =
      oConfig.get<float>("PyramidParams[2].fMedianThresh",
                         oParams.oPyramidParams[i].fMedianThresh);
  oParams.oPyramidParams[i].bSplittedMedian =
      oConfig.get<bool>("PyramidParams[2].bSplittedMedian",
                        oParams.oPyramidParams[i].bSplittedMedian);
  oParams.oPyramidParams[i].iSpeckleMaxSize =
      oConfig.get<int>("PyramidParams[2].iSpeckleMaxSize",
                       oParams.oPyramidParams[i].iSpeckleMaxSize);
  oParams.oPyramidParams[i].fSpeckleMaxDiff =
      oConfig.get<float>("PyramidParams[2].fSpeckleMaxDiff",
                         oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oParams.oPyramidParams[i].bInterpol = oConfig.get<bool>(
      "PyramidParams[2].bInterpol", oParams.oPyramidParams[i].bInterpol);
  oParams.oPyramidParams[i].nDir =
      oConfig.get<int>("PyramidParams[2].nDir", oParams.oPyramidParams[i].nDir);
  oParams.oPyramidParams[i].nNth =
      oConfig.get<int>("PyramidParams[2].nNth", oParams.oPyramidParams[i].nNth);
  oParams.oPyramidParams[i].nMinNth = oConfig.get<int>(
      "PyramidParams[2].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oParams.oPyramidParams[i].fRadius = oConfig.get<float>(
      "PyramidParams[2].fRadius", oParams.oPyramidParams[i].fRadius);
  oParams.oPyramidParams[i].nMode = oConfig.get<int>(
      "PyramidParams[2].nMode", oParams.oPyramidParams[i].nMode);
  oParams.oPyramidParams[i].bOccInterpol = oConfig.get<bool>(
      "PyramidParams[2].bOccInterpol", oParams.oPyramidParams[i].bOccInterpol);
  oParams.oPyramidParams[i].nOccDir = oConfig.get<int>(
      "PyramidParams[2].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oParams.oPyramidParams[i].nOccNth = oConfig.get<int>(
      "PyramidParams[2].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oParams.oPyramidParams[i].nOccMinNth = oConfig.get<int>(
      "PyramidParams[2].nOccMinNth", oParams.oPyramidParams[i].nOccMinNth);
  oParams.oPyramidParams[i].fOccRadius = oConfig.get<float>(
      "PyramidParams[2].fOccRadius", oParams.oPyramidParams[i].fOccRadius);
  oParams.oPyramidParams[i].nOccMode = oConfig.get<int>(
      "PyramidParams[2].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeX =
      oConfig.get<int>("PyramidParams[2].oMetricParams.nWindowSizeX",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeY =
      oConfig.get<int>("PyramidParams[2].oMetricParams.nWindowSizeY",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX =
      oConfig.get<float>("PyramidParams[2].oMetricParams.fMetricDeltaX",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY =
      oConfig.get<float>("PyramidParams[2].oMetricParams.fMetricDeltaY",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction =
      oConfig.get<int>(
          "PyramidParams[2].oMetricParams.nCensusCenterFunction",
          oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfScales =
      oConfig.get<int>("PyramidParams[2].oMetricParams.nNumberOfScales",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oParams.oPyramidParams[i].oMetricParams.fScaleStepSize = oConfig.get<float>(
      "PyramidParams[2].oMetricParams.fScaleStepSize",
      oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants =
      oConfig.get<int>("PyramidParams[2].oMetricParams.nNumberOfSlants",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oParams.oPyramidParams[i].oMetricParams.fSlantStepSize = oConfig.get<float>(
      "PyramidParams[2].oMetricParams.fSlantStepSize",
      oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight =
      oConfig.get<bool>(
          "PyramidParams[2].oMetricParams.bUseProximityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oParams.oPyramidParams[i].oMetricParams.fProximityGamma = oConfig.get<float>(
      "PyramidParams[2].oMetricParams.fProximityGamma",
      oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight =
      oConfig.get<bool>(
          "PyramidParams[2].oMetricParams.bUseSimilarityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma = oConfig.get<float>(
      "PyramidParams[2].oMetricParams.fSimilarityGamma",
      oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oParams.oPyramidParams[i].oSGMParams.fCostNodataValue =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fCostNodataValue",
                         oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount =
      oConfig.get<int>(
          "PyramidParams[2].oSGMParams.nAllowedOutsideImageMatchCount",
          oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection = oConfig.get<bool>(
      "PyramidParams[2].oSGMParams.bDSICountCorrection",
      oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oParams.oPyramidParams[i]
      .oSGMParams.nDSICountCorrectionNoDataCheckMode = oConfig.get<int>(
      "PyramidParams[2].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oParams.oPyramidParams[i].oSGMParams.bMatch_S_N =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bMatch_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP1_S_N =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP1_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP2_S_N =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP2_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bP2EqualP1_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N =
      oConfig.get<int>("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N =
      oConfig.get<int>("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bAdditive_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oParams.oPyramidParams[i].oSGMParams.bMatch_E_W =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bMatch_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP1_E_W =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP1_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP2_E_W =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP2_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bP2EqualP1_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W =
      oConfig.get<int>("PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W =
      oConfig.get<int>("PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bAdditive_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bMatch_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP1_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP2_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bP2EqualP1_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bAdditive_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bMatch_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP1_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW =
      oConfig.get<float>("PyramidParams[2].oSGMParams.fP2_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bP2EqualP1_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[2].oSGMParams.nAdaptiveP1Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[2].oSGMParams.nAdaptiveP2Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bAdditive_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bUseDSITexture =
      oConfig.get<bool>("PyramidParams[2].oSGMParams.bUseDSITexture",
                        oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture =
      oConfig.get<bool>(
          "PyramidParams[2].oSGMParams.bUseAggregatedCostTexture",
          oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  i = 3;
  oParams.oPyramidParams[i].bCompute = oConfig.get<bool>(
      "PyramidParams[3].bCompute", oParams.oPyramidParams[i].bCompute);
  oParams.oPyramidParams[i].nMetric = oConfig.get<int>(
      "PyramidParams[3].nMetric", oParams.oPyramidParams[i].nMetric);
  oParams.oPyramidParams[i].nMaximumNumberOfDisparities =
      oConfig.get<int>("PyramidParams[3].nMaximumNumberOfDisparities",
                       oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oParams.oPyramidParams[i].fDisparityStepSize =
      oConfig.get<float>("PyramidParams[3].fDisparityStepSize",
                         oParams.oPyramidParams[i].fDisparityStepSize);
  oParams.oPyramidParams[i].nSubpixelFnc = oConfig.get<int>(
      "PyramidParams[3].nSubpixelFnc", oParams.oPyramidParams[i].nSubpixelFnc);
  oParams.oPyramidParams[i].fMaxCost = oConfig.get<float>(
      "PyramidParams[3].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oParams.oPyramidParams[i].bDoSubpixelNormalization =
      oConfig.get<bool>("PyramidParams[3].bDoSubpixelNormalization",
                        oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oParams.oPyramidParams[i].bBackMatch = oConfig.get<bool>(
      "PyramidParams[3].bBackMatch", oParams.oPyramidParams[i].bBackMatch);
  oParams.oPyramidParams[i].bComputeBackDisparity =
      oConfig.get<bool>("PyramidParams[3].bComputeBackDisparity",
                        oParams.oPyramidParams[i].bComputeBackDisparity);
  oParams.oPyramidParams[i].fMaxBackMatchingDist =
      oConfig.get<float>("PyramidParams[3].fMaxBackMatchingDist",
                         oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch = oConfig.get<bool>(
      "PyramidParams[3].bKeepValueOnNodataInBackmatch",
      oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oParams.oPyramidParams[i].bKeepValueOnOutsideImage =
      oConfig.get<bool>("PyramidParams[3].bKeepValueOnOutsideImage",
                        oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oParams.oPyramidParams[i].bOcclusionDetection =
      oConfig.get<bool>("PyramidParams[3].bOcclusionDetection",
                        oParams.oPyramidParams[i].bOcclusionDetection);
  oParams.oPyramidParams[i].fOcclusionDispGapSize =
      oConfig.get<float>("PyramidParams[3].fOcclusionDispGapSize",
                         oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oParams.oPyramidParams[i].nRegionGrowingMode =
      oConfig.get<int>("PyramidParams[3].nRegionGrowingMode",
                       oParams.oPyramidParams[i].nRegionGrowingMode);
  oParams.oPyramidParams[i].nRegionGrowingIterations =
      oConfig.get<int>("PyramidParams[3].nRegionGrowingIterations",
                       oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oParams.oPyramidParams[i].bMedian = oConfig.get<bool>(
      "PyramidParams[3].bMedian", oParams.oPyramidParams[i].bMedian);
  oParams.oPyramidParams[i].iMedX = oConfig.get<int>(
      "PyramidParams[3].iMedX", oParams.oPyramidParams[i].iMedX);
  oParams.oPyramidParams[i].iMedY = oConfig.get<int>(
      "PyramidParams[3].iMedY", oParams.oPyramidParams[i].iMedY);
  oParams.oPyramidParams[i].bSpeckle = oConfig.get<bool>(
      "PyramidParams[3].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oParams.oPyramidParams[i].iNodataPol = oConfig.get<int>(
      "PyramidParams[3].iNodataPol", oParams.oPyramidParams[i].iNodataPol);
  oParams.oPyramidParams[i].iBorderPol = oConfig.get<int>(
      "PyramidParams[3].iBorderPol", oParams.oPyramidParams[i].iBorderPol);
  oParams.oPyramidParams[i].fMedianThresh =
      oConfig.get<float>("PyramidParams[3].fMedianThresh",
                         oParams.oPyramidParams[i].fMedianThresh);
  oParams.oPyramidParams[i].bSplittedMedian =
      oConfig.get<bool>("PyramidParams[3].bSplittedMedian",
                        oParams.oPyramidParams[i].bSplittedMedian);
  oParams.oPyramidParams[i].iSpeckleMaxSize =
      oConfig.get<int>("PyramidParams[3].iSpeckleMaxSize",
                       oParams.oPyramidParams[i].iSpeckleMaxSize);
  oParams.oPyramidParams[i].fSpeckleMaxDiff =
      oConfig.get<float>("PyramidParams[3].fSpeckleMaxDiff",
                         oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oParams.oPyramidParams[i].bInterpol = oConfig.get<bool>(
      "PyramidParams[3].bInterpol", oParams.oPyramidParams[i].bInterpol);
  oParams.oPyramidParams[i].nDir =
      oConfig.get<int>("PyramidParams[3].nDir", oParams.oPyramidParams[i].nDir);
  oParams.oPyramidParams[i].nNth =
      oConfig.get<int>("PyramidParams[3].nNth", oParams.oPyramidParams[i].nNth);
  oParams.oPyramidParams[i].nMinNth = oConfig.get<int>(
      "PyramidParams[3].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oParams.oPyramidParams[i].fRadius = oConfig.get<float>(
      "PyramidParams[3].fRadius", oParams.oPyramidParams[i].fRadius);
  oParams.oPyramidParams[i].nMode = oConfig.get<int>(
      "PyramidParams[3].nMode", oParams.oPyramidParams[i].nMode);
  oParams.oPyramidParams[i].bOccInterpol = oConfig.get<bool>(
      "PyramidParams[3].bOccInterpol", oParams.oPyramidParams[i].bOccInterpol);
  oParams.oPyramidParams[i].nOccDir = oConfig.get<int>(
      "PyramidParams[3].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oParams.oPyramidParams[i].nOccNth = oConfig.get<int>(
      "PyramidParams[3].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oParams.oPyramidParams[i].nOccMinNth = oConfig.get<int>(
      "PyramidParams[3].nOccMinNth", oParams.oPyramidParams[i].nOccMinNth);
  oParams.oPyramidParams[i].fOccRadius = oConfig.get<float>(
      "PyramidParams[3].fOccRadius", oParams.oPyramidParams[i].fOccRadius);
  oParams.oPyramidParams[i].nOccMode = oConfig.get<int>(
      "PyramidParams[3].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeX =
      oConfig.get<int>("PyramidParams[3].oMetricParams.nWindowSizeX",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeY =
      oConfig.get<int>("PyramidParams[3].oMetricParams.nWindowSizeY",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX =
      oConfig.get<float>("PyramidParams[3].oMetricParams.fMetricDeltaX",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY =
      oConfig.get<float>("PyramidParams[3].oMetricParams.fMetricDeltaY",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction =
      oConfig.get<int>(
          "PyramidParams[3].oMetricParams.nCensusCenterFunction",
          oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfScales =
      oConfig.get<int>("PyramidParams[3].oMetricParams.nNumberOfScales",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oParams.oPyramidParams[i].oMetricParams.fScaleStepSize = oConfig.get<float>(
      "PyramidParams[3].oMetricParams.fScaleStepSize",
      oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants =
      oConfig.get<int>("PyramidParams[3].oMetricParams.nNumberOfSlants",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oParams.oPyramidParams[i].oMetricParams.fSlantStepSize = oConfig.get<float>(
      "PyramidParams[3].oMetricParams.fSlantStepSize",
      oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight =
      oConfig.get<bool>(
          "PyramidParams[3].oMetricParams.bUseProximityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oParams.oPyramidParams[i].oMetricParams.fProximityGamma = oConfig.get<float>(
      "PyramidParams[3].oMetricParams.fProximityGamma",
      oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight =
      oConfig.get<bool>(
          "PyramidParams[3].oMetricParams.bUseSimilarityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma = oConfig.get<float>(
      "PyramidParams[3].oMetricParams.fSimilarityGamma",
      oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oParams.oPyramidParams[i].oSGMParams.fCostNodataValue =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fCostNodataValue",
                         oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount =
      oConfig.get<int>(
          "PyramidParams[3].oSGMParams.nAllowedOutsideImageMatchCount",
          oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection = oConfig.get<bool>(
      "PyramidParams[3].oSGMParams.bDSICountCorrection",
      oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oParams.oPyramidParams[i]
      .oSGMParams.nDSICountCorrectionNoDataCheckMode = oConfig.get<int>(
      "PyramidParams[3].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oParams.oPyramidParams[i].oSGMParams.bMatch_S_N =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bMatch_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP1_S_N =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP1_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP2_S_N =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP2_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bP2EqualP1_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N =
      oConfig.get<int>("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N =
      oConfig.get<int>("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bAdditive_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oParams.oPyramidParams[i].oSGMParams.bMatch_E_W =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bMatch_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP1_E_W =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP1_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP2_E_W =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP2_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bP2EqualP1_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W =
      oConfig.get<int>("PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W =
      oConfig.get<int>("PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bAdditive_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bMatch_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP1_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP2_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bP2EqualP1_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bAdditive_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bMatch_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP1_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW =
      oConfig.get<float>("PyramidParams[3].oSGMParams.fP2_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bP2EqualP1_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[3].oSGMParams.nAdaptiveP1Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[3].oSGMParams.nAdaptiveP2Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bAdditive_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bUseDSITexture =
      oConfig.get<bool>("PyramidParams[3].oSGMParams.bUseDSITexture",
                        oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture =
      oConfig.get<bool>(
          "PyramidParams[3].oSGMParams.bUseAggregatedCostTexture",
          oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  i = 4;
  oParams.oPyramidParams[i].bCompute = oConfig.get<bool>(
      "PyramidParams[4].bCompute", oParams.oPyramidParams[i].bCompute);
  oParams.oPyramidParams[i].nMetric = oConfig.get<int>(
      "PyramidParams[4].nMetric", oParams.oPyramidParams[i].nMetric);
  oParams.oPyramidParams[i].nMaximumNumberOfDisparities =
      oConfig.get<int>("PyramidParams[4].nMaximumNumberOfDisparities",
                       oParams.oPyramidParams[i].nMaximumNumberOfDisparities);
  oParams.oPyramidParams[i].fDisparityStepSize =
      oConfig.get<float>("PyramidParams[4].fDisparityStepSize",
                         oParams.oPyramidParams[i].fDisparityStepSize);
  oParams.oPyramidParams[i].nSubpixelFnc = oConfig.get<int>(
      "PyramidParams[4].nSubpixelFnc", oParams.oPyramidParams[i].nSubpixelFnc);
  oParams.oPyramidParams[i].fMaxCost = oConfig.get<float>(
      "PyramidParams[4].fMaxCost", oParams.oPyramidParams[i].fMaxCost);
  oParams.oPyramidParams[i].bDoSubpixelNormalization =
      oConfig.get<bool>("PyramidParams[4].bDoSubpixelNormalization",
                        oParams.oPyramidParams[i].bDoSubpixelNormalization);
  oParams.oPyramidParams[i].bBackMatch = oConfig.get<bool>(
      "PyramidParams[4].bBackMatch", oParams.oPyramidParams[i].bBackMatch);
  oParams.oPyramidParams[i].bComputeBackDisparity =
      oConfig.get<bool>("PyramidParams[4].bComputeBackDisparity",
                        oParams.oPyramidParams[i].bComputeBackDisparity);
  oParams.oPyramidParams[i].fMaxBackMatchingDist =
      oConfig.get<float>("PyramidParams[4].fMaxBackMatchingDist",
                         oParams.oPyramidParams[i].fMaxBackMatchingDist);
  oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch = oConfig.get<bool>(
      "PyramidParams[4].bKeepValueOnNodataInBackmatch",
      oParams.oPyramidParams[i].bKeepValueOnNodataInBackmatch);
  oParams.oPyramidParams[i].bKeepValueOnOutsideImage =
      oConfig.get<bool>("PyramidParams[4].bKeepValueOnOutsideImage",
                        oParams.oPyramidParams[i].bKeepValueOnOutsideImage);
  oParams.oPyramidParams[i].bOcclusionDetection =
      oConfig.get<bool>("PyramidParams[4].bOcclusionDetection",
                        oParams.oPyramidParams[i].bOcclusionDetection);
  oParams.oPyramidParams[i].fOcclusionDispGapSize =
      oConfig.get<float>("PyramidParams[4].fOcclusionDispGapSize",
                         oParams.oPyramidParams[i].fOcclusionDispGapSize);
  oParams.oPyramidParams[i].nRegionGrowingMode =
      oConfig.get<int>("PyramidParams[4].nRegionGrowingMode",
                       oParams.oPyramidParams[i].nRegionGrowingMode);
  oParams.oPyramidParams[i].nRegionGrowingIterations =
      oConfig.get<int>("PyramidParams[4].nRegionGrowingIterations",
                       oParams.oPyramidParams[i].nRegionGrowingIterations);

  // manually added from GUI
  oParams.oPyramidParams[i].bMedian = oConfig.get<bool>(
      "PyramidParams[4].bMedian", oParams.oPyramidParams[i].bMedian);
  oParams.oPyramidParams[i].iMedX = oConfig.get<int>(
      "PyramidParams[4].iMedX", oParams.oPyramidParams[i].iMedX);
  oParams.oPyramidParams[i].iMedY = oConfig.get<int>(
      "PyramidParams[4].iMedY", oParams.oPyramidParams[i].iMedY);
  oParams.oPyramidParams[i].bSpeckle = oConfig.get<bool>(
      "PyramidParams[4].bSpeckle", oParams.oPyramidParams[i].bSpeckle);
  oParams.oPyramidParams[i].iNodataPol = oConfig.get<int>(
      "PyramidParams[4].iNodataPol", oParams.oPyramidParams[i].iNodataPol);
  oParams.oPyramidParams[i].iBorderPol = oConfig.get<int>(
      "PyramidParams[4].iBorderPol", oParams.oPyramidParams[i].iBorderPol);
  oParams.oPyramidParams[i].fMedianThresh =
      oConfig.get<float>("PyramidParams[4].fMedianThresh",
                         oParams.oPyramidParams[i].fMedianThresh);
  oParams.oPyramidParams[i].bSplittedMedian =
      oConfig.get<bool>("PyramidParams[4].bSplittedMedian",
                        oParams.oPyramidParams[i].bSplittedMedian);
  oParams.oPyramidParams[i].iSpeckleMaxSize =
      oConfig.get<int>("PyramidParams[4].iSpeckleMaxSize",
                       oParams.oPyramidParams[i].iSpeckleMaxSize);
  oParams.oPyramidParams[i].fSpeckleMaxDiff =
      oConfig.get<float>("PyramidParams[4].fSpeckleMaxDiff",
                         oParams.oPyramidParams[i].fSpeckleMaxDiff);
  oParams.oPyramidParams[i].bInterpol = oConfig.get<bool>(
      "PyramidParams[4].bInterpol", oParams.oPyramidParams[i].bInterpol);
  oParams.oPyramidParams[i].nDir =
      oConfig.get<int>("PyramidParams[4].nDir", oParams.oPyramidParams[i].nDir);
  oParams.oPyramidParams[i].nNth =
      oConfig.get<int>("PyramidParams[4].nNth", oParams.oPyramidParams[i].nNth);
  oParams.oPyramidParams[i].nMinNth = oConfig.get<int>(
      "PyramidParams[4].nMinNth", oParams.oPyramidParams[i].nMinNth);
  oParams.oPyramidParams[i].fRadius = oConfig.get<float>(
      "PyramidParams[4].fRadius", oParams.oPyramidParams[i].fRadius);
  oParams.oPyramidParams[i].nMode = oConfig.get<int>(
      "PyramidParams[4].nMode", oParams.oPyramidParams[i].nMode);
  oParams.oPyramidParams[i].bOccInterpol = oConfig.get<bool>(
      "PyramidParams[4].bOccInterpol", oParams.oPyramidParams[i].bOccInterpol);
  oParams.oPyramidParams[i].nOccDir = oConfig.get<int>(
      "PyramidParams[4].nOccDir", oParams.oPyramidParams[i].nOccDir);
  oParams.oPyramidParams[i].nOccNth = oConfig.get<int>(
      "PyramidParams[4].nOccNth", oParams.oPyramidParams[i].nOccNth);
  oParams.oPyramidParams[i].nOccMinNth = oConfig.get<int>(
      "PyramidParams[4].nOccMinNth", oParams.oPyramidParams[i].nOccMinNth);
  oParams.oPyramidParams[i].fOccRadius = oConfig.get<float>(
      "PyramidParams[4].fOccRadius", oParams.oPyramidParams[i].fOccRadius);
  oParams.oPyramidParams[i].nOccMode = oConfig.get<int>(
      "PyramidParams[4].nOccMode", oParams.oPyramidParams[i].nOccMode);

  // oPyramidParams 0 oMetricParcParams
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeX =
      oConfig.get<int>("PyramidParams[4].oMetricParams.nWindowSizeX",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeX);
  oParams.oPyramidParams[i].oMetricParams.nWindowSizeY =
      oConfig.get<int>("PyramidParams[4].oMetricParams.nWindowSizeY",
                       oParams.oPyramidParams[i].oMetricParams.nWindowSizeY);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX =
      oConfig.get<float>("PyramidParams[4].oMetricParams.fMetricDeltaX",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaX);
  oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY =
      oConfig.get<float>("PyramidParams[4].oMetricParams.fMetricDeltaY",
                         oParams.oPyramidParams[i].oMetricParams.fMetricDeltaY);
  oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction =
      oConfig.get<int>(
          "PyramidParams[4].oMetricParams.nCensusCenterFunction",
          oParams.oPyramidParams[i].oMetricParams.nCensusCenterFunction);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfScales =
      oConfig.get<int>("PyramidParams[4].oMetricParams.nNumberOfScales",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfScales);
  oParams.oPyramidParams[i].oMetricParams.fScaleStepSize = oConfig.get<float>(
      "PyramidParams[4].oMetricParams.fScaleStepSize",
      oParams.oPyramidParams[i].oMetricParams.fScaleStepSize);
  oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants =
      oConfig.get<int>("PyramidParams[4].oMetricParams.nNumberOfSlants",
                       oParams.oPyramidParams[i].oMetricParams.nNumberOfSlants);
  oParams.oPyramidParams[i].oMetricParams.fSlantStepSize = oConfig.get<float>(
      "PyramidParams[4].oMetricParams.fSlantStepSize",
      oParams.oPyramidParams[i].oMetricParams.fSlantStepSize);
  oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight =
      oConfig.get<bool>(
          "PyramidParams[4].oMetricParams.bUseProximityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseProximityWeight);
  oParams.oPyramidParams[i].oMetricParams.fProximityGamma = oConfig.get<float>(
      "PyramidParams[4].oMetricParams.fProximityGamma",
      oParams.oPyramidParams[i].oMetricParams.fProximityGamma);
  oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight =
      oConfig.get<bool>(
          "PyramidParams[4].oMetricParams.bUseSimilarityWeight",
          oParams.oPyramidParams[i].oMetricParams.bUseSimilarityWeight);
  oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma = oConfig.get<float>(
      "PyramidParams[4].oMetricParams.fSimilarityGamma",
      oParams.oPyramidParams[i].oMetricParams.fSimilarityGamma);

  // oPyramidParams 0 oSGMParamsrams
  oParams.oPyramidParams[i].oSGMParams.fCostNodataValue =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fCostNodataValue",
                         oParams.oPyramidParams[i].oSGMParams.fCostNodataValue);
  oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount =
      oConfig.get<int>(
          "PyramidParams[4].oSGMParams.nAllowedOutsideImageMatchCount",
          oParams.oPyramidParams[i].oSGMParams.nAllowedOutsideImageMatchCount);
  oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection = oConfig.get<bool>(
      "PyramidParams[4].oSGMParams.bDSICountCorrection",
      oParams.oPyramidParams[i].oSGMParams.bDSICountCorrection);
  oParams.oPyramidParams[i]
      .oSGMParams.nDSICountCorrectionNoDataCheckMode = oConfig.get<int>(
      "PyramidParams[4].oSGMParams.nDSICountCorrectionNoDataCheckMode",
      oParams.oPyramidParams[i].oSGMParams.nDSICountCorrectionNoDataCheckMode);
  oParams.oPyramidParams[i].oSGMParams.bMatch_S_N =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bMatch_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP1_S_N =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP1_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.fP2_S_N =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP2_S_N",
                         oParams.oPyramidParams[i].oSGMParams.fP2_S_N);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bP2EqualP1_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N =
      oConfig.get<int>("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N =
      oConfig.get<int>("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_S_N",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_S_N);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bAdditive_S_N",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_S_N);
  oParams.oPyramidParams[i].oSGMParams.bMatch_E_W =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bMatch_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP1_E_W =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP1_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.fP2_E_W =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP2_E_W",
                         oParams.oPyramidParams[i].oSGMParams.fP2_E_W);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bP2EqualP1_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W =
      oConfig.get<int>("PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W =
      oConfig.get<int>("PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_E_W",
                       oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_E_W);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bAdditive_E_W",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_E_W);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bMatch_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP1_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP2_SW_NE",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bP2EqualP1_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE = oConfig.get<int>(
      "PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_SW_NE",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bAdditive_SW_NE",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SW_NE);
  oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bMatch_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bMatch_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP1_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW =
      oConfig.get<float>("PyramidParams[4].oSGMParams.fP2_SE_NW",
                         oParams.oPyramidParams[i].oSGMParams.fP2_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bP2EqualP1_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bP2EqualP1_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[4].oSGMParams.nAdaptiveP1Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP1Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW = oConfig.get<int>(
      "PyramidParams[4].oSGMParams.nAdaptiveP2Fnc_SE_NW",
      oParams.oPyramidParams[i].oSGMParams.nAdaptiveP2Fnc_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bAdditive_SE_NW",
                        oParams.oPyramidParams[i].oSGMParams.bAdditive_SE_NW);
  oParams.oPyramidParams[i].oSGMParams.bUseDSITexture =
      oConfig.get<bool>("PyramidParams[4].oSGMParams.bUseDSITexture",
                        oParams.oPyramidParams[i].oSGMParams.bUseDSITexture);
  oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture =
      oConfig.get<bool>(
          "PyramidParams[4].oSGMParams.bUseAggregatedCostTexture",
          oParams.oPyramidParams[i].oSGMParams.bUseAggregatedCostTexture);

  // oFinalSubPixelParameters
  oParams.oFinalSubPixelParameters.bCompute =
      oConfig.get<bool>("SubpixelPyramidParams.bCompute",
                        oParams.oFinalSubPixelParameters.bCompute);
  oParams.oFinalSubPixelParameters.nMetric =
      oConfig.get<int>("SubpixelPyramidParams.nMetric",
                       oParams.oFinalSubPixelParameters.nMetric);
  oParams.oFinalSubPixelParameters.nMaximumNumberOfDisparities =
      oConfig.get<int>(
          "SubpixelPyramidParams.nMaximumNumberOfDisparities",
          oParams.oFinalSubPixelParameters.nMaximumNumberOfDisparities);
  oParams.oFinalSubPixelParameters.fDisparityStepSize =
      oConfig.get<float>("SubpixelPyramidParams.fDisparityStepSize",
                         oParams.oFinalSubPixelParameters.fDisparityStepSize);
  oParams.oFinalSubPixelParameters.nSubpixelFnc =
      oConfig.get<int>("SubpixelPyramidParams.nSubpixelFnc",
                       oParams.oFinalSubPixelParameters.nSubpixelFnc);
  oParams.oFinalSubPixelParameters.fMaxCost =
      oConfig.get<float>("SubpixelPyramidParams.fMaxCost",
                         oParams.oFinalSubPixelParameters.fMaxCost);
  oParams.oFinalSubPixelParameters.bDoSubpixelNormalization = oConfig.get<bool>(
      "SubpixelPyramidParams.bDoSubpixelNormalization",
      oParams.oFinalSubPixelParameters.bDoSubpixelNormalization);
  oParams.oFinalSubPixelParameters.bBackMatch =
      oConfig.get<bool>("SubpixelPyramidParams.bBackMatch",
                        oParams.oFinalSubPixelParameters.bBackMatch);
  oParams.oFinalSubPixelParameters.bComputeBackDisparity =
      oConfig.get<bool>("SubpixelPyramidParams.bComputeBackDisparity",
                        oParams.oFinalSubPixelParameters.bComputeBackDisparity);
  oParams.oFinalSubPixelParameters.fMaxBackMatchingDist =
      oConfig.get<float>("SubpixelPyramidParams.fMaxBackMatchingDist",
                         oParams.oFinalSubPixelParameters.fMaxBackMatchingDist);
  oParams.oFinalSubPixelParameters.bKeepValueOnNodataInBackmatch =
      oConfig.get<bool>(
          "SubpixelPyramidParams.bKeepValueOnNodataInBackmatch",
          oParams.oFinalSubPixelParameters.bKeepValueOnNodataInBackmatch);
  oParams.oFinalSubPixelParameters.bKeepValueOnOutsideImage = oConfig.get<bool>(
      "SubpixelPyramidParams.bKeepValueOnOutsideImage",
      oParams.oFinalSubPixelParameters.bKeepValueOnOutsideImage);
  oParams.oFinalSubPixelParameters.bOcclusionDetection =
      oConfig.get<bool>("SubpixelPyramidParams.bOcclusionDetection",
                        oParams.oFinalSubPixelParameters.bOcclusionDetection);
  oParams.oFinalSubPixelParameters.fOcclusionDispGapSize = oConfig.get<float>(
      "SubpixelPyramidParams.fOcclusionDispGapSize",
      oParams.oFinalSubPixelParameters.fOcclusionDispGapSize);
  oParams.oFinalSubPixelParameters.nRegionGrowingMode =
      oConfig.get<int>("SubpixelPyramidParams.nRegionGrowingMode",
                       oParams.oFinalSubPixelParameters.nRegionGrowingMode);
  oParams.oFinalSubPixelParameters.nRegionGrowingIterations = oConfig.get<int>(
      "SubpixelPyramidParams.nRegionGrowingIterations",
      oParams.oFinalSubPixelParameters.nRegionGrowingIterations);
}
