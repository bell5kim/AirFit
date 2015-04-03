#include "AirFitInput.h"

// Initiation of AirFit Parameters
void AirFitInput::initAirFitInput(){
 
     E = 0.0;
     ZI = 0.0;                   // iso-center position
     Z0 = 0.0;                   // target (primary source) position
     ZS = 0.0;                   // filter (scatter source) position
     ZM = 0.0;                   // modifier (MLC) position
     MX = 0;                     // collimate x twice: TRUE or FALSE
     MY = 0;                     // collimate y twice: TRUE or FALSE
     ZX = 0.0;                   // x collimator position
     ZY = 0.0;                   // y collimator position

     // normalization position and field size
     XN = 0.0;
     YN = 0.0;
     ZN = 0.0;
     WXN = 0.0;
     WYN = 0.0;

     // number of measured air profiles (number of entries in list file)
     NC = 0;

     // Non-WFIT stuff but good for UI
     DEV = 0.0;
     CHI = 0.0;
	
     // parameters to fit
     p0 =  0.85;  ip0 = 0;  // probabilty for primary photons
     s0 =  0.15;  is0 = 0;  // primary width
     h0 =  0.0;  ih0 = 0;  // horn correction parameter 0
     h1 =  0.0;  ih1 = 0;  // horn correction parameter 1
     h2 =  0.0;  ih2 = 0;  // horn correction parameter 2
     h3 =  0.0;  ih3 = 0;  // horn correction parameter 3
     h4 =  0.0;  ih4 = 0;  // horn correction parameter 4
     ss =  1.5;  iss = 0;  // scatter width

     version_ok = false;
     energy_ok  = false;
     p0_ok      = false;
     s0_ok      = false;
     h0_ok      = false;
     h1_ok      = false;
     h2_ok      = false;
     h3_ok      = false;
     h4_ok      = false;
     ss_ok      = false;
     Z0_ok      = false;
     ZS_ok      = false;
     ZM_ok      = false;
     ZX_ok      = false;
     ZY_ok      = false;
     ZI_ok      = false;
     XN_ok      = false;
     YN_ok      = false;
     ZN_ok      = false;
     WXN_ok     = false;
     WYN_ok     = false;
     list_ok    = false;
}

// error handler
void AirFitInput::err_exit(char *location, char *reason, int exit_code)
{
   cerr << endl;
   cerr << "RUN-TIME ERROR in " << location << endl;
   cerr << "Reason: " << reason << "!" << endl;
   cerr << "Exiting to system..." << endl;
   cerr << endl;
   exit(exit_code);
}

// Read Input File
void AirFitInput::readAirFitInput(char *fname)
{
   // string of input filename
   // ifile[0] = 0;
   // string to check file version
   //version_string[0] = 0;
   // nominal energy string (no energy by default)
   //energy_string[0] = 0;
   // name of the list file
   //list_string[0] = 0;
   // cout <<  endl;

   // read input file
   // cout << "Opening file: " << argv[1] << endl;
   
   ifstream inp_file;
   inp_file.open(fname,ios::in);
   if (inp_file.bad()) err_exit("getAitFitInput","cannot open input file",8);

   while (!inp_file.eof())
   {
          char line[81] = "";  // lines to read from file
          inp_file.getline(line,sizeof(line));
          istringstream line_stream(line);
          char keyword[20] = "";
          line_stream >> keyword;
          if (!strcmp(keyword,"version:")) { line_stream >> version_string;
                                                            version_ok = true; }
          if (!strcmp(keyword,"energy:"))  { line_stream >> E;
                                                            energy_ok  = true; }
          if (!strcmp(keyword,"p0:"))   { line_stream >> p0 >> ip0; p0_ok = true; }
          if (!strcmp(keyword,"s0:"))   { line_stream >> s0 >> is0; s0_ok = true; }
          if (!strcmp(keyword,"h0:"))   { line_stream >> h0 >> ih0; h0_ok = true; }
          if (!strcmp(keyword,"h1:"))   { line_stream >> h1 >> ih1; h1_ok = true; }
          if (!strcmp(keyword,"h2:"))   { line_stream >> h2 >> ih2; h2_ok = true; }
          if (!strcmp(keyword,"h3:"))   { line_stream >> h3 >> ih3; h3_ok = true; }
          if (!strcmp(keyword,"h4:"))   { line_stream >> h4 >> ih4; h4_ok = true; }
          if (!strcmp(keyword,"ss:"))   { line_stream >> ss >> iss; ss_ok = true; }
          if (!strcmp(keyword,"Z0:"))   { line_stream >>  Z0;  Z0_ok = true; }
          if (!strcmp(keyword,"ZS:"))   { line_stream >>  ZS;  ZS_ok = true; }
          if (!strcmp(keyword,"ZM:"))   { line_stream >>  ZM >> MX >> MY;
                                                               ZM_ok = true; }
          if (!strcmp(keyword,"ZX:"))   { line_stream >>  ZX;  ZX_ok = true; }
          if (!strcmp(keyword,"ZY:"))   { line_stream >>  ZY;  ZY_ok = true; }
          if (!strcmp(keyword,"ZI:"))   { line_stream >>  ZI;  ZI_ok = true; }
          if (!strcmp(keyword,"XN:"))   { line_stream >>  XN;  XN_ok = true; }
          if (!strcmp(keyword,"YN:"))   { line_stream >>  YN;  YN_ok = true; }
          if (!strcmp(keyword,"ZN:"))   { line_stream >>  ZN;  ZN_ok = true; }
          if (!strcmp(keyword,"WXN:"))  { line_stream >> WXN; WXN_ok = true; }
          if (!strcmp(keyword,"WYN:"))  { line_stream >> WYN; WYN_ok = true; }
          if (!strcmp(keyword,"DEV:")) { line_stream >>  DEV;}
          if (!strcmp(keyword,"CHI:")) { line_stream >>  CHI;}
          if (!strcmp(keyword,"list:")) { line_stream >> list_string >> NC;
                                                         list_ok = true; }
   }
   inp_file.close();

   if (!version_ok) err_exit("getAitFitInput","no version input",8);
   if (!energy_ok)  err_exit("getAitFitInput","no energy input",8);
   if (!p0_ok)      err_exit("getAitFitInput","no p0 input",8);
   if (!s0_ok)      err_exit("getAitFitInput","no s0 input",8);
   if (!h0_ok)      err_exit("getAitFitInput","no h0 input",8);
   if (!h1_ok)      err_exit("getAitFitInput","no h1 input",8);
   if (!h2_ok)      err_exit("getAitFitInput","no h2 input",8);
   if (!h3_ok)      err_exit("getAitFitInput","no h3 input",8);
   if (!h4_ok)      err_exit("getAitFitInput","no h4 input",8);
   if (!ss_ok)      err_exit("getAitFitInput","no ss input",8);
   if (!Z0_ok)      err_exit("getAitFitInput","no Z0 input",8);
   if (!ZS_ok)      err_exit("getAitFitInput","no ZS input",8);
   if (!ZM_ok)      err_exit("getAitFitInput","no ZM input",8);
   if (!ZX_ok)      err_exit("getAitFitInput","no ZX input",8);
   if (!ZY_ok)      err_exit("getAitFitInput","no ZY input",8);
   if (!ZI_ok)      err_exit("getAitFitInput","no ZI input",8);
   if (!XN_ok)      err_exit("getAitFitInput","no XN input",8);
   if (!YN_ok)      err_exit("getAitFitInput","no YN input",8);
   if (!ZN_ok)      err_exit("getAitFitInput","no ZN input",8);
   if (!WXN_ok)     err_exit("getAitFitInput","no WXN input",8);
   if (!WYN_ok)     err_exit("getAitFitInput","no WYN input",8);
   if (!list_ok)    err_exit("getAitFitInput","no list input",8);
 
}

