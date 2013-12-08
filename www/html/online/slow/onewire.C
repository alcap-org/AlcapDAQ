
// Blame this one on Kevin Lynch
// July 16, 2006
// I should write a data file specification, because this and the
// corresponding slow control front end are set up to dynamically
// generate their format depending on the number of ADC and
// temperature devices attached ... this is asking for disaster if
// you're not careful :-)

#include <fstream>
#include <time.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>


//#include <boost/shared_ptr.hpp>
//#include <boost/tokenizer.hpp>

#include "../common/init.C"

#include "TGraph.h"
#include "TCanvas.h"
#include "TVirtualPad.h"

//#define DEBUG
//#define CHECK_INPUT
// earliest parseable time without serial numbers in header
//const time_t earliest_possible = 1153008000; // July 14, 2006,
					     // 00:00:00 gmt

// earliest parseable time with serial numbers in header
const time_t earliest_possible = 1153905206; // July 26, 2006 
                                             // 09:13:26 gmt

const time_t oneday = 24*60*60;
std::string fename("onewire");
//std::string directory("../slowdata/");
std::string directory("/mnt/tb06raid/slow/");
const char *const datetimeformat = "%Y%m%d%H%M%S";
const char *const dateformat = "%Y%m%d";

int current_adcs=0, current_temps=0;

// protos
void onewire(const int lookback);
void onewire(const time_t first_gmt, const time_t last_gmt);

struct adc_point { float vals[4]; };
namespace ow {
  std::string get_filename(const time_t date);
  std::string gmt_date(const time_t date);
  std::string gmt_datetime(const time_t date);
  int hour_from_time(const time_t time);
  int minute_from_time(const time_t time);
  int second_from_time(const time_t time);
  int day_from_time(const time_t time);
  int month_from_time(const time_t time);
  //int strip_seconds(const time_t time);
  std::string local_date(const time_t date);
  std::string local_datetime(const time_t date);
  time_t time_from_string(const std::string& time);
  struct adc_vals{
    float adc[4];
  };
  void Tokenize(const std::string& str,
		std::vector<std::string>& tokens,
		const std::string& delimiters);
  double stof(const std::string& s);
  int stoi(const std::string& s, int base);
}




  
Double_t ymin, ymax, tmin, tmax;


#define DRAWFRAME(title,ytitle) {\
  pad->cd();\
  pad->SetBottomMargin(0.1);\
  frame = pad->DrawFrame(tmin,ymin,tmax,ymax,title);\
  frame->GetXaxis()->SetTimeDisplay(1);\
  frame->GetXaxis()->SetTimeFormat("%H:%M");\
  frame->GetXaxis()->SetTimeOffset(time_offset);\
  frame->GetXaxis()->SetLabelSize(0.045);\
  frame->GetXaxis()->ChooseTimeFormat(0.1);\
  frame->GetXaxis()->SetTitleOffset(1.0);\
  frame->GetYaxis()->SetTitleOffset(1.1);\
  frame->SetXTitle("time");\
  frame->SetYTitle(ytitle);\
  frame->GetXaxis()->SetTitleSize(0.055);\
  frame->GetXaxis()->SetLabelSize(0.03);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
  frame->GetYaxis()->SetTitleSize(0.055);\
  frame->GetYaxis()->SetLabelSize(0.05);\
  frame->GetYaxis()->SetTitleOffset(0.65);\
}


// definitions
void onewire(const int lookback){ // lookback is in hours
 
  // First, figure out the time calculations
  time_t now;
  time(&now);
  
 
  time_t earliest = now - lookback*60*60;
  if( lookback == 0 )
    earliest = earliest_possible;
  // minor optimization
  earliest = std::max(earliest, earliest_possible);

   std::cout << earliest_possible << ' ' << earliest << ' ' << now << '\n';

  onewire(earliest, now);

}

