/**************************************************************************************************/
//
//    RESPONSIBILITY:  hue
//    COMPILER:        ANSI C++
//    CREATION DATE:   11. 1. 2017
//    MODIFIED DATE:   27.04.2018 (log): Updated to new interface, fixed issues regarding linux compillation
//                     05.08.2019 (log): Added a cfg file reader to read JR's config parameter pool
//
//----------------------------------------------------------------------------------------------------
//
//    Interface for externally integratable dll - for rectification, matching and triangulation
//
//    QS Review ():
//    Design Review():
//
//----------------------------------------------------------------------------------------------------
//
//    Copyright 2019 Joanneum Research ForschungsgesmbH. All rights reserved!
//
/**************************************************************************************************/

//#include "PhobosConfig.hpp"
#include "PhobosIntegration/PhobosIntegrationExport.hpp"
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>


namespace JR
{
namespace Phobos
{

//for licensing information
void JR_PHOBOS_PHOBOSINTEGRATION_EXPORT GetHostInfo( std::string & roHostName, std::string & roHostId );

typedef enum _EVerbosityLevel
{
    e_logNone,     /**< Logging is disabled, do not give any messages */
    e_logFatal,   /**< The message indicates a fatal error condition without any possible to cont */
    e_logError,   /**< Indicates a serious error, which will very likely cause false results */
    e_logWarning, /**< Indicates a problem which might cause problems */
    e_logInfo,    /**< Indicates a normal status info during a process */
    e_logLog,     /**< More detailed information useful for experienced users */
    e_logDebug,   /**< Very high level of detail, causing lots of messages */
    e_logTrace   /**< Highest level of detail, possibly causing a flood of messages */
} EVerbosityLevel;

struct JR_PHOBOS_PHOBOSINTEGRATION_EXPORT SMetricParameters
{
    int nWindowSizeX;
    int nWindowSizeY;
    float fMetricDeltaX;        //usually 1, distance between two pixels in census
    float fMetricDeltaY;        //usually 1, distance between two pixels in census
    int nCensusCenterFunction;  //only for census, 0 for center pixel (normal census), 1 for mean (better on random dot projector like from kinect)
    bool bUseCensusTexture;     //only for census, texture memory is much lower as global GPU memory but faster, if possible use texture memory, if you get out of memory disable it

    int nRotatedWindow;         //0 for disabled (only one census size as specified in window size), 4,8,16 otherwise....(experimental, here we suppose nWindowSizeX!=nWindowSizeY and the window is tried to fit in different rotated variants )

    //slants/scales cannot be used in combination with rotated window!!!!
    int nNumberOfScales; //= 1 if not used (requires much census memory if enabled, check bUseCensusTexture), greatly increases accuracy on flat surfaces
    float fScaleStepSize;
    int nNumberOfSlants; //= 0 if not used (requires much census memory if enabled, check bUseCensusTexture), greatly increases accuracy on flat surfaces
    float fSlantStepSize;

    bool bUseProximityWeight;
    float fProximityGamma;

    bool bUseSimilarityWeight;  //only census
    float fSimilarityGamma;     //only census


};

struct JR_PHOBOS_PHOBOSINTEGRATION_EXPORT SSGMParameters
{
    float fCostNodataValue;
    int nAllowedOutsideImageMatchCount;
    bool bDSICountCorrection;               //subtracts N-1 times (N=number of directions) the base DSI from aggregated cost like on MGM paper
    int nDSICountCorrectionNoDataCheckMode; //0 means nothing (same initialization as all other DSI values with (Ndirections - 1)*DSI as purposed in MGM paper), 
    //1 means nodata is initialized as 0
    //2 means nodata is intitialized as fCostNodataValue
    bool bMatch_S_N;
    float fP1_S_N;
    float fP2_S_N;
    bool bP2EqualP1_S_N;
    int nAdaptiveP1Fnc_S_N; //0 for no adaptive (usually only P2 is adaptive), 1 for rolis adaptive
    int nAdaptiveP2Fnc_S_N; //0 for no adaptive, 1 for rolis adaptive. Use adaptive penalty if you do not have projector pattern!
    bool bAdditive_S_N; //experimental, let it be true!

