#include "matcherwidgeti3drsgm.h"
#include "ui_matcherwidgeti3drsgm.h"

MatcherWidgetI3DRSGM::MatcherWidgetI3DRSGM(QWidget* parent,
                                             cv::Size image_size)
    : MatcherWidget(parent), ui(new Ui::MatcherWidgetI3DRSGM) {
  ui->setupUi(this);
  matcher = new MatcherI3DRSGM(this, image_size);
  reloadParameters();
  connect(ui->costP1Slider, SIGNAL(valueChanged(int)), this,
          SLOT(onCostUpdated()));
  connect(ui->costP2Slider, SIGNAL(valueChanged(int)), this,
          SLOT(onCostUpdated()));
  connect(ui->disparityShiftSlider, SIGNAL(valueChanged(int)), matcher,
          SLOT(setDisparityShift(int)));
  connect(ui->disparityShiftSlider, SIGNAL(valueChanged(int)),
          ui->disparityShiftLabel, SLOT(setNum(int)));

  connect(ui->occlusionCheckbox, SIGNAL(toggled(bool)), matcher,
          SLOT(enableOcclusionDetection(bool)));
  // connect(ui->subpixelCheckbox, SIGNAL(toggled(bool)), matcher,
  // SLOT(enableSubpixel(bool)));
  connect(ui->interpolationCheckbox, SIGNAL(toggled(bool)), matcher,
          SLOT(enableInterpolation(bool)));

  connect(ui->disparityRangeSlider, SIGNAL(valueChanged(int)), this,
          SLOT(updateDisparityRange(int)));
  connect(ui->censusSizeSlider, SIGNAL(valueChanged(int)), this,
          SLOT(updateCensus(int)));
  connect(ui->savePushButton, SIGNAL(clicked(bool)), matcher,
          SLOT(saveParameters()));
  connect(ui->reloadPushButton, SIGNAL(clicked(bool)), this,
          SLOT(reloadParameters()));
}

void MatcherWidgetI3DRSGM::reloadParameters() {
  matcher->loadParameters();
  initParameters();
}

void MatcherWidgetI3DRSGM::initParameters() {
  ui->costP1Slider->setValue(matcher->getP1() * 100);
  ui->costP1Label->setText(QString::number(matcher->getP1()));

  ui->costP2Slider->setValue(matcher->getP2() * 100);
  ui->costP2Label->setText(QString::number(matcher->getP2()));

  ui->censusSizeSlider->setValue((matcher->getCensusSize() - 1) / 2);
  ui->censusSizeLabel->setText(QString::number(matcher->getCensusSize()));

  ui->disparityRangeLabel->setText(
      QString::number(matcher->getDisparityRange()));
  ui->disparityRangeSlider->setValue(matcher->getDisparityRange());

  ui->disparityShiftSlider->setValue(matcher->getDisparityShift());

  ui->interpolationCheckbox->setChecked(matcher->getInterpolate());
  ui->occlusionCheckbox->setChecked(matcher->getOcclusionDetect());
  // ui->subpixelCheckbox->setChecked(matcher->getSubpixel());
}

AbstractStereoMatcher* MatcherWidgetI3DRSGM::getMatcher(void) {
  return matcher;
}

void MatcherWidgetI3DRSGM::onCostUpdated(void) {
  double p1 = ui->costP1Slider->value() / 100.0;
  double p2 = ui->costP2Slider->value() / 100.0;

  ui->costP1Label->setText(QString::number(p1, 'f', 2));
  ui->costP2Label->setText(QString::number(p2, 'f', 2));

  matcher->setMatchCosts(p1, p2);
}

void MatcherWidgetI3DRSGM::updateDisparityRange(int range) {
  range = range * 2 + 1;
  ui->disparityRangeLabel->setText(QString::number(range));
  matcher->setDisparityRange(range);
}

void MatcherWidgetI3DRSGM::updateCensus(int width) {
  int window_size = width * 2 + 1;
  ui->censusSizeLabel->setText(QString::number(window_size));
  matcher->setWindowSize(window_size);
}

MatcherWidgetI3DRSGM::~MatcherWidgetI3DRSGM() { delete ui; }