void onewire(const time_t first_gmt, const time_t last_gmt){
  
  using std::string;
  using std::cout;
  typedef std::string::size_type size_type;

  int adccount = 0;
  int tempcount = 0;
 


  cout << "1-wire monitor online plot generator\n";
  if( first_gmt >= last_gmt ){
    cout << "Serious Error: first time must be before the last time.\n"; 
    return;
  }
  
#ifdef DEBUG
  cout << "Test gmt_date " << ow::gmt_date(last_gmt) << '\n';
  cout << "Test gmt_datetime " << ow::gmt_datetime(last_gmt) << '\n';
  cout << "Test local_date " << ow::local_date(last_gmt) << '\n';
  cout << "Test local_datetime " << ow::local_datetime(last_gmt) << '\n';
  cout << "Test get_filename " << ow::get_filename(last_gmt) << '\n';
#endif
 
  //long int funnytime =  ow::gmt_datetime(first_gmt)>>4;

  cout << "Generating plot between UTC times " 
	    << ow::gmt_datetime(first_gmt) 
	    << " and " 
	    << ow::gmt_datetime(last_gmt)
            << " or "
       << first_gmt << " and " << last_gmt
       << 
       '\n';

 
  
 

  int lookback = (last_gmt - first_gmt);
  int averagerange = lookback/3600;
  if(averagerange == 0)
    averagerange = 1;
  int nBins = 60;
  
  cout << "lookback is " << lookback << " range for averaging is "  << averagerange << "\n";
    

  typedef std::vector<string> filelist;
  filelist files;
  std::vector<float> nullvect;
  nullvect.clear();
  //std::vector<std::string> adc_serials, temp_serials;
  std::vector<std::string> order;
  std::vector<std::string> tokens;
  std::vector<std::vector <float> > data;
  std::vector<time_t> timedata;
  
  //datavect myvect;
  float hold;
  
     // std::map<std::string, std::vector<adc_point> > adc_points;
     //  std::map<std::string, std::vector<float> > temp_points;
     //std::map<std::string, std::vector<float> > data;
  
  // Build a list of files to access, and open them for reading
  time_t temp = first_gmt;
  time_t strip = temp % (60*60*24); // strip off hours, minutes, and seconds
  temp -= strip;
  
  


  while( temp < last_gmt ){
    files.push_back( ow::get_filename(temp) );
    temp+=oneday;
  }

  cout << "I will access the following files:\n";
  for(std::size_t i=0; i!=files.size(); ++i)
    cout << '\t' << files[i] << '\n';
  
  // Process the files one at a time ... I'd prefer to open the files
  // first and store them in a vector (if such can be done ... is
  // fstream Copyable?) ... in any case that crashes Root.  I could
  // store references, but then I'd need a reference wrapper, and I'm
  // just not that interested in the effort that would require...
  for(std::size_t i=0; i!=files.size(); ++i){
    std::ifstream f(files[i].c_str());
    if(!f) {
      cout << "Couldn't open file " << files[i] << "; continuing ...\n";
      continue;
    } else 
      cout << "Opened file " << files[i] << "; continuing ...\n";
     
    string line;
    while(std::getline(f,line)){
      tokens.clear();
      
      // strip leading white space
      size_type first = line.find_first_not_of(" \t");
      line = line.substr(first);
   
      // check for comments and parse if header
      size_type last = line.find_first_of("#");
      if(last!=string::npos){ // found a comment character
	if( line[0] == '#' ) { // part of header?
	  	  cout << "Line!! " << line << '\n';
	  if( line.find("ADC:")!=string::npos ){
	    //cout << "Found ADCS: " << line << '\n';
	    ow::Tokenize(line, tokens, " \t");
	    if( tokens.size() == 3 ){
	      current_adcs = ow::stoi(tokens[2], 16);
	      if(current_adcs > adccount)
		adccount = current_adcs;
		
		  
	      //cout << "current_adcs changed to: " << current_adcs << '\n';
	    }
	  } else if (line.find("TEMP:")!=string::npos){
	    cout << "Found TEMPS: " << line << '\n';
	    ow::Tokenize(line, tokens, " \t");
	    if( tokens.size() == 3 ){
	      current_temps = ow::stoi(tokens[2], 16);
	      if(current_temps > tempcount)
		tempcount = current_temps;
	      //cout << "current_temps changed to: " << current_temps << '\n';
	    }
	  } else if ( line.find("time")!=string::npos){
	 
	    ow::Tokenize(line, tokens, " \t");
       
	    if(tokens.size() == 2+4*current_adcs+current_temps ){
	  
	      if(data.size() < (tokens.size()-2))
		{
		  cout << "data size is smaller than line size" << std::endl;
		  for( int t = 2; t < tokens.size(); ++t){
		    
		    order.push_back(tokens[t]);
		    data.push_back(nullvect);
		    std::cout << "push back\n";
		    
		  }
		}
	    }
	  
	    else {
	      
	      cout << "-Names entry in header does not contain expected number of entries\n";
	    }
	  }
	
	}
      }
	
    
    

	// done processing comments ...
	// rstrip line, including comment character
    
	line = line.substr(0,last);
      
      // toss blank lines, which disposes of complete comment lines
      // and blank entries
      if(line.empty())
	continue;
    
  


      // Now we should be looking at a data line ... tokenize
      ow::Tokenize(line, tokens, " \t");
      //cout << "Reading in data \n";
   

      int cols = 0;
      // Did the line contain the entries is was supposed too?
      if( tokens.size() != (cols = 1+4*current_adcs+current_temps) ){
	cout << "Problem with parsing line!  Expected " 
	     << cols << " and found " 
	     << tokens.size() << "\n";
	break;
      }
      
     
      
    
      std::vector<std::string>::iterator 
	btok = tokens.begin();
     
      time_t etime= ow::time_from_string(*btok++);
  
     
     

      // do the right thing when time is out of bounds
      if( etime < first_gmt ){
	//std::cout << "continuing" << first_gmt <<  std::endl;
	continue;
      }
      else if( etime > last_gmt ){
	//std::cout << "breaking" << std::endl;
	break;
      }
      
     
      

      timedata.push_back(etime);


    
    
    
      // map stuffing time....
      //std::cout << "entries in tokens is " << tokens.size() << " \n";
      for(int btoker = 1; btoker < tokens.size(); ++btoker){
      
	hold = ow::stof( *btok++ );
	data [btoker-1].push_back(hold);
      
       
 
      }
    
    }
  
  }

     	  
  #ifdef CHECK_INPUT

  std::cout << "number of columns is " << data.size() << "\n";
  std::cout << "number of time entries is  " << timedata.size() << "\n";
  for(int check0 = 0; check0 < data.size(); check0++){
    std::cout << "number of entries in column " << check0+2 << " is " << data[check0].size() << "\n\n";
  }


  for(int check1 = 0; check1 < timedata.size(); check1++){
    std::cout << timedata[check1] << " \t";
    for(int check2 = 0; check2 < data.size(); check2++){
      std::cout << (data[check2])[check1] << " \t";
    }
    std::cout << "\n";
  }
      

  #endif
    
      
   // processing files ... yippee!

  

  // now we can plot: shite!

  // Let's start with initializing our Root styles.  I don't know what
  // any of this does....
  //init();
  //gStyle->SetOptStat(11);

  // and a canvas to draw on ...

  // HACK
  TCanvas can("can", "can", 0, 0, 800, 1000);
  
  can.Draw(); 
  can.cd(); // why?

  // Carve up the canvas ...
  can.Divide((1+(adccount*4+tempcount)/3),3, 0.001, 0.001, 0);
  can.Update();
  gStyle->SetTimeOffset(timedata[0]);
  // ... and put some stuff on it.
  
  
 
 


  

  float markersize = 0.3;
  // HACK!
  std::string name;
  std::string namehold;
  char *histname;
  std::ostringstream oss(name);

  int starthour_minute = (ow::hour_from_time(first_gmt))*3600+(ow::minute_from_time(first_gmt))*60;
  
  int dataiterator = 0;
    int timehold = first_gmt;
    float sum = 0.0;
    int averagecounter = 0;
    int binnum;
    int timeWOseconds;
  for(int a = 0; a<data.size();a++){
    namehold = order[a];
    oss.str("");
    oss << namehold;
    cout << oss.str() << "\n\n\n";
    //datagraphs.push_back(nullgraph);
    
    
   
    TH1F *frame = new TH1F(oss.str().c_str(),oss.str().c_str(),nBins,0,lookback);
    frame->GetXaxis()->SetTimeDisplay(1);
    frame->GetXaxis()->SetTimeFormat("%H:%M");
   
    for(int bincounter = 1; bincounter <= nBins; ++bincounter){
      //time_t time1 = ow::gmt_datetime(timedata[0] + frame->GetBinLowEdge(bincounter));
      //time_t time2 = ow::gmt_datetime(timedata[0] + frame->GetBinLowEdge(bincounter) + frame->GetBinWidth(bincounter));

      //cout << "Time range between " << time1 << " and " << time2 << "\n";
      cout << "Time range between " << ow::gmt_datetime(timedata[0]+frame->GetBinLowEdge(bincounter)) << " and " << ow::gmt_datetime(timedata[0]+frame->GetBinLowEdge(bincounter)+frame->GetBinWidth(bincounter)-1) << "\n";

      while(timedata[dataiterator] >= (timedata[0] + frame->GetBinLowEdge(bincounter)) && timedata[dataiterator] < (timedata[0] + frame->GetBinWidth(bincounter)+frame->GetBinLowEdge(bincounter))){
	
	if(dataiterator >= timedata.size())
	  {
	    cout << "end of data/n";
	    break;
	  }
	cout << "at data " << dataiterator << " value " << ow::gmt_datetime(timedata[dataiterator]) << "\n";
	
	sum += (data[a])[dataiterator];
	
	++dataiterator;
	++averagecounter;
      }
     
      
      cout << "filling histogram, bin number " << bincounter << " timehold is " << ow::gmt_datetime(timehold) << " with " << sum/averagecounter <<  "\n";
      if(averagecounter == 0)
	frame->SetBinContent(bincounter,0);
      else
      frame->SetBinContent(bincounter,(sum/averagecounter));
      averagecounter = 0;
      sum = 0;
    }
    dataiterator = 0;
   
  
  can.cd(a+1);
  
  frame->Draw();
  }


  


  #if 0 
    for(int i=0; i!=adccount*4; ++i){
    // Build the plots
    oss.str("");
    // HACK
    oss << "ADC" << std::hex << i << std::dec;
    cout << oss.str() << '\n';
    /*TGraph g(1024);
    g.SetName(oss.str().c_str());
    g.SetTitle(oss.str().c_str());
    g.SetMarkerStyle(21);
    g.SetMarkerSize(markersize);
    g.SetMarkerColor(colors[i%4]);
    
    adcgraphs.push_back(g);
    */ }
  for(int i=0; i!=current_temps/*temp_probes*/; ++i){
    // Build the plots
    oss.str("");
    // HACK
    oss << "TEMP" << std::hex << i << std::dec;
    cout << oss.str() << '\n';
    /*TGraph g(1024);
    g.SetName(oss.str().c_str());
    g.SetTitle(oss.str().c_str());
    g.SetMarkerStyle(21);
    g.SetMarkerSize(markersize);
    g.SetMarkerColor(colors[i%4]);
    
    tempgraphs.push_back(g);
    */  }





  TVirtualPad *pad;
  TH1F *frame;


  const int time_offset = -2;
  ymin = -0.1, ymax=5.22;
  tmin = first_gmt-240, tmax = last_gmt+240;
  for(int i=0; i!=/*adc_probes*/current_adcs; ++i){
    pad = can.GetPad(i);

    DRAWFRAME("ADC", "Voltage");
    for(int j=1; j<data.size(); ++j)
      
  }
  
#endif
  // Update it and ship it out
  can.Update();
  can.SaveAs("../png/onewire.png");
  can.SaveAs("./onewire.ps");

  

}



