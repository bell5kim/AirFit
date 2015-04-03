#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qprintdialog.h>

#include <qwt_counter.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_text.h>
#include <qwt_math.h>

#include <QDomElement>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include "pixmaps.h"
#include "myplot.h"
#include "airfit.h"
#include "ui_airfit.h"

using namespace std;
#include <iostream>
#include <cmath>
#include "AirFitInput.h"

#ifndef XVMC
#define XVMC on
#endif

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QWidget *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }
};

// This should be declared at very beginning --> Removed
AirFitInput *afit = new AirFitInput;

AirFit::AirFit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AirFit)
{
    ui->setupUi(this);

    // Plot *damping = new Plot(this);

    const int margin = 5;
    ui->myPlot->setContentsMargins( margin, margin, margin, 0 );

    setContextMenuPolicy( Qt::NoContextMenu );

    d_zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,
        ui->myPlot->canvas() );
    d_zoomer[0]->setRubberBand( QwtPicker::RectRubberBand );
    d_zoomer[0]->setRubberBandPen( QColor( Qt::green ) );
    d_zoomer[0]->setTrackerMode( QwtPicker::ActiveOnly );
    d_zoomer[0]->setTrackerPen( QColor( Qt::white ) );

    d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,
         ui->myPlot->canvas() );

    d_panner = new QwtPlotPanner( ui->myPlot->canvas() );
    d_panner->setMouseButton( Qt::MidButton );

    d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        ui->myPlot->canvas() );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
    d_picker->setRubberBandPen( QColor( Qt::green ) );
    d_picker->setRubberBand( QwtPicker::CrossRubberBand );
    d_picker->setTrackerPen( QColor( Qt::white ) );

    // setCentralWidget( ui->myPlot );

    QToolBar *toolBar = new QToolBar( this );

    QToolButton *btnZoom = new QToolButton( toolBar );
    btnZoom->setText( "Zoom" );
    btnZoom->setIcon( QPixmap( zoom_xpm ) );
    btnZoom->setCheckable( true );
    btnZoom->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnZoom );
    connect( btnZoom, SIGNAL( toggled( bool ) ), SLOT( enableZoomMode( bool ) ) );

#ifndef QT_NO_PRINTER
    QToolButton *btnPrint = new QToolButton( toolBar );
    btnPrint->setText( "Print" );
    btnPrint->setIcon( QPixmap( print_xpm ) );
    btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnPrint );
    connect( btnPrint, SIGNAL( clicked() ), SLOT( print() ) );
#endif

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setIcon( QPixmap( print_xpm ) );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnExport );
    connect( btnExport, SIGNAL( clicked() ), SLOT( exportDocument() ) );

    QToolButton *btnTable = new QToolButton( toolBar );
    btnTable->setText( "Table" );
    btnTable->setIcon( QPixmap( table_xpm ) );
    btnTable->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnTable );
    connect( btnTable, SIGNAL( clicked() ), SLOT( runGammaTable() ) );

/*
    toolBar->addSeparator();

    QWidget *hBox = new QWidget( toolBar );

    QHBoxLayout *layout = new QHBoxLayout( hBox );
    layout->setSpacing( 0 );
    layout->addWidget( new QWidget( hBox ), 10 ); // spacer
    layout->addWidget( new QLabel( "Damping Factor", hBox ), 0 );
    layout->addSpacing( 10 );

    QwtCounter *cntDamp = new QwtCounter( hBox );
    cntDamp->setRange( 0.0, 5.0 );
    cntDamp->setSingleStep( 0.01 );
    cntDamp->setValue( 0.0 );

    layout->addWidget( cntDamp, 0 );

    ( void )toolBar->addWidget( hBox );
*/

    addToolBar( toolBar );

#ifndef QT_NO_STATUSBAR
    ( void )statusBar();
#endif

    enableZoomMode( false );
    showInfo();

    //connect( cntDamp, SIGNAL( valueChanged( double ) ),
    //    ui->myPlot, SLOT( setDamp( double ) ) );

    // connect( cntDamp, &QwtCounter::valueChanged, damping, &Plot::damp );
    // connect( cntDamp, &QwtCounter::valueChanged, ui->myPlot, &Plot::damp );

    connect( d_picker, SIGNAL( moved( const QPoint & ) ),
        SLOT( moved( const QPoint & ) ) );

    connect( d_picker, SIGNAL( selected( const QPolygon & ) ),
        SLOT( selected( const QPolygon & ) ) );

    init();

    setAirFitValues("afit.inp.tmp");
    // myPlot->showToolBar(this);
    ui->checkBoxAutoPlot->setChecked(true);
    iListBox = 3; // set to zListBox
    iListItem = 0; // set to first item
    plotAirFit();
    resetAirFitValues();

}

AirFit::~AirFit()
{
    delete ui;
}

// CLASS Status ------------------------------------------------------
#define XML on
#ifdef XML
class Status {
  public:
     QString value, date;
};

QDomElement StatusToNode (QDomDocument &d, const Status &s, QString e){
   QDomElement elm = d.createElement(e);
   elm.setAttribute("value", s.value);
   elm.setAttribute("date", s.date);
   return elm;
};
#endif


#ifndef QT_NO_PRINTER

void AirFit::print()
{
    QPrinter printer( QPrinter::HighResolution );

    QString docName = ui->myPlot->title().text();
    if ( !docName.isEmpty() )
    {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( "Air Fitting Report" );
    printer.setOrientation( QPrinter::Landscape );
    printer.setPageSize(QPrinter::Letter);
    printer.setColorMode(QPrinter::Color);

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( ui->myPlot, printer );
    }
}

#endif

void AirFit::exportDocument()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( ui->myPlot, "AirFit.pdf" );
}

void AirFit::enableZoomMode( bool on )
{
    d_panner->setEnabled( on );

    d_zoomer[0]->setEnabled( on );
    d_zoomer[0]->zoom( 0 );

    d_zoomer[1]->setEnabled( on );
    d_zoomer[1]->zoom( 0 );

    d_picker->setEnabled( !on );

    showInfo();
}

void AirFit::showInfo( QString text )
{
    if ( text == QString::null )
    {
        if ( d_picker->rubberBand() )
            text = "Cursor Pos: Press left mouse button in plot region";
        else
            text = "Zoom: Press mouse button and drag";
    }

#ifndef QT_NO_STATUSBAR
    statusBar()->showMessage( text );
#endif
}

void AirFit::moved( const QPoint &pos )
{
    QString info;
    info.sprintf( "x=%g, Dose=%g, Diff=%g",
        ui->myPlot->invTransform( QwtPlot::xBottom, pos.x() ),
        ui->myPlot->invTransform( QwtPlot::yLeft, pos.y() ),
        ui->myPlot->invTransform( QwtPlot::yRight, pos.y() )
    );
    showInfo( info );
}

void AirFit::selected( const QPolygon & )
{
    showInfo();
}

// ----------------------------------------------------------
void AirFit::resetZoomer(){
    d_zoomer[0]->setZoomBase();
    d_zoomer[1]->setZoomBase();
}

