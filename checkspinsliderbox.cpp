#include "checkspinsliderbox.h"
#include "ui_checkspinsliderbox.h"

#include <QTextStream>

CheckSpinSliderBox::CheckSpinSliderBox(QWidget *parent) :
    QWidget(parent),
    uix(new Ui::CheckSpinSliderBox)
{
    uix->setupUi(this);
}

CheckSpinSliderBox::~CheckSpinSliderBox()
{
    delete uix;
}

void CheckSpinSliderBox::on_dSpinBox_valueChanged(double arg1)
{
    float minSpinBox = uix->dSpinBox->minimum();
    float maxSpinBox = uix->dSpinBox->maximum();
    float minSlider  = uix->hSlider->minimum();
    float maxSlider  = uix->hSlider->maximum();
    int value = (int)(minSlider +
                      (arg1 - minSpinBox)/(maxSpinBox - minSpinBox)
                      *(maxSlider - minSlider));
    uix->hSlider->setValue(value);
    // QTextStream(stdout) << "SpinBox = " << minSpinBox << " " << maxSpinBox << " " << value << endl;
}

void CheckSpinSliderBox::on_hSlider_valueChanged(int value)
{
    float minSpinBox = uix->dSpinBox->minimum();
    float maxSpinBox = uix->dSpinBox->maximum();
    float minSlider  = uix->hSlider->minimum();
    float maxSlider  = uix->hSlider->maximum();
    double arg1 = (double)(minSpinBox +
                           (value - minSlider)/(maxSlider - minSlider)
                           *(maxSpinBox - minSpinBox));
    uix->dSpinBox->setValue(arg1);
    // QTextStream(stdout) << "Slider = " << minSlider << " " << maxSlider << " " << arg1 << endl;
}

void CheckSpinSliderBox::on_checkBox_clicked()
{
    if(uix->dSpinBox->isEnabled()) {
        uix->dSpinBox->setDisabled(true);
        uix->hSlider->setDisabled(true);
    }
    else {
        uix->dSpinBox->setEnabled(true);
        uix->hSlider->setEnabled(true);
    }
}

void CheckSpinSliderBox::setCheckBoxText(QString text)
{
    uix->checkBox->setText(text);
}


void CheckSpinSliderBox::setMinimum(int value)
{
    //uix->hSlider->setMinimum(value);
    uix->dSpinBox->setMinimum(value);
}

void CheckSpinSliderBox::setMaximum(int value)
{
    //uix->hSlider->setMaximum(value);
    uix->dSpinBox->setMaximum(value);
}

void CheckSpinSliderBox::setValue(double value)
{
    uix->dSpinBox->setValue(value);
}

void CheckSpinSliderBox::setSliderStep(int value)
{
    uix->hSlider->setSingleStep(value);
    uix->hSlider->setTickInterval(value);
}

double CheckSpinSliderBox::value(void)
{
    return(uix->dSpinBox->value());
}

void CheckSpinSliderBox::setSpinStep(double value)
{
    uix->dSpinBox->setSingleStep(value);
}

void CheckSpinSliderBox::setSliderRange(int minValue, int maxValue)
{
    uix->hSlider->setRange(minValue, maxValue);
}

void CheckSpinSliderBox::setSpinBoxRange(double minValue, double maxValue)
{
    uix->dSpinBox->setRange(minValue, maxValue);
}

void CheckSpinSliderBox::validate(int value)
{
    uix->dSpinBox->setDecimals(value);
}

void CheckSpinSliderBox::setChecked(bool value)
{
     uix->checkBox->setChecked(value);
}

void CheckSpinSliderBox::toggle()
{
     uix->checkBox->toggle();
}

bool CheckSpinSliderBox::isChecked()
{
     return(uix->checkBox->isChecked());
}

