#ifndef AIRFIT_H
#define AIRFIT_H

#include <QMainWindow>

namespace Ui {
class AirFit;
}

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class Plot;
class QPolygon;

class AirFit : public QMainWindow
{
    Q_OBJECT

public:
    explicit AirFit(QWidget *parent = 0);
    ~AirFit();

    void init();

    int iListBox;   // current Listbox xListBox=1, yListBox=2, zListBox=3
    int iListItem;  // current ListItem

    void resetZoomer();


public slots:

    virtual void initAirFit();

    virtual void setLineEditE();
    virtual void setLineEditZ0();
    virtual void setLineEditZS();
    virtual void setLineEditZM();
    virtual void setLineEditZX();
    virtual void setLineEditZY();
    virtual void setLineEditZI();
    virtual void setLineEditXN();
    virtual void setLineEditYN();
    virtual void setLineEditZN();
    virtual void setLineEditWXN();
    virtual void setLineEditWYN();
    virtual void setLineEditDev();
    virtual void setLineEditChi();

    virtual void setFloatSpinBoxP0();
    virtual void setFloatSpinBoxS0();
    virtual void setFloatSpinBoxSS();
    virtual void setFloatSpinBoxH0();
    virtual void setFloatSpinBoxH1();
    virtual void setFloatSpinBoxH2();
    virtual void setFloatSpinBoxH3();
    virtual void setFloatSpinBoxH4();

    virtual void setAllHZero();
    virtual void setCheckBoxP0();
    virtual void setCheckBoxS0();
    virtual void setCheckBoxSS();
    virtual void setCheckBoxH0();
    virtual void setCheckBoxH1();
    virtual void setCheckBoxH2();
    virtual void setCheckBoxH3();
    virtual void setCheckBoxH4();
    virtual void setCheckBoxMX();
    virtual void setCheckBoxMY();

    virtual void toggleCheckBoxLeft();
    virtual void toggleCheckBoxRight();

    virtual void getFloatSpinBoxP0();
    virtual void getFloatSpinBoxS0();
    virtual void getFloatSpinBoxSS();
    virtual void getFloatSpinBoxH0();
    virtual void getFloatSpinBoxH1();
    virtual void getFloatSpinBoxH2();
    virtual void getFloatSpinBoxH3();
    virtual void getFloatSpinBoxH4();

    virtual void getCheckBoxMX();
    virtual void getCheckBoxMY();
    virtual void getLineEditZS();
    virtual void getLineEditZM();
    virtual void getLineEditZX();
    virtual void getLineEditZY();
    virtual void getLineEditZN();

    virtual void setAirFitValues( char * fname );
    virtual void newAirFitValues();
    virtual void updateAirFitValues();
    virtual void resetAirFitValues();
    virtual void readAirFit( char * fname );
    virtual void writeAirFit();
    // virtual void runAirFit();
    // virtual void Interrupt();
    // virtual void Done();

    virtual void plotAirFit();
    virtual void plotAirFitOld();
    virtual void writeLocalSetting( QString keyWord, QString keyValue );
    // virtual void editAirFitInput();
    virtual void runAirFitNew();
    virtual void writeBDT( QString bdtFile, QString pType, float pCon );
    virtual void loadData();
    //virtual void getXPlotData();
    //virtual void getYPlotData();
    //virtual void getZPlotData();
    //virtual void showLegend();
    // virtual void showDD();

    virtual bool makeDir( QString dirStr );
    virtual QString readLocalSetting( QString keyWord );


// private Q_SLOTS:
private slots:
    void moved( const QPoint & );
    void selected( const QPolygon & );
    void runGammaTable();

#ifndef QT_NO_PRINTER
    void print();
#endif

    void exportDocument();
    void enableZoomMode( bool );


    void on_resetButton_clicked();

    void on_doneButton_clicked();

    void on_interruptButton_clicked();

    void on_editButton_clicked();

    void on_xListBox_clicked(const QModelIndex &index);

    void on_plotButton_clicked();

    void on_pushButtonZ0_clicked();


    void on_pushButtonZS_clicked();

    void on_pushButtonZM_clicked();

    void on_pushButtonZX_clicked();

    void on_pushButtonZY_clicked();

    void on_pushButtonZN_clicked();

    void on_yListBox_clicked(const QModelIndex &index);

    void on_zListBox_clicked(const QModelIndex &index);

    void on_radioButtonLegend_toggled(bool checked);

    void on_radioButtonDD_toggled(bool checked);

    void on_pushButtonCheckPS_clicked();

    void on_fitButton_clicked();


    void on_pushButtonCheckH_clicked();

    void on_pushButtonCheckZero_clicked();

private:
    void showInfo( QString text = QString::null );

    Ui::AirFit *ui;

    QwtPlotZoomer *d_zoomer[2];
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
};


#endif // AIRFIT_H