void AirFit::runGammaTable() {

   // char fname[256];

   QString LBIN;
   LBIN = getenv("XVMC_HOME");

   QString CMD = LBIN + "/gammatable.exe -i ";

   QString AFIT_DIR = getenv("PWD");
   QFile mFile(AFIT_DIR+"/afit.lst");
   QString FNAME(ui->myPlot->fname);
   if (mFile.exists()) {
      QTextStream stream( &mFile );
      QString sLine;
      mFile.open( QIODevice::ReadOnly );
      while ( !stream.atEnd() ) {
        sLine = stream.readLine();
        QString strLine = sLine.simplified();
        if (strLine.contains(ui->myPlot->fname)) {
           CMD = CMD + "diff/"
                     + strLine.section('\t',0,0).section('/',1,1)
                     + ":";
        }
      }
      mFile.close();
   }

  // cout << CMD << endl;
  if (system(CMD.toUtf8().constData()) != 0) {
     cout << "ERROR: Somethings are wrong: " << CMD.toUtf8().constData() << '\n' ;
  }
}


void AirFit::init() {

    int m = 6;
    int n = 3;
    int mFactor = 10;

    // Environment Settings -------------------------------------
    QString XVMC_WORK = getenv("XVMC_WORK"); // XVMC_WORK = getenv("PWD");
    QString mName = XVMC_WORK.section('/',-1);
    ui->textLabelMachine->setText(mName);

    // P0 Group Settings    -------------------------------------
    ui->widgetP0->setCheckBoxText("P0: Prob. of Primary Photon");
    ui->widgetP0->setSpinBoxRange(0,1.0);
    // ui->widgetP0->->validate(1,4); // Not implemented Yet!
    ui->widgetP0->validate(4);
    ui->widgetP0->setMaximum(1.0);
    ui->widgetP0->setMinimum(0.0);
    ui->widgetP0->setSpinStep(0.01);
    ui->widgetP0->setSliderRange(0,(int)pow(10.0,1+4-1));
    ui->widgetP0->setSliderStep((int)pow(10.0,1+4-2));
    ui->widgetP0->setValue(0.9);

    // S0 Group Setting     -------------------------------------
    ui->widgetS0->setCheckBoxText("S0: Sigma for Primary (cm)");
    ui->widgetS0->setSpinBoxRange(0,1.0);
    ui->widgetS0->validate(4);
    ui->widgetS0->setSpinStep(0.01);
    ui->widgetS0->setSliderRange(0,(int)pow(10.0,1+4-1));
    ui->widgetS0->setSliderStep((int)pow(10.0,1+4-2));
    ui->widgetS0->setValue(0.1);

    // SS Group Setting     -------------------------------------
    ui->widgetSS->setCheckBoxText("SS: Sigma for Scatter (cm)");
    ui->widgetSS->setSpinBoxRange(0,10.0);
    ui->widgetSS->validate(4);
    ui->widgetSS->setSpinStep(0.01);
    ui->widgetSS->setSliderRange(0,(int)pow(10.0,1+4-1));
    ui->widgetSS->setSliderStep((int)pow(10.0,1+4-2));
    ui->widgetSS->setValue(0.0);

    // H0 Group Settings    -------------------------------------
    ui->widgetH0->setCheckBoxText("h0: 0th order");
    ui->widgetH0->setSpinBoxRange(-100000.0,100000.0);
    ui->widgetH0->validate(1);
    ui->widgetH0->setSpinStep(1000.0);
    ui->widgetH0->setSliderRange(-1*(int)pow(10.0,m+n-1),(int)pow(10.0,m+n-1));
    ui->widgetH0->setSliderStep((int)pow(10.0,m+n-2));
    ui->widgetH0->setValue(0.0);

    // H1 Group Settings    -------------------------------------
    ui->widgetH1->setCheckBoxText("h1: 1st order");
    ui->widgetH1->setSpinBoxRange(-100000.0,100000.0);
    ui->widgetH1->validate(1);
    ui->widgetH1->setSpinStep(1000.0);
    ui->widgetH1->setSliderRange(-1*(int)pow(10.0,m+n-1),(int)pow(10.0,m+n-1));
    ui->widgetH1->setSliderStep((int)pow(10.0,m+n-2));
    ui->widgetH1->setValue(0.0);

    // H2 Group Settings    -------------------------------------
    ui->widgetH2->setCheckBoxText("h2: 2nd order");
    ui->widgetH2->setSpinBoxRange(-100000.0,100000.0);
    ui->widgetH2->validate(1);
    ui->widgetH2->setSpinStep(1000.0);
    ui->widgetH2->setSliderRange(-1*(int)pow(10.0,m+n-1),(int)pow(10.0,m+n-1));
    ui->widgetH2->setSliderStep((int)pow(10.0,m+n-2));
    ui->widgetH2->setValue(0.0);

    // H3 Group Settings    -------------------------------------
    ui->widgetH3->setCheckBoxText("h3: 3rd order");
    ui->widgetH3->setSpinBoxRange(-50000.0*mFactor,50000.0*mFactor);
    ui->widgetH3->validate(1);
    ui->widgetH3->setSpinStep(1000.0);
    ui->widgetH3->setSliderRange(-5*(int)pow(10.0,m+n-1),5*(int)pow(10.0,m+n-1));
    ui->widgetH3->setSliderStep(5*(int)pow(10.0,m+n-2));
    ui->widgetH3->setValue(0.0);

    // H3 Group Settings    -------------------------------------
    ui->widgetH4->setCheckBoxText("h4: 4th order");
    ui->widgetH4->setSpinBoxRange(-20000.0*mFactor,20000.0*mFactor);
    ui->widgetH4->validate(1);
    ui->widgetH4->setSpinStep(1000.0);
    ui->widgetH4->setSliderRange(-20*(int)pow(10.0,m+n-1),20*(int)pow(10.0,m+n-1));
    ui->widgetH4->setSliderStep(100*(int)pow(10.0,m+n-2));
    ui->widgetH4->setValue(0.0);

    // Line Editor Validations  -------------------------------------
    QRegExp regExp2d("[1-9]\\d{0,1}");
    QRegExp regExpReal("\\d{0,3}\\.\\d{0,2}");
    ui->lineEditE->setValidator(new QRegExpValidator(regExp2d, this));
    ui->lineEditZ0->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditZS->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditZM->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditZX->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditZY->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditXN->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditYN->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditZN->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditWXN->setValidator(new QRegExpValidator(regExpReal, this));
    ui->lineEditWYN->setValidator(new QRegExpValidator(regExpReal, this));

    // User Functions   -------------------------------------
    initAirFit();
    loadData();

}

// ----------------------------------------------
void AirFit::initAirFit() {

    afit->initAirFitInput();
    // QTextStream(stdout) << "* initAirFit got initAirFitInput " <<  endl;
}


// Line Editors ----------------------------------------------
void AirFit::setLineEditE()
{
    QString usrText;
    usrText.setNum(afit->E);
    ui->lineEditE->setText(usrText);
}

void AirFit::setLineEditZ0()
{
    QString usrText;
    usrText.setNum(afit->Z0);
    ui->lineEditZ0->setText(usrText);
}

void AirFit::setLineEditZS()
{
    QString usrText;
    usrText.setNum(afit->ZS);
    ui->lineEditZS->setText(usrText);
}

void AirFit::setLineEditZM()
{
    QString usrText;
    usrText.setNum(afit->ZM);
    ui->lineEditZM->setText(usrText);
}

