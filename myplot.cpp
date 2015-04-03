#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_canvas.h>
#include <qmath.h>
#include "myplot.h"

#include <QDir>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

#if QT_VERSION < 0x040601
#define qExp(x) ::exp(x)
#define qAtan2(y, x) ::atan2(y, x)
#endif


MyPlot::MyPlot( QWidget *parent ):
    QwtPlot( parent )
{
    setAutoReplot( false );

    setTitle( "Comparison of AFIT against Measurements" );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    setCanvas( canvas );
    setCanvasBackground( QColor( "LightGray" ) );

    // legend
    // QwtLegend *legend = new QwtLegend;
    // insertLegend( legend, QwtPlot::BottomLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->attach( this );

    // axes
    enableAxis( QwtPlot::yRight );
    setAxisTitle( QwtPlot::xBottom, "Distance from CAX (cm)" );
    setAxisTitle( QwtPlot::yLeft, "Relative Output Factor" );
    // setAxisTitle( QwtPlot::yRight, "Phase [deg]" );

    // setAxisMaxMajor( QwtPlot::xBottom, 6 );
    // setAxisMaxMinor( QwtPlot::xBottom, 9 );
    // setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    setAutoReplot( true );


}

void MyPlot::usrDataFile (char *fname) {

  // QTextStream(stdout) << "*** DEBUG: usrDataFile " << "fname= " << fname << endl;

  QDir *dirCalc = new QDir;
  dirCalc->setPath("diff");
  QStringList filefilter;
  filefilter << QString(fname)+QString(".*");
  dirCalc->setNameFilters(filefilter);
  QStringList cStrList = dirCalc->entryList(QDir::Files, QDir::Name);
  cStrList.sort();

  // Start Log
  QString AFIT_INP_TMP = "afit.inp.tmp";
  QString AFIT_OUT = "afit.out";
  QString DATE = "na";
  QString E = "na";
  QString version = "na";
  QString p0 = "na";
  QString s0 = "na";
  QString ss = "na";
  QString zs = "na";
  QString zm = "na";
  QString zx = "na";
  QString zy = "na";

  QFile afile(AFIT_INP_TMP);
  QFile ofile(AFIT_OUT);
  if (ofile.exists() && afile.open(QIODevice::ReadOnly)){
    QTextStream stream( &afile );
    QString sline;
     while ( !stream.atEnd() ) {
         sline = stream.readLine(); // line of text excluding '\n'
         QString strLine = sline.simplified();
         if (strLine.contains("version:")) version = strLine.section(':',1,1);
         if (strLine.contains("energy:")) E = strLine.section(':',1,1);
         if (strLine.contains("p0:")) p0 = strLine.section(':',1,1).simplified().section(' ',0,0);
         if (strLine.contains("s0:")) s0 = strLine.section(':',1,1).simplified().section(' ',0,0);
         if (strLine.contains("ss:")) ss = strLine.section(':',1,1).simplified().section(' ',0,0);
         if (strLine.contains("ZS:")) zs = strLine.section(':',1,1).simplified();
         if (strLine.contains("ZM:")) zm = strLine.section(':',1,1).simplified();
         if (strLine.contains("ZX:")) zx = strLine.section(':',1,1).simplified();
         if (strLine.contains("ZY:")) zy = strLine.section(':',1,1).simplified();
         if (strLine.contains("# Date")) DATE = strLine.section(':',1,1);

     }
 }
 afile.close();

 //QTextStream(stdout) << "*** DEBUG: usrDataFile " << " p0 " << p0 << endl;
  // End of Log


  double xMin = 1000;  double xMax = -1000;
  double yMin = 1000;  double yMax = -1000;
  double zMin = 1000;  double zMax = -1000;
  double dmMin = 1000; double dmMax = -1000;
  double ddMin = 1000; double ddMax = -1000;

  // removeCurves();
  // removeMarkers();
  QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotCurve);
  QwtPlotDict::detachItems(QwtPlotItem::Rtti_PlotMarker);

  // QTextStream(stdout) << "*** DEBUG: usrDataFile " << " cStrList " << cStrList.join(" ") << endl;
  int i = 0;
  for ( QStringList::Iterator it = cStrList.begin(); it != cStrList.end(); ++it ) {
        // QTextStream(stdout) << "*it= " << *it << endl;

        QString lightColor = "black";
        QString darkColor = "darkGray";
        if (i == 0) {lightColor = "red"; darkColor = "darkRed";}
        if (i == 1) {lightColor = "blue"; darkColor = "darkBlue";}
        if (i == 2) {lightColor = "green"; darkColor = "darkGreen";}
        if (i == 3) {lightColor = "magenta"; darkColor = "darkMagenta";}
        if (i == 4) {lightColor = "cyan"; darkColor = "darkCyan";}
        if (i == 5) {lightColor = "yellow"; darkColor = "darkYellow";}

        QString fullFileName=*it;
        QString depth=fullFileName.section('.',1,1);
        QString fs=fullFileName.section('x',0,0);

        // Measured Curve  ----------------------
        QString mFNAME("diff/");
        mFNAME.append(*it);
        //QTextStream(stdout) << "*** DEBUG: usrDataFile mFNAME = " << mFNAME << endl;
        ifstream meas_file;
        meas_file.open(mFNAME.toLatin1().data(),ios::in);
        if (meas_file.bad()) err_Exit("PlotAir::usrDataFile","cannot open meas input file",8);

        int im = 0;
        while (!meas_file.eof()) {
            char line[256] = "";  // lines to read from file
            meas_file.getline(line,sizeof(line));
            istringstream line_stream(line);
            x[im] = 0.0; y[im] = 0.0; z[im] = 0.0;
            dc[im] = 0.0; dm[im] = 0.0; dd[im] = 0.0;
            line_stream >> x[im] >> y[im] >> z[im] >> dc[im] >> dm[im] >> dd[im];
            //QTextStream (stdout) << im << "  " << x[im] << "  " << y[im] << "  " << z[im]
            //        << "  " << dc[im] << "  " << dm[im] << "  " << dd[im]
            //        << endl;
            if (x[im] == 0.0 && y[im] == 0.0 && z[im] == 0.0 ||
                 dc[im] == 0.0 && dm[im] == 0.0 && dd[im] == 0.0) {
            } else {
              if (x[im] <= xMin) xMin = x[im];
              if (x[im] >= xMax) xMax = x[im];
              if (y[im] <= yMin) yMin = y[im];
              if (y[im] >= yMax) yMax = y[im];
              if (z[im] <= zMin && z[im] > 0) zMin = z[im];
              if (z[im] >= zMax) zMax = z[im];
              // if (dm[im] <= dmMin && dm[im] > 0) dmMin = dm[im];
              if (dm[im] <= dmMin) dmMin = dm[im];
              if (dm[im] >= dmMax) dmMax = dm[im];
              if (dd[im] <= ddMin) ddMin = dd[im];
              if (dd[im] >= ddMax) ddMax = dd[im];
           }
            im++;
        }
        nData = im-1;
        // close file
        meas_file.close();
        // QTextStream (stdout) << "Before Plotting ---------------- nData = " << nData << endl;
        QString mLegend="";
        QString cLegend="";
        QString dLegend="";

        if (strncmp("x",fname,1) == 0) {
            // mLegend.append("m: ").append(depth);
            QTextStream(&mLegend) << "m: " << depth;
            QTextStream(&cLegend) << "c: " << depth;
            QTextStream(&dLegend) << "d: " << depth;
        }
        if (strncmp("y",fname,1) == 0) {
            QTextStream(&mLegend) << "m: " << depth;
            QTextStream(&cLegend) << "c: " << depth;
            QTextStream(&dLegend) << "d: " << depth;
        }
        if (strncmp("z",fname,1) == 0) {
            QTextStream(&mLegend) << "m: PDD";
            QTextStream(&cLegend) << "c: PDD";
            QTextStream(&dLegend) << "d: PDD";
        }

        //QTextStream (stdout) << "mLegend " << mLegend << endl;
        //QTextStream (stdout) << "cLegend " << cLegend << endl;
        //QTextStream (stdout) << "dLegend " << dLegend << endl;


        // Curve definition for measured data
        crvm[i] = new QwtPlotCurve(mLegend);
        crvm[i]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crvm[i]->setPen(QPen(Qt::darkBlue));
        crvm[i]->setYAxis(QwtPlot::yLeft);
        crvm[i]->attach(this);

        //QTextStream (stdout) << "Check Point after crvm[i] "<< endl;

        // Curve definition for fitted data
        crvc[i] = new QwtPlotCurve(cLegend);
        crvc[i]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crvc[i]->setPen(QPen(Qt::darkRed));
        crvc[i]->setYAxis(QwtPlot::yLeft);
        crvc[i]->attach(this);

        //QTextStream (stdout) << "Check Point after crvc[i] "<< endl;

        // Curve definition for difference
        crvd[i] = new QwtPlotCurve(dLegend);
        crvd[i]->setRenderHint(QwtPlotItem::RenderAntialiased);
        crvd[i]->setPen(QPen(Qt::darkYellow));
        crvd[i]->setYAxis(QwtPlot::yLeft);
        crvd[i]->attach(this);
        // toggleCurve(crvd[i]);

        //QTextStream (stdout) << "Check Point after crvd[i] "<< endl;

        QString titleText="";
        QString PWD = getenv("PWD");
        QTextStream(&titleText) << "In-air Fitting: "
//		                     << readSettingString("Machine/Machine")
                               << PWD.section('/',-2,-2)
                             << ": " << fname;
       setTitle(titleText);

       //QTextStream (stdout) << "Check Point Title [i] "<< i << endl;

        if (strncmp("x",fname,1) == 0) {
            crvm[i]->rtti();
            crvm[i]->setSamples(x, dc, nData);
            crvc[i]->setSamples(x, dm, nData);
            crvd[i]->setSamples(x, dd, nData);
        }
        if (strncmp("y",fname,1) == 0) {
            crvm[i]->setSamples(y, dc, nData);
            crvc[i]->setSamples(y, dm, nData);
            crvd[i]->setSamples(y, dd, nData);
        }
        if (strncmp("z",fname,1) == 0) {
            crvm[i]->setSamples(z, dc, nData);
            crvc[i]->setSamples(z, dm, nData);
            crvd[i]->setSamples(z, dd, nData);
        }

        i++;
  }

  // QTextStream (stdout) << "** DEBUG: Check Point I " << endl;

  QString FNAME = fname;
  // cout << FNAME << endl;
  QString FWFW = "";

  if (strncmp("x",fname,1) == 0) {
     FWFW = FNAME.section("x",1,1).section("x",0,0);
     QString FW = FWFW;
     // cout << "FWFW = " << FWFW << " FW = " << FW << endl;
     bool ok;
     float fw = FW.toFloat(&ok)*1.5/10;
      // cout << "xMin = " << xMin << "  xMax = " << xMax << " fw = " << fw << endl;
     if (xMin < -fw) xMin = -fw;
     if (xMax > fw)  xMax = fw;
      setAxisTitle(QwtPlot::xBottom, "Distance from CAX (cm)");
      setAxisScale(QwtPlot::xBottom, xMin, xMax,0);
      dmMin = 0;
      setAxisScale(QwtPlot::yLeft, dmMin, dmMax*1.1,0);
  }

  if (strncmp("y",fname,1) == 0) {
      FWFW = FNAME.section("y",1,1).section("x",1,1);
     QString FW = FWFW;
     // cout << "FWFW = " << FWFW << " FW = " << FW << endl;
     bool ok;
     float fw = FW.toFloat(&ok)*1.5/10;
      // cout << "yMin = " << yMin << "  yMax = " << yMax << " fw = " << fw << endl;
     if (yMin < -fw) yMin = -fw;
     if (yMax > fw)  yMax = fw;
      setAxisTitle(QwtPlot::xBottom, "Distance from CAX (cm)");
      setAxisScale(QwtPlot::xBottom, yMin, yMax,0);
      dmMin = 0;
      setAxisScale(QwtPlot::yLeft, dmMin, dmMax*1.1,0);
  }

  if (strncmp("z",fname,1) == 0) {
      setAxisTitle(QwtPlot::xBottom, "Depth (cm)");
      setAxisScale(QwtPlot::xBottom, zMin, zMax,0);
      setAxisScale(QwtPlot::yLeft, dmMin, dmMax*1.02,0);
  }


  // QTextStream (stdout) << "** DEBUG: Check Point II " << endl;
  setAxisScale(QwtPlot::yRight, ddMin, ddMax,0);