    bool bMatch_E_W;
    float fP1_E_W;
    float fP2_E_W;
    bool bP2EqualP1_E_W;
    int nAdaptiveP1Fnc_E_W; //0 for no adaptive (usually only P2 is adaptive), 1 for rolis adaptive
    int nAdaptiveP2Fnc_E_W; //0 for no adaptive, 1 for rolis adaptive. Use adaptive penalty if you do not have projector pattern!
    bool bAdditive_E_W;//experimental, let it be true!

    bool bMatch_SW_NE;
    float fP1_SW_NE;
    float fP2_SW_NE;
    bool bP2EqualP1_SW_NE;
    int nAdaptiveP1Fnc_SW_NE; //0 for no adaptive (usually only P2 is adaptive), 1 for rolis adaptive
    int nAdaptiveP2Fnc_SW_NE; //0 for no adaptive, 1 for rolis adaptive. Use adaptive penalty if you do not have projector pattern!
    bool bAdditive_SW_NE;//experimental, let it be true!

    bool bMatch_SE_NW;
    float fP1_SE_NW;
    float fP2_SE_NW;
    bool bP2EqualP1_SE_NW;
    int nAdaptiveP1Fnc_SE_NW; //0 for no adaptive (usually only P2 is adaptive), 1 for rolis adaptive
    int nAdaptiveP2Fnc_SE_NW; //0 for no adaptive, 1 for rolis adaptive. Use adaptive penalty if you do not have projector pattern!
    bool bAdditive_SE_NW;//experimental, let it be true!


    bool bUseDSITexture;            //only performance, texturememory is usually faster here but you can only allocate a limited mount of texture memory
    bool bUseAggregatedCostTexture; //only performance, texturememory is usually faster here but you can only allocate a limited mount of texture memory
    bool bUseCPUSgm;                //use SGM computation on CPU (requires less GPU memory)

};


struct JR_PHOBOS_PHOBOSINTEGRATION_EXPORT SMatchingParameters
{
    bool bCompute; //compute this pyramid (maybe a result of subpixel pyramid is accurate enough, speed issue)

    int nMetric; //0 for census, 1 for absdiff, etc..

    SMetricParameters oMetricParams;

    int nMaximumNumberOfDisparities;//<= 31 very fast, > 31 only fast ;-). Must be odd.

    float fDisparityStepSize;       //Usually 1, for sub pixel can be < 1 means 1 disparity is <1 disparity :-), or for faster computation set > 1
    int nSubpixelFnc;               //0 no subpixel, 1 parabolic, etc (see original)
    float fMaxCost;                 //Cost threshold, set to -1 if you do not want to use it....
    bool bDoSubpixelNormalization;
    SSGMParameters oSGMParams;

    bool bMultiGPU_AfterDSI; //if true also select best cost, occlusion detection etc. is done on multi GPUs, of false only census, DSI creation and SGM is performed on multi gpu's (may faster because of data transfer)

    bool bMedian;          // use median optimizer
    int iMedX;             // median size
    int iMedY;

    bool bSpeckle;         // use speckle optimizer

    int iNodataPol;        //"Disparity Median Optimizer Nodata Policy"    
    int iBorderPol;        //"Disparity Median Optimizer Border Policy"    
    float fMedianThresh;   //"Disparity Median Optimizer Threshold"        
    bool bSplittedMedian;  //"Disparity Median Optimizer Splitted Version" 

    int iSpeckleMaxSize;   //"Disparity Speckle Filter Max Region Size"    
    float fSpeckleMaxDiff; //"Disparity Speckle Filter Max Difference"     
    //back matching parameters:
    bool bBackMatch;
    bool bComputeBackDisparity;

    float fMaxBackMatchingDist;
    bool bKeepValueOnNodataInBackmatch;
    bool bKeepValueOnOutsideImage;

    bool bOcclusionDetection;
    float fOcclusionDispGapSize;
    int nRegionGrowingMode;      //0 no, 1 iterations, 2//full
    int nRegionGrowingIterations;//for nRegionGrowingMode == 1