void AirFit::setLineEditZX()
{
    QString usrText;
    usrText.setNum(afit->ZX);
    ui->lineEditZX->setText(usrText);
}

void AirFit::setLineEditZY()
{
    QString usrText;
    usrText.setNum(afit->ZY);
    ui->lineEditZY->setText(usrText);
}

void AirFit::setLineEditZI()
{
    QString usrText;
    usrText.setNum(afit->ZI);
    ui->lineEditZI->setText(usrText);
}

void AirFit::setLineEditXN()
{
    QString usrText;
    usrText.setNum(afit->XN);
    ui->lineEditXN->setText(usrText);
}

void AirFit::setLineEditYN()
{
    QString usrText;
    usrText.setNum(afit->YN);
    ui->lineEditYN->setText(usrText);
}

void AirFit::setLineEditZN()
{
    QString usrText;
    usrText.setNum(afit->ZN);
    ui->lineEditZN->setText(usrText);
}

void AirFit::setLineEditWXN()
{
    QString usrText;
    usrText.setNum(afit->WXN);
    ui->lineEditWXN->setText(usrText);
}

void AirFit::setLineEditWYN()
{
    QString usrText;
    usrText.setNum(afit->WYN);
    ui->lineEditWYN->setText(usrText);
}

void AirFit::setLineEditDev()
{
    QString usrText;
    usrText.setNum(afit->DEV);
    ui->lineEditDev->setText(usrText);
}

void AirFit::setLineEditChi()
{
    QString usrText;
    usrText.setNum(afit->CHI);
    ui->lineEditChi->setText(usrText);
}


// Float Spin Box ---------------------------------------------
void AirFit::setFloatSpinBoxP0() {
     ui->widgetP0->setValue(afit->p0);
}
void AirFit::setFloatSpinBoxS0() {
     ui->widgetS0->setValue(afit->s0);
}
void AirFit::setFloatSpinBoxSS() {
     ui->widgetSS->setValue(afit->ss);
}
void AirFit::setFloatSpinBoxH0() {
     ui->widgetH0->setValue(afit->h0);
}
void AirFit::setFloatSpinBoxH1() {
     ui->widgetH1->setValue(afit->h1);
}
void AirFit::setFloatSpinBoxH2() {
     ui->widgetH2->setValue(afit->h2);
}
void AirFit::setFloatSpinBoxH3() {
     ui->widgetH3->setValue(afit->h3);
}
void AirFit::setFloatSpinBoxH4() {
     ui->widgetH4->setValue(afit->h4);
}


// Set H Spin Box ---------------------------------------------
void AirFit::setAllHZero() {
     ui->widgetH0->setValue(0.0);
     ui->widgetH1->setValue(0.0);
     ui->widgetH2->setValue(0.0);
     ui->widgetH3->setValue(0.0);
     ui->widgetH4->setValue(0.0);
     toggleCheckBoxRight();
}

void AirFit::setCheckBoxP0() {
    if(afit->ip0 == 1) ui->widgetP0->setChecked(false);
    if(afit->ip0 == 0) ui->widgetP0->setChecked(true);
}
void AirFit::setCheckBoxS0() {
    if(afit->is0 == 1) ui->widgetS0->setChecked(false);
    if(afit->is0 == 0) ui->widgetS0->setChecked(true);
}
void AirFit::setCheckBoxSS() {
    if(afit->iss == 1) ui->widgetSS->setChecked(false);
    if(afit->iss == 0) ui->widgetSS->setChecked(true);
}

void AirFit::setCheckBoxH0() {
    if(afit->ih0 == 1) ui->widgetH0->setChecked(false);
    if(afit->ih0 == 0) ui->widgetH0->setChecked(true);
}
void AirFit::setCheckBoxH1() {
    if(afit->ih1 == 1) ui->widgetH1->setChecked(false);
    if(afit->ih1 == 0) ui->widgetH1->setChecked(true);
}
void AirFit::setCheckBoxH2() {
    if(afit->ih2 == 1) ui->widgetH2->setChecked(false);
    if(afit->ih2 == 0) ui->widgetH2->setChecked(true);
}
void AirFit::setCheckBoxH3() {
    if(afit->ih3 == 1) ui->widgetH3->setChecked(false);
    if(afit->ih3 == 0) ui->widgetH3->setChecked(true);
}
void AirFit::setCheckBoxH4() {
    if(afit->ih4 == 1) ui->widgetH4->setChecked(false);
    if(afit->ih4 == 0) ui->widgetH4->setChecked(true);
}

void AirFit::setCheckBoxMX() {
    if(afit->MX == 1) ui->checkBoxMX->setChecked(true);
    if(afit->MX == 0) ui->checkBoxMX->setChecked(false);
}

void AirFit::setCheckBoxMY() {
    if(afit->MY == 1) ui->checkBoxMY->setChecked(true);
    if(afit->MY == 0) ui->checkBoxMY->setChecked(false);
}

void AirFit::toggleCheckBoxLeft() {
    ui->widgetP0->toggle();
    ui->widgetS0->toggle();
    ui->widgetSS->toggle();
}

void AirFit::toggleCheckBoxRight() {
    ui->widgetH0->toggle();
    ui->widgetH1->toggle();
    ui->widgetH2->toggle();
    ui->widgetH3->toggle();
    ui->widgetH4->toggle();
}


// ---------------------------------------------
void AirFit::getFloatSpinBoxP0() {
     afit->p0 = ui->widgetP0->value();
     if(ui->widgetP0->isChecked()) afit->ip0 = 0;
     else afit->ip0 = 1;
}
void AirFit::getFloatSpinBoxS0() {
     afit->s0 = ui->widgetS0->value();
     if(ui->widgetS0->isChecked()) afit->is0 = 0;
     else afit->is0 = 1;
}
void AirFit::getFloatSpinBoxSS() {
     afit->ss = ui->widgetSS->value();
     if(ui->widgetSS->isChecked()) afit->iss = 0;
     else afit->iss = 1;
}
void AirFit::getFloatSpinBoxH0() {
     afit->h0 = ui->widgetH0->value();
     if(ui->widgetH0->isChecked()) afit->ih0 = 0;
     else afit->ih0 = 1;
}
void AirFit::getFloatSpinBoxH1() {
     afit->h1 = ui->widgetH1->value();
     if(ui->widgetH1->isChecked()) afit->ih1 = 0;
     else afit->ih1 = 1;
}
void AirFit::getFloatSpinBoxH2() {
     afit->h2 = ui->widgetH2->value();
     if(ui->widgetH2->isChecked()) afit->ih2 = 0;
     else afit->ih2 = 1;
}
void AirFit::getFloatSpinBoxH3() {
     afit->h3 = ui->widgetH3->value();
     if(ui->widgetH3->isChecked()) afit->ih3 = 0;
     else afit->ih3 = 1;
}
void AirFit::getFloatSpinBoxH4() {
     afit->h4 = ui->widgetH4->value();
     if(ui->widgetH4->isChecked()) afit->ih4 = 0;
     else afit->ih4 = 1;
}

void AirFit::getCheckBoxMX() {
 if(ui->checkBoxMX->isChecked()) afit->MX = 1;
 else afit->MX = 0;
}

void AirFit::getCheckBoxMY() {
 if(ui->checkBoxMY->isChecked()) afit->MY = 1;
 else afit->MY = 0;
}