// Utility functions below here...


namespace ow {
  std::string get_filename(const time_t date){
    return std::string( directory + gmt_date(date) 
			+ '-' + fename + ".dat" );
  }

  std::string gmt_date(const time_t date){
    const int size = 32;
    char buffer[size];
    tm tmdate;
    
    gmtime_r(&date, &tmdate);
    //tmdate = gmtime(&date);
    strftime(buffer, size, dateformat, &tmdate);

    return std::string(buffer);
  }

  std::string gmt_datetime(const time_t date){
    const int size = 32;
    char buffer[size];
    tm tmdate;
    gmtime_r(&date, &tmdate);
    //tmdate = gmtime(&date);
    strftime(buffer, size, datetimeformat, &tmdate);

    return std::string(buffer);
  }

  int hour_from_time(const time_t time){
    tm tmtime;
    gmtime_r(&time, &tmtime);
    return tmtime.tm_hour;
  }

  int minute_from_time(const time_t time){
    tm tmtime;
    gmtime_r(&time, &tmtime);
    return tmtime.tm_min;
  }

  int second_from_time(const time_t time){
    tm tmtime;
    gmtime_r(&time, &tmtime);
    return tmtime.tm_sec;
  }

  int day_from_time(const time_t time){
    tm tmtime;
    gmtime_r(&time, &tmtime);
    return tmtime.tm_mday;
}
  int month_from_time(const time_t time){
    tm tmtime;
    gmtime_r(&time, &tmtime);
    return tmtime.tm_mon;
  }
  //int strip_seconds(const time_t time){
  //int month = month_from_time(time);
  // if(month == 01 || month == 03 || month == 05 || month == 07 || month == 08 || month == 10 || month == 12){
  //   return month*31*24*60+day_from_time(time)*24*60+hour_from_time(time)*60+minute_from_time(time);
  //}
  //else if(month == 02)
  //  {
  //return 2*28*24*60+day_from_time(time)*24*60+hour_from_time(time)*60+minute_from_time(time);
  //  }
  //else{
  //  return  month*30*24*60+day_from_time(time)*24*60+hour_from_time(time)*60+minute_from_time(time);
  //}
  //}
    
