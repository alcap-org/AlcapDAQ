#include "ProcessCorrectionFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "TSetupData.h"

using namespace std;

bool ProcessValues(TSetupData* s_data, std::string field,std::string bank_name, std::string newVal);

void CheckSetupData(TSetupData* s_data, char* correction_file){
  // if empty, then continue as no corrections to apply
  if(strcmp(correction_file,"")==0) return;

  // open the correction file
  std::ifstream file(correction_file, std::ifstream::in);
  if(!file.is_open()){
          cout<<"Problem opening correction file: "<<correction_file<<endl;
          return;
  }

  int max_line_size=1000;
  char line[max_line_size];
  // Skip the first two lines of the file which we assume to be header
  file.getline(line,max_line_size);
  file.getline(line,max_line_size);

  std::string field,bank_name, newVal;
  while(file.getline(line,max_line_size)){
          // Get the values from the line
          std::stringstream tokens(line);
          tokens>>bank_name>>field>>newVal;

          // Check that the bank name is meaningfull
          if(s_data->GetDetectorName(bank_name)==bank_name) {
                  cout<<"Warning: There is no bank called: "<<bank_name<<endl;
                  continue;
          }

          ProcessValues(s_data, field,bank_name, newVal);
  }
}

// BankName = STRING[48] :
// DetectorName = STRING[48] :
// Enabled = BOOL[48] :
// SamplingFrequency = FLOAT[48] :
// TriggerPolarity = INT[48] :
// Pedestal = INT[48] :
bool ProcessValues(TSetupData* s_data,std::string field,std::string bank_name, std::string newVal){
  if(field == "DetectorName"){
    s_data->SetDetectorName(bank_name,newVal);
  }else if(field == "Enabled"){
  }else if(field == "SamplingFrequency"){
  }else if(field == "TriggerPolarity"){
    s_data->SetTriggerPolarity(bank_name,atoi(newVal.c_str()));
  }else if(field == "Pedestal"){
    s_data->SetPedestal(bank_name,atoi(newVal.c_str()));
  }else if(field == "TimeShift"){
    s_data->SetTimeShift(bank_name,atof(newVal.c_str()));
  }else{
    cout<<"WARNING: Unknown wiremap field: "<<field<<endl;
  }
  cout<<"Setting "<<field<<" for bank "<<bank_name<<" to "<<atof(newVal.c_str())<<endl;
  return true;
}
