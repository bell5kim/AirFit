#ifndef CHECKSPINSLIDERBOX_H
#define CHECKSPINSLIDERBOX_H

#include <QWidget>

namespace Ui {
class CheckSpinSliderBox;
}

class CheckSpinSliderBox : public QWidget
{
    Q_OBJECT

public:
    explicit CheckSpinSliderBox(QWidget *parent = 0);
    ~CheckSpinSliderBox();

    void setCheckBoxText(QString);
    void setMinimum(int);
    void setMaximum(int);
    void setValue(double);
    void setSliderStep(int);
    double value(void);
    void setSpinStep(double);
    void setSliderRange(int, int);
    void setSpinBoxRange(double, double);
    void validate(int);
    void setChecked(bool);
    void toggle();
    bool isChecked();

private slots:
    void on_dSpinBox_valueChanged(double arg1);

    void on_hSlider_valueChanged(int value);

    void on_checkBox_clicked();

private:
    Ui::CheckSpinSliderBox *uix;
};

#endif // CHECKSPINSLIDERBOX_H
