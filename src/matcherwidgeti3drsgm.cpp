/*
* Copyright I3D Robotics Ltd, 2017
* Author: Josh Veitch-Michaelis
*/

#include "matcherwidgeti3drsgm.h"
#include "ui_matcherwidgeti3drsgm.h"

MatcherWidgetI3DRSGM::MatcherWidgetI3DRSGM(QWidget* parent,
                                           cv::Size image_size)
    : MatcherWidget(parent), ui(new Ui::MatcherWidgetI3DRSGM) {
    ui->setupUi(this);
    matcher = new MatcherI3DRSGM(this, image_size);

    /*

    ui->blockSizeSlider->setValue((matcher->getBlockSize() - 1) / 2.0);
    ui->blockSizeLabel->setText(QString::number(matcher->getBlockSize()));

    ui->disparityRangeSlider->setValue(matcher->getDisparityRange() / 16.0);
    ui->disparityRangeLabel->setText(
                QString::number(matcher->getDisparityRange()));

    ui->minDisparitySlider->setValue(matcher->getMinDisparity());
    ui->minDisparityLabel->setText(QString::number(matcher->getMinDisparity()));

    ui->uniquenessRatioSlider->setValue(matcher->getUniquenessRatio());
    ui->uniquenessRatioLabel->setText(
                QString::number(matcher->getUniquenessRatio()));

    ui->prefilterCombo->setCurrentIndex(matcher->getPrefilterType());

    ui->prefilterCapSlider->setValue(matcher->getPrefilterCap());
    ui->prefilterCapLabel->setText(QString::number(matcher->getPrefilterCap()));

    ui->prefilterSizeSlider->setValue((matcher->getPrefilterSize() - 1) / 2.0);
    ui->prefilterSizeLabel->setText(QString::number(matcher->getPrefilterSize()));

    ui->textureThresholdSlider->setValue(matcher->getTextureThreshold());
    ui->textureThresholdLabel->setText(
                QString::number(matcher->getTextureThreshold()));

    if (matcher->getSpeckleFilterWindow() <= 0 ||
            matcher->getSpeckleFilterRange() <= 0) {
        ui->speckleFilterCheck->setChecked(false);
    } else {
        ui->speckleFilterCheck->setChecked(true);
        ui->speckleRangeSlider->setEnabled(true);
        ui->speckleWindowSlider->setEnabled(true);
        ui->speckleRangeLabel->setEnabled(true);
        ui->speckleWindowLabel->setEnabled(true);
    }

    if (matcher->getDisp12MaxDiff() <= 0 || matcher->getDisp12MaxDiff() <= 0) {
        ui->consistencyCheck->setChecked(false);
    } else {
        ui->consistencyCheck->setChecked(true);
        ui->consistencyLabel->setEnabled(true);
        ui->consistencySlider->setEnabled(true);
    }

    ui->consistencySlider->setValue(matcher->getDisp12MaxDiff());
    ui->consistencyLabel->setText(QString::number(matcher->getDisp12MaxDiff()));

    ui->speckleRangeSlider->setValue(matcher->getSpeckleFilterRange());
    ui->speckleRangeLabel->setText(
                QString::number(matcher->getSpeckleFilterRange()));

    ui->speckleWindowSlider->setValue(matcher->getSpeckleFilterWindow());
    ui->speckleWindowLabel->setText(
                QString::number(matcher->getSpeckleFilterWindow()));

    */

    connect(ui->blockSizeSlider, SIGNAL(valueChanged(int)), this,
            SLOT(updateBlockSize(int)));
    connect(ui->minDisparitySlider, SIGNAL(valueChanged(int)), this,
            SLOT(updateMinDisparity(int)));
    connect(ui->disparityRangeSlider, SIGNAL(valueChanged(int)), this,
            SLOT(updateDisparityRange(int)));

    connect(ui->prefilterCapSlider, SIGNAL(valueChanged(int)), this,
            SLOT(updatePrefilterCap(int)));
    connect(ui->prefilterCapSlider, SIGNAL(valueChanged(int)),
            ui->prefilterCapLabel, SLOT(setNum(int)));
    connect(ui->prefilterSizeSlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updatePrefilterSize(int)));
    connect(ui->prefilterCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updatePrefilterType(int)));

    connect(ui->textureThresholdSlider, SIGNAL(sliderMoved(int)),
            ui->textureThresholdLabel, SLOT(setNum(int)));
    connect(ui->textureThresholdSlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updateTextureThreshold(int)));

    connect(ui->consistencySlider, SIGNAL(sliderMoved(int)), ui->consistencyLabel,
            SLOT(setNum(int)));
    connect(ui->consistencySlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updateConsistency(int)));

    connect(ui->uniquenessRatioSlider, SIGNAL(sliderMoved(int)),
            ui->uniquenessRatioLabel, SLOT(setNum(int)));
    connect(ui->uniquenessRatioSlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updateUniquenessRatio(int)));

    connect(ui->speckleRangeSlider, SIGNAL(sliderMoved(int)),
            ui->speckleRangeLabel, SLOT(setNum(int)));
    connect(ui->speckleWindowSlider, SIGNAL(sliderMoved(int)),
            ui->speckleWindowLabel, SLOT(setNum(int)));

    connect(ui->speckleRangeSlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updateSpeckleRange(int)));
    connect(ui->speckleWindowSlider, SIGNAL(sliderMoved(int)), this,
            SLOT(updateSpeckleWindow(int)));

    connect(ui->speckleFilterCheck, SIGNAL(toggled(bool)), this,
            SLOT(enableSpeckleFilter(bool)));
    connect(ui->consistencyCheck, SIGNAL(toggled(bool)), this,
            SLOT(enableConsistency(bool)));

    connect(ui->saveParametersButton, SIGNAL(clicked(bool)), this,
            SLOT(onSaveClicked()));
}

