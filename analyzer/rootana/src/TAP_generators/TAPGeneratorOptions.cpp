#include "TAPGeneratorOptions.h"
#include "TAPGeneratorFactory.h"

void TAPGeneratorOptions::AddArgument(const int& number, const std::string& option) {
  // Get the name of this argument
  std::string name = TAPGeneratorFactory::Instance()->GetArgumentName(fModuleName+"APGenerator",number);
  // Set the value of the corresponding option
  SetOption(name,option);
}