/*
  // setMarkerLineStyle(textPos, QwtMarker::VLine);
  if (strncmp("x",fname,1) == 0) {
      QTextStream (stdout) << "** DEBUG: Check Point III x  " << xMin << " " << dmMax*1.1+(dmMax-dmMin)*0.1/8*0.8 << endl;
     // textMarker1->setValue(xMin,dmMax*1.1+(dmMax-dmMin)*0.1/8*0.8);
  }

  if (strncmp("y",fname,1) == 0) {
      QTextStream (stdout) << "** DEBUG: Check Point III y  " << xMin << " " << dmMax*1.1+(dmMax-dmMin)*0.1/8*0.8 << endl;
     // textMarker1->setValue(yMin,dmMax*1.1+(dmMax-dmMin)*0.1/8*0.8);
  }
  if (strncmp("z",fname,1) == 0) {
      QTextStream (stdout) << "** DEBUG: Check Point III z  " << xMin << " " << dmMax*1.1+(dmMax-dmMin)*0.1/8*0.8 << endl;
     // textMarker1->setValue(zMin,dmMax*1.02+(dmMax-dmMin)*0.1/8*0.8);
  }

  QTextStream (stdout) << "** DEBUG: Check Point IV " << endl;

  //textMarker1->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
  QTextStream (stdout) << "** DEBUG: Check Point IV - 1" << endl;
  textMarker1->setLinePen(QPen(Qt::blue, 0, Qt::DashDotLine));

  QwtText textLabel1(
          "Date:"+DATE+" Ver:"+version+" E="+E+" Pp="+p0+" Sp="+s0
          +" Ss="+ss+" ZS="+zs+" ZX="+zx+" ZY="+zy+" ZM="+zm);

  QTextStream (stdout) << "** DEBUG: Check Point IV - 2 " << endl;
  textLabel1.setFont(QFont("Helvetica", 8, QFont::Normal));

  QTextStream (stdout) << "** DEBUG: Check Point IV -3 " << endl;
  textMarker1->setLabel(textLabel1);

  QTextStream (stdout) << "** DEBUG: Check Point V " << endl;
  textMarker2->setValue(xMin+(xMax-xMin)*1.5/2,yMax*1.1-(yMax-yMin)*0.2*2/5);
  textMarker2->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
  textMarker2->setLinePen(QPen(Qt::blue, 0, Qt::DashDotLine));
  QwtText textLabel2("");
  textLabel2.setFont(QFont("Helvetica", 10, QFont::Normal));
  textMarker1->setLabel(textLabel2);
*/
  //QTextStream (stdout) << "** DEBUG: Check Point END " << endl;

  replot();

  // cout << "yMin=" << yMin << "  yMax=" << yMax << endl;

} // End of UsrDataFile

void MyPlot::showLegend() {
    if (LegendSwitch == "Show") {

         // legend
         QwtLegend *legend = new QwtLegend;
         legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
         // legend->setLegendFrameStyle(QFrame::NoFrame|QFrame::Plain);
         //insertLegend(legend, QwtPlot::BottomLegend);
         this->insertLegend(legend, QwtPlot::RightLegend);
    }
    else {
         this->insertLegend(NULL);
    }
    //QwtPlotCurve::updateLegend(legend);
    replot();
}

void MyPlot::showDD() {
     if (DDSwitch == "Show") {

        // enableYRightAxis(TRUE);
        this->enableAxis(yRight,true);
        setAxisTitle(QwtPlot::yRight, "Percent Dose Difference");
     }
     else  {
         this->enableAxis(yRight, false);
     }
     replot();
}

// error handler
void MyPlot::err_Exit(char *location, char *reason, int exit_code)
{
   cerr << endl;
   cerr << "RUN-TIME ERROR in " << location << endl;
   cerr << "Reason: " << reason << "!" << endl;
   cerr << "Exiting to system..." << endl;
   cerr << endl;
   exit(exit_code);
}

