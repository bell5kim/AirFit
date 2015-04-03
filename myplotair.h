#include <qwt_plot.h>

#define MAXDATA 5000

// class QPointArray;

class QwtPlotCurve;
class QwtPlotMarker;
// class QwtPlotPanner;


class MyPlotAir: public QwtPlot
{
     Q_OBJECT
	 
public:
     explicit MyPlotAir( QWidget *parent = 0 );
    
     void showToolBar(QMainWindow *mWin);
     void err_Exit(char *location, char *reason, int exit_code);
     void loadData(char *dName);
     void usrDataFile(char *fname);
	 void showLegend();
	 void showDD();

     char fname[256];
	 
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
	 
	 QString     LegendSwitch;
	 QString     DDSwitch;
	 


	 QString Info;
	 int iListBox;   // current Listbox xListBox=1, yListBox=2, zListBox=3
	 int iListItem;  // current ListItem
 	 
private slots:

     void toggleCurve(long);

	 	 
private:
     // QStatusBar *statusBar;
	 
     QString ddFile;

     QwtPlotCurve *(crvc[10]);
     QwtPlotCurve *(crvm[10]);
     QwtPlotCurve *(crvd[10]);

     QwtPlotMarker *mrk1;
     QwtPlotMarker *mrk2;

     QwtPlotMarker *textMarker1;
     QwtPlotMarker *textMarker2;
  
};