AbstractStereoMatcher* MatcherWidgetI3DRSGM::getMatcher() {
    return matcher;
}

void MatcherWidgetI3DRSGM::onSaveClicked() {
    //matcher->saveParams();
}

void MatcherWidgetI3DRSGM::updateSpeckleRange(int range) {
    //matcher->setSpeckleFilterRange(range);
}

void MatcherWidgetI3DRSGM::updateSpeckleWindow(int window) {
    //matcher->setSpeckleFilterWindow(window);
}

void MatcherWidgetI3DRSGM::enableSpeckleFilter(bool enable) {
    if (enable) {
        //matcher->setSpeckleFilterRange(ui->speckleRangeSlider->value());
        //matcher->setSpeckleFilterWindow(ui->speckleWindowSlider->value());

        ui->speckleRangeSlider->setEnabled(true);
        ui->speckleWindowSlider->setEnabled(true);
        ui->speckleRangeLabel->setEnabled(true);
        ui->speckleWindowLabel->setEnabled(true);
    } else {
        //matcher->setSpeckleFilterRange(0);
        //matcher->setSpeckleFilterWindow(0);

        ui->speckleRangeSlider->setEnabled(false);
        ui->speckleWindowSlider->setEnabled(false);
        ui->speckleRangeLabel->setEnabled(false);
        ui->speckleWindowLabel->setEnabled(false);
    }
}

void MatcherWidgetI3DRSGM::enableConsistency(bool enable) {
    if (enable) {
        //matcher->setDisp12MaxDiff(ui->consistencySlider->value());

        ui->consistencyLabel->setEnabled(true);
        ui->consistencySlider->setEnabled(true);
    } else {
        //matcher->setDisp12MaxDiff(-1);

        ui->consistencyLabel->setEnabled(false);
        ui->consistencySlider->setEnabled(false);
    }
}

void MatcherWidgetI3DRSGM::updatePrefilterCap(int cap) {
    //matcher->maxPyramid(cap);
}

void MatcherWidgetI3DRSGM::updateTextureThreshold(int threshold) {
    //matcher->setTextureThreshold(threshold);
}

void MatcherWidgetI3DRSGM::updateConsistency(int con) {
    ui->consistencyLabel->setNum(con);
    //matcher->setDisp12MaxDiff(con);
}

void MatcherWidgetI3DRSGM::updatePrefilterSize(int size) {
    ui->prefilterSizeLabel->setNum(size);
    matcher->maxPyramid(size);
}

void MatcherWidgetI3DRSGM::updateUniquenessRatio(int ratio) {
    //matcher->setUniquenessRatio(ratio);
}

void MatcherWidgetI3DRSGM::updatePrefilterType(int index) {
    if (index == 0) {
        //matcher->setPrefilterType(-1);
    } else {
        //matcher->setPrefilterType(index - 1);
    }
}

void MatcherWidgetI3DRSGM::setImageWidth(int width) { image_width = width; }

void MatcherWidgetI3DRSGM::updateMinDisparity(int min_disparity) {
    ui->minDisparityLabel->setNum(min_disparity);
    this->min_disparity = min_disparity;

    double shift_p = (double)min_disparity / 20;
    matcher->setDisparityShift(shift_p);
}

void MatcherWidgetI3DRSGM::updateDisparityRange(int range) {
    disparity_range = range;
    ui->disparityRangeLabel->setNum(16 * range);

    int m_range = (16 * range) / 10;
    //force odd number
    if (m_range % 2 == 0)
    {
        m_range++;
    }
    matcher->setDisparityRange(m_range);
}

void MatcherWidgetI3DRSGM::updateBlockSize(int size) {
    block_size = size;
    ui->blockSizeLabel->setNum(2 * size + 1);

    int census_size = size;
    if (census_size % 2 == 0)
    {
        census_size++;
    }

    matcher->setWindowSize(census_size);
}

MatcherWidgetI3DRSGM::~MatcherWidgetI3DRSGM() { delete ui; }
