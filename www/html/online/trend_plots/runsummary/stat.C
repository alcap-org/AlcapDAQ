
#include<fstream>


#include "../common/mysql.h"
#include "../common/init.C"

void stat()
{

  TSQLResult *res;
  TSQLRow    *row;

  TSQLServer *db = TSQLServer::Connect(MYSQL_SERVER,MYSQL_USER,MYSQL_PASSWD);

  if ( !db ) return;
  
  // *** open HTML file for output ***
  
  ofstream *os = new ofstream ("../html/stat.html");
  
  
  // ==================================================================
  //                    *** Tape 1 *** 
  // ==================================================================

  res = db->Query("select count(run), "
		  "sum(size) "
		  "from Runlog "
		  "where  "
		  "tape1 is not null  "
		  "and tape1!='' "
		  );
  
  row = res->Next();
  
  /*** number of runs ***/
  int tape1_nruns = atoi(row->GetField(0));
  /*** total data size collected ***/
  Double_t tape1_size = atof(row->GetField(1));
   
  *os << "<h3>Tape 1</h3>\n"
      << "total runs written: "    << tape1_nruns     << "<br>\n"
      << "total size written: "    << tape1_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
      << "number of tapes estimate: (total size written)/400 = "    << int(tape1_size/(1024.*1024.*1024.*400.))      << " tapes<br>\n"
      << endl;
  


  delete row;
  delete res;



  // ==================================================================
  //                    *** Tape 2 *** 
  // ==================================================================

  res = db->Query("select count(run), "
		  "sum(size) "
		  "from Runlog "
		  "where  "
		  "tape2 is not null "
		  "and tape2!='FTP' "
		  "and tape2!='' "
		  );
  
  row = res->Next();
  
  /*** number of runs ***/
  int tape2_nruns = atoi(row->GetField(0));
  /*** total data size collected ***/
  Double_t tape2_size = atof(row->GetField(1));
   
  *os << "<h3>Tape 2</h3>\n"
      << "total runs written: "    << tape2_nruns     << "<br>\n"
      << "total size written: "    << tape2_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
      << "number of tapes estimate: (total size written)/400 = "    << int(tape2_size/(1024.*1024.*1024.*400.))      << " tapes<br>\n"
      << endl;
  


  delete row;
  delete res;


  // ==================================================================
  //                    *** FTP *** 
  // ==================================================================

  res = db->Query("select count(run), "
		  "sum(size) "
		  "from Runlog "
		  "where  "
		  "tape2 is not null  "
		  "and tape2='FTP' "
		  ); 
  
  row = res->Next();
  
  /*** number of runs ***/
  int ftp_nruns = atoi(row->GetField(0));
  /*** total data size collected ***/
  Double_t ftp_size = atof(row->GetField(1));
   
  *os << "<h3>PSI archive</h3>\n"
      << "total runs written: "    << ftp_nruns     << "<br>\n"
      << "total size written: "    << ftp_size/(1024.*1024.*1024.*1024.)      << " Tb<br>\n"
      << endl;
  


  delete row;
  delete res;





  delete db;
  delete os;

  /***

  int nrows = res->GetRowCount();
  int nfields = res->GetFieldCount();
  
  struct s_query {
    int   nruns;
    float n_muons;
    float hours;
    int   day;
  };

  s_query *query = new s_query[nrows];  
  
  for( int i=0; i<nrows; i++ ) {

    TSQLRow *row = res->Next();
    
    query[i].nruns   = atoi(row->GetField(0));
    query[i].n_muons = atof(row->GetField(1));
    query[i].hours   = atof(row->GetField(2))/3600.0;
    query[i].day     = atoi(row->GetField(3));

    printf("i=%d day: %d nruns: %d, n_muons: %f, n_hours: %f\n",
	   i,
	   query[i].day,
	   query[i].nruns,
	   query[i].n_muons,
	   query[i].hours);

    delete row;
  }
  double day0 = query[0].day;
  double day1 = query[nrows-1].day;
  int nday = int(day1-day0+1.1);

  */


  
}
