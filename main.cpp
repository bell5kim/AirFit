#include "airfit.h"
#include <QApplication>
#include <cstdio>

void usage() {
  printf("\n Usage: %s -i afit.inp\n", "airfit.exe");
  printf("\n");
}

int main(int argc, char *argv[])
{

    //setenv("XVMC_HOME", "/home/jokim/Work/MonacoBCT/bin",1);
    //setenv("XVMC_WORK", "/home/jokim/Work/MonacoBCT/Va06xBLV1",1);

    if (argc != 3) {
       usage();
       return(0);
    }
    char *fName = "";


    for(int iArg=0; iArg < argc; iArg++) {
        if(iArg < argc-1) {
            if( strcmp(argv[iArg],"-i") == 0 || strcmp(argv[iArg],"-fname") == 0) {
                    iArg++;
                    fName = argv[iArg];
            }
            if(strcmp("-help", argv[iArg]) == 0 || strcmp("-h", argv[iArg]) == 0 ) {
                usage();
                return(0);
            }
        }
    }


    QApplication a(argc, argv);

    AirFit w;
    // w.resize( 540, 400 );
    w.show();

    return a.exec();
}
