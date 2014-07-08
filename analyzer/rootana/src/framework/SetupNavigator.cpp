#include "SetupNavigator.h"

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <cstdlib>
#include <iostream>
#include <sstream>

SetupNavigator* SetupNavigator::fThis=NULL;

double SetupNavigator::GetPedestalError(std::string bankname) {

  // The values that we will read in
  double noise;

  // Get the SQLite database file
  TSQLiteServer* server = new TSQLiteServer("sqlite://pedestals-and-noises.sqlite");

  std::stringstream query; 
  std::string tablename = "pedestals_and_noises";
  int run_number = GetRunNumber(); // get this run number (Note that if we don't have a catalogue of pedestals and noises for each run then we will want to change this)
  if (server) {

    query << "SELECT * FROM " << tablename << " WHERE bank=\'" << bankname << "\' AND run=" << run_number << ";"; // get all the pedestals and noises
    TSQLiteResult* result = (TSQLiteResult*) server->Query(query.str().c_str());  // get the result of this query
    query.str(""); // clear the stringstream after use

    TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
    while (row != NULL) {
      noise = atof(row->GetField(4));
      
      delete row;
      row = (TSQLiteRow*) result->Next(); // get the next row
    }
    delete result; // user has to delete the result
  }
  else {
    std::cout << "Error: Couldn't connect to SQLite database" << std::endl;
  }
  server->Close();
  return noise;
}
