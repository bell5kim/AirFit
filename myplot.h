#ifndef _MYPLOT_H_
#define _MYPLOT_H_

#include <qwt_plot.h>

#define MAXDATA 5000

class QwtPlotCurve;
class QwtPlotMarker;

class MyPlot: public QwtPlot
{
    Q_OBJECT

public:
    explicit MyPlot( QWidget *parent = 0);


    char fname[256];
    void usrDataFile(char *fname);

    QString     LegendSwitch;
    QString     DDSwitch;

    void showLegend();
    void showDD();


    void err_Exit(char *location, char *reason, int exit_code);


    double x[MAXDATA];     // x position
    double y[MAXDATA];     // y position
    double z[MAXDATA];     // z position
    double dc[MAXDATA];    // calculated dair
    double dm[MAXDATA];    // measured dair
    double dd[MAXDATA];    // (dc-dm)/dm*100
    int    nData;          // number of data points

    double  xPos;          // x Position of Profile
    double  yPos;          // y Position of Profile
    double  zPos;          // z Position of Profile

    int     nCurves;          // Number of Curves

//signals:
   // void setDamp( double );

// public Q_SLOTS:
public slots:
   // void setDamp( double damping );
//    void damp(double);

private:

    QString ddFile;

    QwtPlotCurve *(crvc[10]);
    QwtPlotCurve *(crvm[10]);
    QwtPlotCurve *(crvd[10]);

    QwtPlotMarker *mrk1;
    QwtPlotMarker *mrk2;

    QwtPlotMarker *textMarker1;
    QwtPlotMarker *textMarker2;

};

#endif