// Print Input File
void AirFitInput::printAirFitInput() {
     cout << "version:   " << version_string << endl;
     cout << "energy:   " << E  << endl;
     cout << "p0:   " << p0 << "   " << ip0 << endl;
     cout << "s0:   " << s0 << "   " << is0 << endl;
     cout << "h0:   " << h0 << "   " << ih0 << endl;
     cout << "h1:   " << h1 << "   " << ih1 << endl;
     cout << "h2:   " << h2 << "   " << ih2 << endl;
     cout << "h3:   " << h3 << "   " << ih3 << endl;
     cout << "h4:   " << h4 << "   " << ih4 << endl;
     cout << "ss:   " << ss << "   " << iss << endl;
     cout << "Z0:   " << Z0 << endl;
     cout << "ZS:   " << ZS << endl;
     cout << "ZM:   " << ZM << "   " << MX << "   " << MY << endl;
     cout << "ZX:   " << ZX << endl;
     cout << "ZY:   " << ZY << endl;
     cout << "ZI:   " << ZI << endl;
     cout << "XN:   " << XN << endl;
     cout << "YN:   " << YN << endl;
     cout << "YN:   " << ZN << endl;
     cout << "WXN:   " << WXN << endl;
     cout << "WYN:   " << WYN << endl;
     cout << "list:   " << list_string << "  " << NC << endl;
     cout << "DEV:   " << DEV << endl;
     cout << "CHI:   " << CHI << endl;
     cout << "#Date: " << __DATE__ << __TIME__ << CHI << endl;
 }

// Write Input File
void AirFitInput::writeAirFitInput() {
     ofstream out_file("afit.inp.tmp",ios::out);
     if (out_file.bad()){
        err_exit("writeAirFitInput","cannot open file",8);
 } 
 
 out_file << "version:   " << version_string << endl;
 out_file << "energy:   " << E  << endl;
 out_file << "p0:   " << p0 << "   " << ip0 << endl;
 out_file << "s0:   " << s0 << "   " << is0 << endl;
 out_file << "h0:   " << h0 << "   " << ih0 << endl;
 out_file << "h1:   " << h1 << "   " << ih1 << endl;
 out_file << "h2:   " << h2 << "   " << ih2 << endl;
 out_file << "h3:   " << h3 << "   " << ih3 << endl;
 out_file << "h4:   " << h4 << "   " << ih4 << endl;
 out_file << "ss:   " << ss << "   " << iss << endl;
 out_file << "Z0:   " << Z0 << endl;
 out_file << "ZS:   " << ZS << endl;
 out_file << "ZM:   " << ZM << "   " << MX << "   " << MY << endl;
 out_file << "ZX:   " << ZX << endl;
 out_file << "ZY:   " << ZY << endl;
 out_file << "ZI:   " << ZI << endl;
 out_file << "XN:   " << XN << endl;
 out_file << "YN:   " << YN << endl;
 out_file << "ZN:   " << ZN << endl;
 out_file << "WXN:   " << WXN << endl;
 out_file << "WYN:   " << WYN << endl;
 out_file << "list:   " << list_string << "  " << NC << endl;
 out_file << "DEV:   " << DEV << endl;
 out_file << "CHI:   " << CHI << endl;
 out_file << "# Date: " << __DATE__ << " " << __TIME__ << endl;
 
 out_file.close();
}