void AirFit::getLineEditZS(){
 bool ok;
   afit->ZS = ui->lineEditZS->text().toFloat(&ok);
}
void AirFit::getLineEditZM(){
 bool ok;
   afit->ZM = ui->lineEditZM->text().toFloat(&ok);
}
void AirFit::getLineEditZX(){
 bool ok;
   afit->ZX = ui->lineEditZX->text().toFloat(&ok);
}
void AirFit::getLineEditZY(){
 bool ok;
   afit->ZY = ui->lineEditZY->text().toFloat(&ok);
}
void AirFit::getLineEditZN(){
 bool ok;
   afit->ZN = ui->lineEditZN->text().toFloat(&ok);
}


// --------------------------------------------------------------------------------
void AirFit::setAirFitValues(char *fname){

 readAirFit(fname);
 setLineEditE();
 setLineEditZ0();
 setLineEditZS();
 setLineEditZM();
 setLineEditZX();
 setLineEditZY();
 setLineEditZI();
 setLineEditXN();
 setLineEditYN();
 setLineEditZN();
 setLineEditWXN();
 setLineEditWYN();
 setLineEditChi();
 setLineEditDev();

 setFloatSpinBoxP0();  setCheckBoxP0();
 setFloatSpinBoxS0();  setCheckBoxS0();
 setFloatSpinBoxSS();  setCheckBoxSS();
 setFloatSpinBoxH0();  setCheckBoxH0();
 setFloatSpinBoxH1();  setCheckBoxH1();
 setFloatSpinBoxH2();  setCheckBoxH2();
 setFloatSpinBoxH3();  setCheckBoxH3();
 setFloatSpinBoxH4();  setCheckBoxH4();

 setCheckBoxMX();  setCheckBoxMY();
}

void AirFit::newAirFitValues(){
 setLineEditE();
 setLineEditZ0();
 setLineEditZS();
 setLineEditZM();
 setLineEditZX();
 setLineEditZY();
 setLineEditZI();
 setLineEditXN();
 setLineEditYN();
 setLineEditZN();
 setLineEditWXN();
 setLineEditWYN();
 setLineEditChi();
 setLineEditDev();

 setFloatSpinBoxP0();  setCheckBoxP0();
 setFloatSpinBoxS0();  setCheckBoxS0();
 setFloatSpinBoxSS();  setCheckBoxSS();
 setFloatSpinBoxH0();  setCheckBoxH0();
 setFloatSpinBoxH1();  setCheckBoxH1();
 setFloatSpinBoxH2();  setCheckBoxH2();
 setFloatSpinBoxH3();  setCheckBoxH3();
 setFloatSpinBoxH4();  setCheckBoxH4();

 setCheckBoxMX();  setCheckBoxMY();
}

void AirFit::updateAirFitValues(){ // Update Values from Temp File
    setAirFitValues("afit.inp.tmp");
}


// ----------------------------------------------
void AirFit::readAirFit(char *fname) {
   afit->readAirFitInput(fname);
}

void AirFit::writeAirFit() {
   getFloatSpinBoxP0();
   getFloatSpinBoxS0();
   getFloatSpinBoxSS();
   getFloatSpinBoxH0();
   getFloatSpinBoxH1();
   getFloatSpinBoxH2();
   getFloatSpinBoxH3();
   getFloatSpinBoxH4();

   getCheckBoxMX();
   getCheckBoxMY();

   getLineEditZS();
   getLineEditZM();
   getLineEditZX();
   getLineEditZY();
   getLineEditZN();

   afit->writeAirFitInput();
}


bool AirFit::makeDir(QString dirStr) {
     bool dirExists = false;
     QString lastDir = dirStr.section('/',-1);
     QString dirPath = dirStr.section('/',0,-2);
     QDir *afitDir = new QDir;
     afitDir->setPath(dirStr);
     if(afitDir->exists()) dirExists = true;
     else {
        afitDir->setPath(dirPath);
        afitDir->mkdir(lastDir);
        dirExists = true;
     }
     return (dirExists);
}

void AirFit::plotAirFitOld() {
 QString LBIN = getenv("XVMC_HOME");
 // cout << "AirFit::plotAirFit: LBIN = " << LBIN << endl;
 QString CMD = LBIN + "/plotAir.exe";
   if (system(CMD.toStdString().c_str()) != 0) {
       cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl ;
   }
}

QString AirFit::readLocalSetting(QString keyWord){
   QString XVMC_WORK = getenv("XVMC_WORK");
   // QString XVMC_WORK = getenv("PWD");
   QString group = XVMC_WORK.section('/',-1);
   QString keyValue = "";
#ifdef XML
   QString mName = group;
   QFile xmlFile(XVMC_WORK+"/status.xml");
   // cout << XVMC_WORK+"/status.xml" << endl;
   if (!xmlFile.open(QIODevice::ReadOnly)){
      cout << "No " << XVMC_WORK.toStdString()+"/status.xml is found" << endl;
      exit(-1);
   }

   QDomDocument docType(mName);
   docType.setContent(&xmlFile);
   xmlFile.close();

   QDomElement root = docType.documentElement();
   // cout << "root.tagName()=" << root.tagName() << endl;
   if (root.tagName() != mName) {
      // cout << "Tag Name ("<<root.tagName()<<") is different from " << mName << endl;
      exit(-1);
   }

   QDomNode n = root.firstChild();

   while (!n.isNull()){
      QDomElement e = n.toElement();
      if(!e.isNull()) {
         if(e.tagName() == keyWord) keyValue = e.attribute("value", "");
      }
      n = n.nextSibling();
   }
#else

   QString Entry = "/"+group+"/" + keyWord;
   QSettings settings;
   settings.insertSearchPath( QSettings::Unix, XVMC_WORK);
   settings.beginGroup("/"+group);
   keyValue = settings.readEntry(Entry, "");
   settings.endGroup();
#endif
   return (keyValue.simplified());
}

void AirFit::writeLocalSetting(QString keyWord, QString keyValue){
   QDateTime currentDateTime = QDateTime::currentDateTime();
   QString DT = currentDateTime.toString();
   QString XVMC_WORK = getenv("XVMC_WORK");
   // QString XVMC_WORK = getenv("PWD");
   // QTextStream (stdout) << "writeLocalSetting: XVMC_WORK = " <<  XVMC_WORK << endl;
   QString group = XVMC_WORK.section('/',-1);
   // QTextStream (stdout) <<  "writeLocalSetting: group = " << group << endl;

#ifdef XML
   QString mName = group;
   QFile xmlFile(XVMC_WORK+"/status.xml");
   // QTextStream (stdout) << XVMC_WORK+"/status.xml mName =" <<  mName << endl;
   if (!xmlFile.open(QIODevice::ReadOnly)){
      cout << "No " << XVMC_WORK.toStdString()+"/status.xml is found" << endl;
      exit(-1);
   }

   QDomDocument doc(mName);
   doc.setContent(&xmlFile);
   xmlFile.close();

   QDomElement root = doc.documentElement();
   if (root.tagName() != mName) {
      QTextStream (stdout) << "Tag Name ("<< root.tagName()<<") is different from " << mName << endl;
      exit(-1);
   }

   QDomNode n = root.firstChild();

   // Set attribute for existing tag
   bool isDone = false;
   while (!n.isNull()){
      QDomElement e = n.toElement();
      if(!e.isNull()) {
         if(e.tagName() == keyWord) {
            e.setAttribute("value", keyValue);
            isDone = true;
         }
      }
      n = n.nextSibling();
   }

   if(!isDone) {
      Status s;  // Status class

      QString eName = keyWord;
      s.value = keyValue;  s.date = DT;
      root.appendChild(StatusToNode(doc, s, eName));
   }

   xmlFile.open(QIODevice::WriteOnly);
   QTextStream txtStrm(&xmlFile);
   // QTextStream (stdout) << "doc = " << doc << endl;
   txtStrm << doc.toString();
   xmlFile.close();

#else
   QString Entry = "/"+group+"/" + keyWord;
   //cout << "writeLocalSetting: group = " << group << endl;
   //cout << "writeLocalSetting: Entry = " << Entry << endl;
   QSettings settings;
   settings.insertSearchPath(QSettings::Unix, XVMC_WORK);
   settings.beginGroup("/"+group);
   settings.writeEntry(Entry, keyValue);
   settings.writeEntry(Entry+"_DT", DT);
   settings.endGroup();
   // cout << "End of writeSetting" << endl;
#endif
}