  std::string local_date(const time_t date){
    const int size = 32;
    char buffer[size];
    tm tmdate;
    localtime_r(&date, &tmdate);
    //tmdate = localtime(&date);
    strftime(buffer, size, dateformat, &tmdate);

    return std::string(buffer);
  }


  std::string local_datetime(const time_t date){
    const int size = 32;
    char buffer[size];
    tm tmdate;
    localtime_r(&date, &tmdate);
    //tmdate = localtime(&date);
    strftime(buffer, size, datetimeformat, &tmdate);

    return std::string(buffer);
  }

  time_t time_from_string(const std::string& time){
    const char* t = time.c_str();
    tm outtm;
    time_t out = 0;
    gmtime_r( &out, &outtm );
    //outtim = gmtime(&out);
    strptime(t, datetimeformat, &outtm);
    out = timegm( &outtm);
    return out;
  }

  

  // stolen from the web: http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
  void Tokenize(const std::string& str,
		std::vector<std::string>& tokens,
		const std::string& delimiters)
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (std::string::npos != pos || std::string::npos != lastPos)
      {
	// Found a token, add it to the vector.
	tokens.push_back(str.substr(lastPos, pos - lastPos));
	// Skip delimiters.  Note the "not_of"
	lastPos = str.find_first_not_of(delimiters, pos);
	// Find next "non-delimiter"
	pos = str.find_first_of(delimiters, lastPos);
      }
  }

  double stof(const std::string& s){
    return std::atof(s.c_str());
  }
  
  int stoi(const std::string& s, int base){
    return std::strtol(s.c_str(), (char **)NULL, base);
  }
  
}