    bool bInterpol; // "Interpolate Disparity"
    int nDir; // "Interpolator Number Of Directions"         
    int nNth; // "Interpolator Number Nth Elements"          
    int nMinNth; // "Interpolator Minimum Number Of Elements"   
    float fRadius; // "Interpolator Match Radius"                 
    int nMode; // "Interpolator Mode", i recommend Gauss, Min = 0, Max = 1, Linear = 2, Median = 3, Mean = 4, Nearest = 5, Inverse = 6, Gauss = 7, AbsMin = 8, AbsMax = 9, 25P = 10, 75P = 11, 10P = 12, 90P = 13                      

    bool bOccInterpol; // "Interpolate Occlusions"                            
    int nOccDir; // "Occlusion Interpolator Number Of Directions"       
    int nOccNth; // "Occlusion Interpolator Number Nth Elements"        
    int nOccMinNth; // "Occlusion Interpolator Minimum Number Of Elements" 
    float fOccRadius; // "Occlusion Interpolator Match Radius"               
    int nOccMode; // "Occlusion Interpolator Mode"       
};

struct JR_PHOBOS_PHOBOSINTEGRATION_EXPORT SParametersInput
{
    //input
    std::string strPath1;               // left rectified images input directory
    std::string strPath2;               // right rectified images input directory
    int iFirst;                         // first index to use - e.g. 0
    int iLast;                          // last inded to use  - e.g. 100 or 2147483647
    std::string strPrefix1;             // prefix left  e.g. left_
    std::string strPrefix2;             // prefix right e.g. right_
    std::string strPostfix1;            // postfix e.g. file extension .tif
    std::string strPostfix2;            // postfix e.g. file extension .tif
    int iDigits1;                       // amount of digits specifying left  index e.g. 6 for 000010
    int iDigits2;                       // amount of digits specifying right index e.g. 6 for 000010
    bool bHasNodata;                    // is there a specified value for invalid data points
    float fNodataValue;                 // specified value for invalid data points e.g. -99999
    //joint input
    std::string strJointPath1;
    std::string strJointPath2;
    std::string strJointPrefix1;
    std::string strJointPrefix2;
    std::string strJointPostfix1;
    std::string strJointPostfix2;
    int iJointDigits1;
    int iJointDigits2;

    //Output
    std::string  strOutputPath;         // output path for disparity maps
    std::string strOutputPrefix;        // prefix of output files e.g. disparity_
    int nOutDigits;                     // digits to be used for output index format e.g. 6 for 000010
    std::string strOutputPostfix;       // output postfix e.g. file extension .tif

    bool bWriteBackDisp;                // write backward disparity maps (pointing from right to left)
    std::string strBackOutputPath;      // output path for backward disparity maps
    std::string strBackOutputPrefix;    // prefix of output files e.g. disparity_back_
    int nBackOutDigits;                 // digits to be used for output index format e.g. 6 for 000010
    std::string strBackOutputPostfix;   // output postfix e.g. file extension .tif

    bool bWriteBackMatchDistance;                   // output path for backmatching distance maps
    std::string strBackMatchDistanceOutputPath;     // output path for backmatching distance maps
    std::string strBackMatchDistanceOutputPrefix;
    int nBackMatchDistanceOutDigits;
    std::string strBackMatchDistanceOutputPostfix;

    bool bWriteRevBackMatchDistance;                // write backmatching for backward disparity maps distance as a consistency measure for disparity maps
    std::string strRevBackMatchDistanceOutputPath;
    std::string strRevBackMatchDistanceOutputPrefix;
    int nRevBackMatchDistanceOutDigits;
    std::string strRevBackMatchDistanceOutputPostfix;

    //parameter
    int nNumberOfPyramids;                  // number of pyramid levels to use in matching
    float fTopPredictionShift;               // predicted constant left to right shift in highest pyramid level (attention: 3 pyramids -> realshift / 2 / 2 )
    int nJointMode;                         //0 = none, 1 = from file/cv::Mat, > 1 experimental....
};

struct SMatchingParametersInput
{
    bool bHasNodata;                    // is there a specified value for invalid data points
    float fNodataValue;                 // specified value for invalid data points e.g. -99999
    