void AirFit::runAirFitNew() {

  bool ok;

  QString EXE = "afit";
  if (ui->checkBoxVersion->isChecked()) EXE="afit_old";
  writeAirFit(); // it creates "afit.inp.tmp"
  QFile aFile("afit.inp.tmp");
  if (aFile.exists()) {
     afit->readAirFitInput("afit.inp.tmp");
     QString LBIN = getenv("XVMC_HOME"); LBIN = QString("~/Work/MonacoBCT/bin");

#ifdef XVMC
     QString CMD = LBIN + "/afit_xvmc afit.inp.tmp > afit.out";
#else
     QString CMD = LBIN + "/" + EXE+ " afit.inp.tmp >& afit.out";
#endif
     if (system(CMD.toStdString().c_str()) != 0) {
        cout << "ERROR: Somethings are wrong: " << CMD.toStdString().c_str() << endl ;
     }
  }

  QString CMDERROR = "";

  QFile mFile("afit.out");
  if (mFile.exists()) {
    QTextStream stream( &mFile );
    QString sLine;
    mFile.open(QIODevice::ReadOnly);
    while ( !stream.atEnd() ) {
      sLine = stream.readLine();
//   QString strLine = sLine.latin1();
      if (!sLine.contains(',')) {
        if (sLine.contains("p0:")) {
           QString p0 = sLine.section(':',1,1).section("+/-",0,0);
           afit->p0 = p0.simplified().toFloat(&ok);
        }
        if (sLine.contains("s0:")) {
           QString s0 = sLine.section(':',1,1).section("+/-",0,0);
           afit->s0 = s0.simplified().toFloat(&ok);
        }
        if (sLine.contains("h0:")) {
           QString h0 = sLine.section(':',1,1).section("+/-",0,0);
           afit->h0 = h0.simplified().toFloat(&ok);
        }
        if (sLine.contains("h1:")) {
           QString h1 = sLine.section(':',1,1).section("+/-",0,0);
           afit->h1 = h1.simplified().toFloat(&ok);
        }
        if (sLine.contains("h2:")) {
           QString h2 = sLine.section(':',1,1).section("+/-",0,0);
           afit->h2 = h2.simplified().toFloat(&ok);
        }
        if (sLine.contains("h3:")) {
           QString h3 = sLine.section(':',1,1).section("+/-",0,0);
           afit->h3 = h3.simplified().toFloat(&ok);
        }
        if (sLine.contains("h4:")) {
           QString h4 = sLine.section(':',1,1).section("+/-",0,0);
           afit->h4 = h4.simplified().toFloat(&ok);
        }
        if (sLine.contains("ss:")) {
           QString ss = sLine.section(':',1,1).section("+/-",0,0);
           afit->ss = ss.simplified().toFloat(&ok);
        }
        if (sLine.contains("Z0:")) {
           QString Z0 = sLine.section(':',1,1);
           afit->Z0 = Z0.simplified().toFloat(&ok);
        }
        if (sLine.contains("ZM:")) {
           QString ZM = sLine.section(':',1,1);
           afit->ZM = ZM.simplified().toFloat(&ok);
        }
        if (sLine.contains("ZX:")) {
           QString ZX = sLine.section(':',1,1);
           afit->ZX = ZX.simplified().toFloat(&ok);
        }
        if (sLine.contains("ZY:")) {
           QString ZY = sLine.section(':',1,1);
           afit->ZY = ZY.simplified().toFloat(&ok);
        }
        if (sLine.contains("ZS:")) {
           QString ZS = sLine.section(':',1,1);
           afit->ZS = ZS.simplified().toFloat(&ok);
        }
        if (sLine.contains("chi:")) {
           QString CHI = sLine.section(':',1,1).section("%",0,0);
           afit->CHI = CHI.simplified().toFloat(&ok);
        }
        if (sLine.contains("Numerical Recipes run-time error...")) {
           CMDERROR = "Numerical Recipes run-time error...";
        }
      } else {
        if (sLine.contains("average deviation:")) {
           QString DEV = sLine.section("average deviation:",1,1)
                              .section("%",0,0);
           afit->DEV = DEV.simplified().toFloat(&ok);
        }
      }
    }

    if (CMDERROR.length() == 0) {
       newAirFitValues();
       afit->writeAirFitInput();
    }

    mFile.close();
  }
  if (CMDERROR.length() > 0)   {
    QMessageBox::critical( 0, "afit",CMDERROR );
    ui->plotButton->setEnabled(false);
    if (aFile.exists()) {
      QString CMD = "rm afit.inp.tmp";
      if (system(CMD.toStdString().c_str()) != 0) {
         cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl ;
      }
    }
  } else {
    // QTextStream (stdout) << "DEBUG: Auto Plot ON" << endl;
    if (ui->checkBoxAutoPlot->isChecked()) plotAirFit();
    ui->plotButton->setEnabled(true);
  }
}



