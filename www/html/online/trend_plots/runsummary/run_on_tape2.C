#include <fstream>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>

//#include "TPaletteAxis.h"
//#include "TMath.h"
#include "../../common/mysql.h"
#include "../../common/init.C"
//#include "../../common/gethist.C"
//#include "/home/mulan/DAQ/analyzer/modules/ZipCodeManipulation.cpp"
//#include "../../common/ZipCodeManipulation.cpp"
//#include "../../common/bank2zip.C"


int Max(const int *Numbers, const int Count)
{
  int Maximum = Numbers[0];
  for(int i = 0; i < Count; i++)
    if( Maximum < Numbers[i] )
      Maximum = Numbers[i];

  return Maximum;
}


void run_on_tape2()
{

  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) return;

  ofstream *os = new ofstream ("../../html/run_on_tape2.html");

  TSQLResult* res = db->Query("select tape2, "
                              "count(run), "
                              "sum(size) "
                              "from Runlog "
                              "where "
             		      "tape2 is not null "
                              "and tape2!='' "
                              "and tape2!='FTP' "
                              "group by "
                              "tape2 "
                              );

  int ntapes = res->GetRowCount();
  cout << "ntapes: " << ntapes << endl;

  int *truns = new int[ntapes];
  string *tape = new string[ntapes];
  Double_t *tsize = new Double_t[ntapes];

  for (int i=0; i<ntapes; i++ ){

    TSQLRow *row = res->Next();
 
    tape[i]  = row->GetField(0);
    truns[i] = atoi(row->GetField(1));
    tsize[i] = atof(row->GetField(2));
    cout << tape[i] << " | " << truns[i] << endl; 
 
    delete row;
  }

    res = db->Query("select tape2, "
                    "run "
                    "from Runlog "
                    "where "
	            "tape2 is not null "
                    "and tape2!='' "
                    "and tape2!='FTP' "
                    "order by "
                    "tape2 "
                    );

  int nrows = res->GetRowCount();
  cout << "nrows: " << nrows << endl;

  //int ttotal = sizeof(truns)/sizeof(int);
  int max_truns = Max(truns, ntapes);
  cout << "max: " << max_truns << endl;

  int **run = new int*[max_truns];

  for (int i=0; i<max_truns; i++)
    run[i]= new int[ntapes];

  for (int j=0; j<ntapes; j++){
    for (int i=0; i<max_truns; i++ ) 
      run[i][j]=0;
  }
  

  for (int j=0; j<ntapes; j++){

    for (int i=0; i<truns[j]; i++ ){ 

      TSQLRow *row = res->Next();
 
      run[i][j] = atoi(row->GetField(1));
      cout << "run[" << i << "][" << j << "]: " << run[i][j] << endl;
      delete row;
    }
  
  }

  
  delete res;
  db->Close();

  int *ID = new int[max_truns];

  for (int i=0; i<max_truns; i++)
    ID[i] = i;

   
  *os << "<hr><br>\n"
      << "<h2>Runs written to Tape2</h2>\n"
      << "<table border=\"1\" align=\"center\" valign=\"middle\" rules=\"all\" frame=\"border\" bgcolor=\"#FFFFA0\">\n"
      << "<tr bgcolor=\"#A0FFA0\">\n"
      << "  <td align=\"center\"><b> ID </b></td>\n";
  for (int i=0; i<ntapes; i++)
    *os << "  <td align=\"center\"><b> " << tape[i] << " </b></td>\n";
    *os << "  </tr>\n";
  

  for (int i=0; i<max_truns; i++ ){
    *os << "<tr>\n"
        << "  <td bgcolor=\"#82CAFA\" align=\"center\"><b> " << ID[i]  << " </b></td>\n";
    for (int j=0; j<ntapes; j++){
      if (run[i][j] != 0){
        *os << "  <td align=\"center\"><b>\n" 
            <<    run[i][j]  << "<br>\n"
            << "  <font color=\"red\" size=1> " << tape[j].substr(0,3) << "</font><font color=\"green\" size=1>" << tape[j].substr(3,2) << "</font><font color=\"blue\" size=1>R" << ID[i] << "</font></b></td>\n";
      }
      else
        *os << "  <td>&nbsp;</td>\n";
    }
  *os << "</tr>\n";
  }    
  *os << "<tr bgcolor=\"#9E7BFF\">\n"
      << "<td align=\"center\"><b>GB</b></td>\n";
  for (int i=0; i<ntapes; i++)
    *os << "  <td align=\"center\"><b> " << tsize[i]/(1024.*1024.*1024.) << " </b></td>\n";
    *os << "  </tr>\n";

   for (int i=0; i<max_truns; i++)
     delete run[i];

   delete run;
   
   delete os;
   delete ID;
   delete tape;
   delete truns;
   
  


}
