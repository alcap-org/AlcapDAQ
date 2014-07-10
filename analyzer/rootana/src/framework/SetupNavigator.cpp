#include "SetupNavigator.h"
#include "ModulesParser.h"
#include "TSetupData.h"

SetupNavigator* SetupNavigator::fThis=NULL;

std::string SetupNavigator::GetBank(const IDs::channel& src)const{
    TSetupData* setup=TSetupData::Instance();
    std::map<std::string, std::string>* bankDetNameMap=&setup->fBankToDetectorMap;
    for(std::map<std::string, std::string>::iterator it = bankDetNameMap->begin();
            it != bankDetNameMap->end(); ++it){
        if(modules::parser::iequals(it->second , src.str())) return it->first;
    }
    return "invalid-det-bank";
}