    //Output
    bool bWriteBackDisp;                // write backward disparity maps (pointing from right to left)

    //parameter
    bool bWriteBackMatchDistance;
    bool bWriteRevBackMatchDistance;
    std::string strBackMatchDistanceOutputFile;
    std::string strRevBackMatchDistanceOutputFile;

    int nNumberOfPyramids;
    int nPredictor;                         // 0 = shift, 2=disparity, 3=top disparity, 4 = const disparity, 5 = const top disparity
    std::string strDispPredictionFile;      
    std::string strBackDispPredictionFile;  
    float fTopPredictionShift;
    int nJointMode;                         //0 = none, 1 = from file/cv::Mat, > 1 experimental....
    int nJointModeKernelSize;
    int nNumberOfMorphologicalIter;

    std::vector<int> oGPUs;
    std::vector<SMatchingParameters> oPyramidParams;
    SMatchingParameters oFinalSubPixelParameters;

    int nDebugLevel;
    std::string  strDebugOutput;
};
//##############################################################################################################################
/*  Rectifies images in a directory
@param[in]  strInputPathLeft path of left input images directory
@param[in]  strInputPathRight path of right input images directory
@param[in]  strCalibFile xml calibration file containing intrinsic and extrinsic camera parameters
@param[in]  strPrefix1 prefix of left images
@param[in]  strPrefix2 prefix of right images
@param[in]  strPostfix1 postfix of left images
@param[in]  strPostfix2 postfix of right images
@param[in]  iDigits1 digits expected from left  input image index format e.g. 6 for 000010
@param[in]  iDigits2 digits expected from right input image index format e.g. 6 for 000010
@param[in]  dAlpha: Free scaling parameter. If it is -1 or absent, the function performs the default scaling.
Otherwise, the parameter should be between 0 and 1. alpha=0 means that the rectified images are zoomed
and shifted so that only valid pixels are visible (no black areas after rectification).
alpha=1 means that the rectified image is decimated and shifted so that all the pixels from the
original images from the cameras are retained in the rectified images (no source image pixels are lost).
Obviously, any intermediate value yields an intermediate result between those two extreme cases.
@param[in]  dMeanObjectDistance: images are rectified in such a way that the remaining shift in x direction is 0
at a distance dMeanObjectDistance from the left camera center. Useful for flat object at known distance from camera.
Q matrix in output calibration file is adjusted accordingly. Turn off with value < 0.
@param[in]  strOutputPath output path of right images directory
@param[in]  strOutputCalib output path of left images directory
@param[in]  strOutputPattern1 pattern of left output files in the following format e.g. "Rectified_Left_%06d.tif" -> for 6 digit index
@param[in]  strOutputPattern2 pattern of right output files in the following format e.g. "Rectified_Right_%06d.tif"
@param[in]  iInterp specifies used interpolator: 0 = nearest neighbour, 1 = bilinear, 2 = bicubic, 3 = lanczos4
@param[in]  strUid1 uid of left camera in camera calibration xml (case sensitive)
@param[in]  strUid2 uid of right camera in camera calibration xml (case sensitive)
@param[in]  strInputParameterFile ini file with additional input parameters and logger definition
*/
int JR_PHOBOS_PHOBOSINTEGRATION_EXPORT RectifyImages(
    const std::string &strInputPathLeft,
    const std::string &strInputPathRight,
    const std::string &strCalibFile,
    const std::string & strPrefix1,
    const std::string & strPrefix2,
    const std::string & strPostfix1,
    const std::string & strPostfix2,
    const int iDigits1,
    const int iDigits2,
    const double dAlpha,
    const double dMeanObjectDistance,
    const std::string &strOutputPath,
    const std::string &strOutputCalib,
    const std::string& strOutputPattern1,
    const std::string& strOutputPattern2,
    const int iInterp,
    const std::string& strUid1,
    const std::string& strUid2,
    const std::string &strInputParameterFile

    );

//##############################################################################################################################
/*  Reads configuration from a JR ini file 
@param[out]  SParametersInput input parameters struct which is overwritten with values from ini
@param[in]  strInputParameterFile ini name
*/
void JR_PHOBOS_PHOBOSINTEGRATION_EXPORT ReadIniFile(
    SMatchingParametersInput& oParams,
    const std::string & strInputParameterFile
    );

//##############################################################################################################################
/*  Executes multi GPU SGM stereo matching for images in directory
@param[in]  SParametersInput input parameters struct
@param[in]  strInputParameterFile ini file with additional input parameters and logger definition
*/
int JR_PHOBOS_PHOBOSINTEGRATION_EXPORT StereoMatcher(
    SParametersInput& oParams,
    const std::string & strInputParameterFile
    );
//##############################################################################################################################
/*  Triangulates disparity maps in a directory
@param[in]  strInputDisparityDir path of directory containing disparity maps
@param[in]  strCalibFile xml calibration file containing intrinsic and extrinsic camera parameters
@param[in]  strPrefix prefix of disparity maps
@param[in]  strPostfix postfix of disparity maps
@param[in]  iDigits digits expected from disparity maps input index format e.g. 6 for 000010
@param[in]  iMode:  0 = 3D Point Cloud (.vtk) -> attention this also needs .vtk in output pattern,
1 = depth map (floating point)  - distance values to left camera center,
2 = depth map (2 byte unsigned) - distance values to left camera center,
3 = Z map (floating point)      - Z component of coordinate system,
4 = Z map (2 byte unsigned)     - Z component of coordinate system,
5 = XYZ map (floating point)    - XYZ components of coordinate system,
6 = XYZ map (2 by unsigned)     - XYZ components of coordinate system
@param[in]  strOutDir output directory
@param[in]  strOutPattern pattern of output files in the following format e.g. "depth_%06d.tif" -> for 6 digit index
@param[in]  strInputParameterFile ini file with additional input parameters and logger definition
*/
int JR_PHOBOS_PHOBOSINTEGRATION_EXPORT TriangulateImages(
    const std::string & strInputDisparityDir,
    const std::string & strCalibFile,
    const std::string & strRectId,
    const std::string & strPrefix,
    const std::string & strPostfix,
    const int iDigits,
    const int iMode,
    const std::string & strOutDir,
    const std::string & strOutPattern,
    const std::string strInputParameterFile
    );
//##############################################################################################################################
//Pairwise call, for performance reasons i separated the alloc part from computing (we do not need to realloc GPU memory
//on each MatchStereo call if frames always have same size and same parameter set is used)
//
// on frame series with same parameters make:
// TSTEREOHANDLE pH;
// pH = CreateMatchStereoHandle(roParamsIn);
// for(int nFrame = 0; nFrame < XXX; ++nFrame)
// {
//      MatchStereo( pH, ...)
// }
// DestroyMatchStereoHandle(pH);

typedef void* TSTEREOHANDLE;

TSTEREOHANDLE JR_PHOBOS_PHOBOSINTEGRATION_EXPORT CreateMatchStereoHandle( const SMatchingParametersInput& roParamsIn );

void JR_PHOBOS_PHOBOSINTEGRATION_EXPORT MatchStereo(
    TSTEREOHANDLE pHandle,
    const cv::Mat& oLeftFrameOpenCV,
    const cv::Mat& oRightFrameOpenCV,
    const cv::Mat& oLeftJointOpenCV,//should be empty if not used!! Only used if  nJointMode is set valid!
    const cv::Mat& oRightJointOpenCV,//should be empty if not used!! Only used if  nJointMode is set valid!
    cv::Mat& oDisparityOpenCV, //32 bit floating point image, cv::Mat is converted if not correct
    std::string sLogFile,
    EVerbosityLevel eLogLevelIn
    );

void JR_PHOBOS_PHOBOSINTEGRATION_EXPORT DestroyMatchStereoHandle( TSTEREOHANDLE pHandle );

}

}