/*
* Copyright I3D Robotics Ltd, 2017
* Author: Josh Veitch-Michaelis
*/

#ifndef MATCHERWIDGETI3DRSGM_H
#define MATCHERWIDGETI3DRSGM_H

#include <matcherwidget.h>
#include <matcheri3drsgm.h>

//!  OpenCV's Block Matcher QT Widget
/*!
  QT widget for OpenCV's Block Matcher controls
*/

namespace Ui {
class MatcherWidgetI3DRSGM;
}

class MatcherWidgetI3DRSGM : public MatcherWidget {
  Q_OBJECT

 signals:
   void minDisparity(int);
   void disparityRange(int);
   void blockSize(int);
   void prefilterCap(int);
   void prefilterSize(int);
   void prefilterType(int);
   void textureThreshold(int);
   void speckleWindow(int);
   void speckleRange(int);
   void consistency(int);
   void uniquenessRatio(int);
   void saveClicked();


 public:
   explicit MatcherWidgetI3DRSGM(QWidget *parent = 0, cv::Size image_size =cv::Size(0,0));
  ~MatcherWidgetI3DRSGM();
   void setImageWidth(int width);

public slots:
   void updateDisparityRange(int);
   void updateBlockSize(int);
   void updateMinDisparity(int);

   void updatePrefilterCap(int cap);
   void updatePrefilterSize(int size);
   void updatePrefilterType(int index);
   void updateTextureThreshold(int threshold);
   void updateConsistency(int consistency);
   void updateUniquenessRatio(int ratio);
   void updateSpeckleRange(int range);
   void updateSpeckleWindow(int window);
   void enableSpeckleFilter(bool enable);
   void enableConsistency(bool enable);

   AbstractStereoMatcher* getMatcher(void);

   void onSaveClicked();

 private:
  Ui::MatcherWidgetI3DRSGM *ui;
  MatcherI3DRSGM* matcher;
  int block_size;
  int min_disparity;
  int disparity_range;
  int image_width = 640;

};

#endif  // MATCHERWIDGETI3DRSGM_H
