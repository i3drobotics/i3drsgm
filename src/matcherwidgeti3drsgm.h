#ifndef MATCHERWIDGETI3DRSGM_H
#define MATCHERWIDGETI3DRSGM_H

#include <matcheri3drsgm.h>
#include <matcherwidget.h>

//!  I3DRSGM Matcher QT Widget
/*!
  QT widget for I3DR's SGM controls
*/

namespace Ui {
class MatcherWidgetI3DRSGM;
}

class MatcherWidgetI3DRSGM : public MatcherWidget
{
    Q_OBJECT

public:
    explicit MatcherWidgetI3DRSGM(QWidget *parent = 0, cv::Size image_size =cv::Size(0,0));
     ~MatcherWidgetI3DRSGM();

public slots:
    void onCostUpdated(void);
    void updateDisparityRange(int range);
    void updateCensus(int width);
    void initParameters();
    void reloadParameters();
    AbstractStereoMatcher* getMatcher(void);


private:
    Ui::MatcherWidgetI3DRSGM *ui;
    MatcherI3DRSGM* matcher;
    void init(void);

signals:
    void setMatchCosts(double p1, double p2);
    void setOcclusion(bool enabled);
    void setInterpolation(bool enabled);
    void setSubpixel(bool enabled);
    void setDisparityRange(int range);
    void setCensusWindow(int width);

};

#endif // MATCHERWIDGETI3DRSGM_H