void AirFit::writeBDT(QString bdtFile, QString pType, float pCon) {

 QString pwd = getenv("PWD");
 QString machineInfo = pwd.section('/',0,-2) + "/"
                     + pwd.section('/',-2,-2) + ".info";
 QString MAXFW = "";
 QString MAXFL = "";
 QString AVAL = "";
 QString ZVAL = "";

 QFile mFile(machineInfo);
 if (mFile.exists()) {
      QTextStream stream( &mFile );
      QString sLine;
      mFile.open( QIODevice::ReadOnly );
      // cout << modelFile << " is open" << endl;
      while ( !stream.atEnd() ) {
           sLine = stream.readLine();
           QString strLine = sLine.toStdString().c_str();
           QString keyWord = strLine.section('=',0,0);
           QString keyValueTmp = strLine.section('#',0,0).section('=',1,1);
           QString keyValue = keyValueTmp.simplified();
           if (keyWord.contains("MAXFW"))  MAXFW = keyValue;
           if (keyWord.contains("MAXFL"))  MAXFL = keyValue;
           if (keyWord.contains("AVAL"))  AVAL = keyValue;
           if (keyWord.contains("ZVAL"))  ZVAL = keyValue;
      }
 }
 mFile.close();

#ifdef XVMC
 float ZC = (afit->ZX + afit->ZY)/2.0;
#endif
 float eE = 0.13 * afit->E + 0.55;
 float nuValue = 0.45;

 bool ok;
 float maxFW = MAXFW.toFloat(&ok)/10.0;
 float maxFL = MAXFL.toFloat(&ok)/10.0;
 float ISO = 100.0;
 float FFRad = sqrt(maxFW*maxFW + maxFL*maxFL)/2.0*afit->ZS/ISO;

 QFile oFile(bdtFile);
 oFile.remove();
 oFile.open( QIODevice::WriteOnly );
 QTextStream oStream( &oFile );
// oStream << "BASE-DATA-FILE-VERSION:  1.3" << endl;
#ifdef XVMC
 oStream << "BASE-DATA-FILE-VERSION:  1.4" << endl;
#else
 oStream << "BASE-DATA-FILE-VERSION:  1.5" << endl;
#endif
 oStream << "BASE-DATA-ENTRY" << endl;
 oStream << "   PARTICLE-TYPE:     " << pType << endl;
 oStream << "   NOMINAL-ENERGY:    "
         << QString("%1").arg(afit->E,0,'f',2) << endl;
#ifdef XVMC
 oStream << "   BEAM-MODEL-ID:     " << "-1"<< endl;
#else
 oStream << "   BEAM-MODEL-ID:     " << "1"<< endl;
#endif
 oStream << "   BEGIN-PARAMETERS" << endl;
 oStream << "      PRIMARY-PHOTONS:    "
         << QString("%1").arg(afit->p0,0,'f',5) << endl;
#ifdef XVMC
 oStream << "      PRIMARY-DIST:       "
         << QString("%1").arg(afit->Z0,0,'f',3) << endl;
#endif
 oStream << "      PRIMARY-SIGMA:      "
         << QString("%1").arg(afit->s0,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN0:      "
         << QString("%1").arg(afit->h0,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN1:      "
         << QString("%1").arg(afit->h1,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN2:      "
         << QString("%1").arg(afit->h2,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN3:      "
         << QString("%1").arg(afit->h3,0,'f',5) << endl;
 oStream << "      PRIMARY-HORN4:      "
         << QString("%1").arg(afit->h4,0,'f',5) << endl;
 oStream << "      SCATTER-DIST:       "
         << QString("%1").arg(afit->ZS,0,'f',3) << endl;
 oStream << "      SCATTER-SIGMA:      "
         << QString("%1").arg(afit->ss,0,'f',5) << endl;
#ifdef XVMC
 oStream << "      COLM-DIST:          "
         << QString("%1").arg(afit->ZM,0,'f',3) << endl;
 oStream << "      COLC-DIST:          "
         << QString("%1").arg(ZC,0,'f',3) << endl;
 oStream << "      COLX-DIST:          "
         << QString("%1").arg(afit->ZX,0,'f',3) << endl;
 oStream << "      COLY-DIST:          "
         << QString("%1").arg(afit->ZY,0,'f',3) << endl;
#endif
 oStream << "      NORM-VALUE:         "  << "1.000" << endl;
 oStream << "      GY/MU-DMAX:         "  << "0.010" << endl;
 oStream << "      ENERGY-MIN:         "  << "0.250" << endl;
 oStream << "      ENERGY-MAX:         "  << afit->E << endl;
 oStream << "      L-VALUE:            "  << "0.000" << endl;
 oStream << "      B-VALUE:            "  << "1.000" << endl;
 oStream << "      A-VALUE:            "  << AVAL << endl;
 oStream << "      Z-VALUE:            "  << ZVAL << endl;
 oStream << "      NU-VALUE:            0.45" << endl;
 //        << QString("%1").arg(nuValue,0,'f',2) << endl;
 oStream << "      CHARGED-PARTICLES:  "
         << QString("%1").arg(pCon,0,'f',3) << endl;
 oStream << "      CHARGED-DIST:       "
         << QString("%1").arg(afit->ZS,0,'f',3) << endl;
 oStream << "      CHARGED-RADIUS:     "
         << QString("%1").arg(FFRad,0,'f',3) << endl;
 oStream << "      CHARGED-E-MEAN:     "
         << QString("%1").arg(eE,0,'f',3) << endl;
 oStream << "   END-PARAMETERS" << endl;
 oStream << "# Date:" << __DATE__ << " " << __TIME__<< endl;

 oFile.close();
}

void AirFit::loadData()
{

  QStringList xStrList;
  QStringList yStrList;
  QStringList zStrList;

  QString AFIT_DIR = getenv("PWD");
  QFile mFile(AFIT_DIR+"/afit.lst");
  if (mFile.exists()) {
     QTextStream stream( &mFile );
     QString sLine;
     mFile.open(QIODevice::ReadOnly );
     while ( !stream.atEnd() ) {
       sLine = stream.readLine();
       QString strLine = sLine;
       // QTextStream(stdout) << "loadDat: " << strLine << endl;
       QString strX = "\tX\t";
       if (strLine.contains("\tx\t") && strLine.contains("\t1\t") ) {
          xStrList << strLine.section('\t',0,0).section('/',1,1);
       }
       if (strLine.contains("\ty\t") && strLine.contains("\t1\t") ) {
          yStrList << strLine.section('\t',0,0).section('/',1,1);
       }
       if (strLine.contains("\tz\t") && strLine.contains("\t1\t") ) {
          zStrList << strLine.section('\t',0,0).section('/',1,1);
       }
       if (strLine.contains("\tX\t") && strLine.contains("\t1\t") ) {
          xStrList << strLine.section('\t',0,0).section('/',1,1);
       }
       if (strLine.contains("\tY\t") && strLine.contains("\t1\t") ) {
          yStrList << strLine.section('\t',0,0).section('/',1,1);
       }
       if (strLine.contains("\tZ\t") && strLine.contains("\t1\t") ) {
          zStrList << strLine.section('\t',0,0).section('/',1,1);
       }
     }
     mFile.close();
  }

  // QTextStream(stdout) << "loadData: xStrList = " << xStrList.join(" ") << endl;
  QString xPrevious("");
  QString xEntry("");
  xStrList.sort();
  QStringList xStrList6d;
  QStringList xStrList7d;
  QStringList xStrList8d;
  // QStringList xStrListFinal;
  for ( QStringList::Iterator it = xStrList.begin(); it != xStrList.end(); ++it ) {
    // cout << *it << endl;
    QString tst(*it);
    xEntry = tst.section('.',0,0);
    if (xEntry != xPrevious) {
      xPrevious = xEntry;
        if (xPrevious.length() == 6) xStrList6d << xEntry;
        if (xPrevious.length() == 7) xStrList7d << xEntry;
        if (xPrevious.length() == 8) xStrList8d << xEntry;
      // xStrListFinal << xEntry;
    }
    // cout << tst.section('.',1,1) << endl;
    }
    // QTextStream(stdout) << "loadData: xStrList6d = " << xStrList6d.join(" ") << endl;
    ui->xListBox->clear();
    //ui->xListBox->insertItems(-1, xStrList6d);
    //ui->xListBox->insertItems(-1, xStrList7d);
    //ui->xListBox->insertItems(-1, xStrList8d);
    ui->xListBox->addItems(xStrList6d);
    ui->xListBox->addItems(xStrList7d);
    ui->xListBox->addItems(xStrList8d);
    // xListBox->insertStringList(xStrListFinal, -1);

  QString yPrevious("");
  QString yEntry("");
  yStrList.sort();
  QStringList yStrList6d;
  QStringList yStrList7d;
  QStringList yStrList8d;
  // QStringList yStrListFinal;
  for ( QStringList::Iterator it = yStrList.begin(); it != yStrList.end(); ++it ) {
    QString tst(*it);
    yEntry = tst.section('.',0,0);
    if (yEntry != yPrevious) {
      yPrevious = yEntry;
        if (yPrevious.length() == 6) yStrList6d << yEntry;
        if (yPrevious.length() == 7) yStrList7d << yEntry;
        if (yPrevious.length() == 8) yStrList8d << yEntry;
      // yStrListFinal << yEntry;
    }
    }
    ui->yListBox->clear();
    //ui->yListBox->insertItems(-1, yStrList6d);
    //ui->yListBox->insertItems(-1, yStrList7d);
    //ui->yListBox->insertItems(-1, yStrList8d);
    ui->yListBox->addItems(yStrList6d);
    ui->yListBox->addItems(yStrList7d);
    ui->yListBox->addItems(yStrList8d);
    // yListBox->insertStringList(yStrListFinal, -1);

  QString zPrevious("");
  QString zEntry("");
  zStrList.sort();
  QStringList zStrList6d;
  QStringList zStrList7d;
  QStringList zStrList8d;
  // QStringList zStrListFinal;
  for ( QStringList::Iterator it = zStrList.begin(); it != zStrList.end(); ++it ) {
    QString tst(*it);
    zEntry = tst.section('.',0,0);
    if (zEntry != zPrevious) {
      zPrevious = zEntry;
        if (zPrevious.length() == 6) zStrList6d << zEntry;
        if (zPrevious.length() == 7) zStrList7d << zEntry;
        if (zPrevious.length() == 8) zStrList8d << zEntry;
      // zStrListFinal << zEntry;
    }
    }
    ui->zListBox->clear();
    //ui->zListBox->insertItems(-1, zStrList6d);
    //ui->zListBox->insertItems(-1, zStrList7d);
    //ui->zListBox->insertItems(-1, zStrList8d);
    ui->zListBox->addItems(zStrList6d);
    ui->zListBox->addItems(zStrList7d);
    ui->zListBox->addItems(zStrList8d);
  // zListBox->insertStringList(zStrListFinal, -1);
}


void AirFit::plotAirFit() {

    loadData();

    /*
    if (ui->xListBox->currentRow() > -1)
        ui->xListBox->item(ui->xListBox->currentRow())->setSelected(false);
    if (ui->yListBox->currentRow() > -1)
        ui->yListBox->item(ui->yListBox->currentRow())->setSelected(false);
    if (ui->zListBox->currentRow() > -1)
        ui->zListBox->item(ui->zListBox->currentRow())->setSelected(false);
    */

     // QTextStream (stdout) << "iListBox and iListItem = " << iListBox << " " << iListItem << endl;
     if (ui->zListBox->count() > 0 && iListBox == 3) {
         ui->zListBox->item(iListItem)->setSelected(true);
         ui->zListBox->setCurrentRow(iListItem);

         strcpy(ui->myPlot->fname,ui->zListBox->currentItem()->text().toStdString().c_str());
     }
     else if (ui->xListBox->count() > 0 && iListBox == 1) {
         ui->xListBox->item(iListItem)->setSelected(true);
         ui->xListBox->setCurrentRow(iListItem);

         strcpy(ui->myPlot->fname,ui->xListBox->currentItem()->text().toStdString().c_str());
     }
     else if (ui->yListBox->count() > 0 && iListBox == 2) {
         ui->yListBox->item(iListItem)->setSelected(true);
         ui->yListBox->setCurrentRow(iListItem);

         strcpy(ui->myPlot->fname,ui->yListBox->currentItem()->text().toStdString().c_str());
     }

     ui->myPlot->usrDataFile(ui->myPlot->fname);
     // ui->myPlot->replot();

}


void AirFit::resetAirFitValues(){
  QFile mFile("afit.inp");
  if (mFile.exists()) setAirFitValues("afit.inp");
}


void AirFit::on_resetButton_clicked()
{
    QFile mFile("afit.inp");
    if (mFile.exists()) setAirFitValues("afit.inp");
}

void AirFit::on_doneButton_clicked()
{
    QString pwd = getenv("PWD");
    // cout << "PWD= " << pwd << endl;
    // QTextStream(stdout) << "PWD = " << pwd << endl;
    if (makeDir(pwd.section('/',0,-2) + "/dat")){

#ifdef XVMC
    QString bdtFile = pwd.section('/',0,-2) + "/dat/"
                   + pwd.section('/',-2,-2) + "_GeoModel.bdt";
    writeBDT(bdtFile, "Photons", 0.0);
    bdtFile = pwd.section('/',0,-2) + "/dat/"
                   + pwd.section('/',-2,-2) + "_GeoModelEcon.bdt";
    writeBDT(bdtFile, "Photons", 1.0);
#else
    // April 20, 2006 for new afit
    QString bdtFile = pwd.section('/',0,-2) + "/dat/"
                    + pwd.section('/',-2,-2) + ".bdt";
    writeBDT(bdtFile, "Photons", 0.0);
#endif
    }
    QFile mFile("afit.inp.tmp");
    if (mFile.exists()) {
          QString CMD = "cp afit.inp.tmp afit.inp";
          if (system(CMD.toStdString().c_str()) != 0) {
              cout << "ERROR: Somethings are wrong: " << CMD.toStdString() << endl ;
          }
          writeLocalSetting("AFIT", "Done");
          writeLocalSetting("MonoMC", "NotDone");
          // cout << "Done: AFIT = Done" << endl;
    } else {
          writeLocalSetting("AFIT", "NotDone");
          // cout << "Done: AFIT = NotDone" << endl;
    }
    close(); // close this widget
}

void AirFit::on_interruptButton_clicked()
{
    // QTextStream(stdout) << "*** DEBUG: Cancel Button " << endl;
    // writeLocalSetting("AFIT", "NotDone");
    close();
}

void AirFit::on_editButton_clicked()
{
    QString LBIN = getenv("XVMC_HOME");
    // QString LBIN = getenv("HOME");
    QString CMD = "cp afit.lst afit.lst.tmp;"
                + LBIN + "/inairtable.exe -i afit.lst";
       if (system(CMD.toStdString().c_str()) != 0) {
          cout << "ERROR: Somethings are wrong in system command: "
               << CMD.toStdString() << endl ;
       }

    // QTextStream(stdout) << "*** DEBUG: Edit Button " << CMD << endl;
    if (ui->checkBoxAutoPlot->isChecked()) plotAirFit();
}


void AirFit::on_xListBox_clicked(const QModelIndex &index)
{

    // QTextStream(stdout) << "*** DEBUG: xListBox Clicked " << endl;
    strcpy(ui->myPlot->fname,ui->xListBox->currentItem()->text().toStdString().c_str());
    // QTextStream(stdout) << ui->xListBox->currentItem()->text() << endl;
    ui->myPlot->usrDataFile(ui->myPlot->fname);
    // QTextStream(stdout) << "on_xListBox_clicked: getXPlotData " << ui->myPlot->fname << endl;

    resetZoomer();
    iListBox = 1;
    iListItem = ui->xListBox->currentRow();
    // QTextStream(stdout) << "on_xListBox_clicked: Check Point A " << iListItem << endl;
    // QTextStream(stdout) << "on_xListBox_clicked: Check Point A " << ui->yListBox->currentRow() << endl;
    if (ui->yListBox->currentRow() > -1)
        ui->yListBox->item(ui->yListBox->currentRow())->setSelected(false);
    if (ui->zListBox->currentRow() > -1)
        ui->zListBox->item(ui->zListBox->currentRow())->setSelected(false);
    /*
    // d_plot->replot();
    if (yListBox->currentItem() > -1) {
      QStringList yStrListFinal;
      for (unsigned int j=0; j<yListBox->count(); j++)
       yStrListFinal << yListBox->text(j);
      yListBox->clear();
        yListBox->insertStringList(yStrListFinal, -1);
      }
    if (zListBox->currentItem() > -1) {
      QStringList zStrListFinal;
      for (unsigned int k=0; k<zListBox->count(); k++)
       zStrListFinal << zListBox->text(k);
      zListBox->clear();
        zListBox->insertStringList(zStrListFinal, -1);
      }
       */

    // QTextStream(stdout) << "End of on_xListBox_clicked " << endl;
}

void AirFit::on_yListBox_clicked(const QModelIndex &index)
{
    // QTextStream(stdout) << "*** DEBUG: yListBox Clicked " << endl;
    strcpy(ui->myPlot->fname,ui->yListBox->currentItem()->text().toStdString().c_str());
    // QTextStream(stdout) << ui->yListBox->currentItem()->text() << endl;
    ui->myPlot->usrDataFile(ui->myPlot->fname);
    // QTextStream(stdout) << "on_yListBox_clicked: getYPlotData " << ui->myPlot->fname << endl;

    resetZoomer();
    iListBox = 2;
    iListItem = ui->yListBox->currentRow();
    // QTextStream(stdout) << "on_yListBox_clicked: Check Point A " << iListItem << endl;
    // QTextStream(stdout) << "on_yListBox_clicked: Check Point A " << ui->xListBox->currentRow() << endl;
    if (ui->xListBox->currentRow() > -1)
        ui->xListBox->item(ui->xListBox->currentRow())->setSelected(false);
    if (ui->zListBox->currentRow() > -1)
        ui->zListBox->item(ui->zListBox->currentRow())->setSelected(false);

 /*
 // d_plot->replot();
 if (xListBox->currentItem() > -1) {
   QStringList xStrListFinal;
   for (unsigned int i=0; i<xListBox->count(); i++)
    xStrListFinal << xListBox->text(i);
   xListBox->clear();
     xListBox->insertStringList(xStrListFinal, -1);
   }
 if (zListBox->currentItem() > -1) {
   QStringList zStrListFinal;
   for (unsigned int k=0; k<zListBox->count(); k++)
    zStrListFinal << zListBox->text(k);
   zListBox->clear();
     zListBox->insertStringList(zStrListFinal, -1);
   }
    */
}


void AirFit::on_zListBox_clicked(const QModelIndex &index)
{
    strcpy(ui->myPlot->fname,ui->zListBox->currentItem()->text().toStdString().c_str());
      // cout << zListBox->currentText() << endl;
    ui->myPlot->usrDataFile(ui->myPlot->fname);

     resetZoomer();

      iListBox = 3;
      iListItem = ui->zListBox->currentRow();

      if (ui->xListBox->currentRow() > -1)
            ui->xListBox->item(ui->xListBox->currentRow())->setSelected(false);
      if (ui->yListBox->currentRow() > -1)
            ui->yListBox->item(ui->yListBox->currentRow())->setSelected(false);

    /*
    // d_plot->replot();
    if (xListBox->currentItem() > -1) {
      QStringList xStrListFinal;
      for (unsigned int i=0; i<xListBox->count(); i++)
       xStrListFinal << xListBox->text(i);
      xListBox->clear();
        xListBox->insertStringList(xStrListFinal, -1);
      }
    if (yListBox->currentItem() > -1) {
      QStringList yStrListFinal;
      for (unsigned int j=0; j<yListBox->count(); j++)
       yStrListFinal << yListBox->text(j);
      yListBox->clear();
        yListBox->insertStringList(yStrListFinal, -1);
      }
       */
}

void AirFit::on_plotButton_clicked()
{
    plotAirFit();
}


void AirFit::on_pushButtonZ0_clicked()
{
    if (ui->pushButtonZ0->isChecked()) {
       // ui->pushButtonZ0->setChecked(false);
        ui->lineEditZ0->setEnabled(false);
    }
    else
    {
       // ui->pushButtonZ0->setChecked(true);
        ui->lineEditZ0->setEnabled(true);
    }

}

void AirFit::on_pushButtonZS_clicked()
{
    if (ui->pushButtonZS->isChecked()) {
        ui->lineEditZS->setEnabled(false);
    }
    else
    {
        ui->lineEditZS->setEnabled(true);
    }
}

void AirFit::on_pushButtonZM_clicked()
{
    if (ui->pushButtonZM->isChecked()) {
        ui->lineEditZM->setEnabled(false);
    }
    else
    {
        ui->lineEditZM->setEnabled(true);
    }
}

void AirFit::on_pushButtonZX_clicked()
{
    if (ui->pushButtonZX->isChecked()) {
        ui->lineEditZX->setEnabled(false);
    }
    else
    {
        ui->lineEditZX->setEnabled(true);
    }
}

void AirFit::on_pushButtonZY_clicked()
{
    if (ui->pushButtonZY->isChecked()) {
        ui->lineEditZY->setEnabled(false);
    }
    else
    {
        ui->lineEditZY->setEnabled(true);
    }
}

void AirFit::on_pushButtonZN_clicked()
{
    if (ui->pushButtonZN->isChecked()) {
        ui->lineEditZN->setEnabled(false);
    }
    else
    {
        ui->lineEditZN->setEnabled(true);
    }
}

void AirFit::on_radioButtonLegend_toggled(bool checked)
{
    if (checked)
    {
        ui->myPlot->LegendSwitch="Show";
    }
    else
    {
        ui->myPlot->LegendSwitch="Hide";
    }
    ui->myPlot->showLegend();
}


void AirFit::on_radioButtonDD_toggled(bool checked)
{
    if (checked)
    {
         // ui->radioButtonDD->setChecked(true);
         ui->myPlot->DDSwitch="Show";
    }
    else
    {
         ui->myPlot->DDSwitch="Hide";
    }
    ui->myPlot->showDD();
}

void AirFit::on_pushButtonCheckPS_clicked()
{
    toggleCheckBoxLeft();
}

void AirFit::on_fitButton_clicked()
{

    runAirFitNew();

    updateAirFitValues();

    plotAirFit();

/*
    // Old Script-based runAirFit
    writeAirFit();
    char *CMD = "runAirFit.csh";
    if (system(CMD) != 0) {
        cout << "ERROR: Somethings are wrong: " << CMD << endl ;
    }
*/
}
// ----------------------------------------------------------------

void AirFit::on_pushButtonCheckH_clicked()
{
    toggleCheckBoxRight();
}

void AirFit::on_pushButtonCheckZero_clicked()
{
    setAllHZero();
}
