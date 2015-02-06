#include "TDPGeneratorOptions.h"
#include "TDPGeneratorFactory.h"

void TDPGeneratorOptions::AddArgument(const int& number, const std::string& option) {
  // Get the name of this argument
  std::string name = TDPGeneratorFactory::Instance()->GetArgumentName(fModuleName,number);
  // Set the value of the corresponding option
  SetOption(name,option);
}
