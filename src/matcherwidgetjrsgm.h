#ifndef MATCHERWIDGETJRSGM_H
#define MATCHERWIDGETJRSGM_H

#include <matcherjrsgm.h>
#include <matcherwidget.h>

//!  JRSGM Matcher QT Widget
/*!
  QT widget for JR's SGM controls
*/

namespace Ui {
class MatcherWidgetJRSGM;
}

class MatcherWidgetJRSGM : public MatcherWidget
{
    Q_OBJECT

public:
    explicit MatcherWidgetJRSGM(QWidget *parent = 0, cv::Size image_size =cv::Size(0,0));
     ~MatcherWidgetJRSGM();

public slots:
    void onCostUpdated(void);
    void updateDisparityRange(int range);
    void updateCensus(int width);
    void initParameters();
    void reloadParameters();
    AbstractStereoMatcher* getMatcher(void);


private:
    Ui::MatcherWidgetJRSGM *ui;
    MatcherJRSGM* matcher;
    void init(void);

signals:
    void setMatchCosts(double p1, double p2);
    void setOcclusion(bool enabled);
    void setInterpolation(bool enabled);
    void setSubpixel(bool enabled);
    void setDisparityRange(int range);
    void setCensusWindow(int width);

};

#endif // MATCHERWIDGETJRSGM_H
