#include "matcheri3drsgm.h"

void MatcherI3DRSGM::init(void)
{
    initSGMParameters();
    /*
    enableInterpolation(false);
    enableOcclusionDetection(false);
    setWindowSize(9);

    min_disparity = 0;
    disparity_range = 65;

    setDisparityRange(21);
    setMatchCosts(0.5, 1.5);
    */
}

int MatcherI3DRSGM::getErrorDisparity(void)
{
    return -10000;
}

void MatcherI3DRSGM::saveParameters(void)
{
    writeConfig("./params/i3dr_matcher_settings.param", params);
}

void MatcherI3DRSGM::loadParameters(void)
{
    if (QFile("./params/i3dr_matcher_settings.param").exists())
        readConfig("./params/i3dr_matcher_settings.param", params);
    else
        qDebug() << "Failed to open matcher settings file";
}

void MatcherI3DRSGM::initSGMParameters()
{
    //setOcclusionDetection(false);
    //enableOccInterpol(false);
    //enableTextureDSI(false);
    //enableSubpixel(true);
    //maxPyramid(4);
    //setNoDataValue(-10000);
}

void MatcherI3DRSGM::setMatchCosts(double P1, double P2)
{
    //TODO
}

void MatcherI3DRSGM::setWindowSize(int census_size)
{
    //TODO
}

void MatcherI3DRSGM::setDisparityShift(int shift)
{
    //TODO
}

void MatcherI3DRSGM::enableSubpixel(bool enable)
{
    //TODO
}

void MatcherI3DRSGM::setDisparityRange(int n)
{
    //TODO
}

void MatcherI3DRSGM::enableInterpolation(bool enable)
{
    //TODO
}

void MatcherI3DRSGM::enableOcclusionDetection(bool enable)
{
    //TODO
}

void MatcherI3DRSGM::forwardMatch()
{
    if (matcher_handle != nullptr)
    {
        std::string sgm_log = "./sgm_log.txt";
        try
        {
            cv::Mat oDisparity;
            JR::Phobos::MatchStereo(matcher_handle,
                                    *left,
                                    *right,
                                    cv::Mat(),
                                    cv::Mat(),
                                    oDisparity,
                                    sgm_log,
                                    JR::Phobos::e_logError);

            oDisparity.convertTo(disparity_lr, CV_32F, -16);
        }
        catch (...)
        {
            std::cerr << "FAILED TO COMPUTE" << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "Matcher handle not found" << std::endl;
        createMatcher();
        return;
    }
}

void MatcherI3DRSGM::backwardMatch()
{
    //TODO
}

void MatcherI3DRSGM::writeConfig(const std::string &sConfigFile,
                                 const JR::Phobos::SMatchingParametersInput &oParams)
{
    //TODO
}

void MatcherI3DRSGM::readConfig(const std::string &sConfigFile,
                                JR::Phobos::SMatchingParametersInput &oParams)
{
    //TODO
}

void MatcherI3DRSGM::createMatcher()
{
    /*
  if (matcher_handle != nullptr)
  {
    JR::Phobos::DestroyMatchStereoHandle(matcher_handle);
  }
  try
  {
    //checkMemoryValid(image_size.width, image_size.height);
    isMemoryValid = true;
    if (isMemoryValid)
    {
      std::cout << "Re-creating matcher with new paramters..." << std::endl;
      WriteIniFileRaw(tmp_param_file, params_raw);
      params = JR::Phobos::SMatchingParametersInput();
      JR::Phobos::ReadIniFile(params, tmp_param_file);
      matcher_handle = JR::Phobos::CreateMatchStereoHandle(params);
      std::cout << "Re-created matcher with new paramters." << std::endl;
    }
    else
    {
      std::cerr << "NOT ENOUGH GPU MEMORY AVAILABLE" << std::endl;
    }
  }
  catch (...)
  {
    std::cerr << "Failed to create I3DR matcher with chosen parameters" << std::endl;
    //std::cerr << ex.what() << std::endl;
  }
  */
}