#include "TF1.h"
#include "TCut.h"

struct Cut {
  Cut(std::string name, std::string eqn_string, std::string x_varname, std::string y_varname, bool keep_above) {
    eqn = new TF1(name.c_str(), eqn_string.c_str(), 0, 25000);
    eqn->SetLineColor(kBlack);

    std::string tcut_string = y_varname;
    if (keep_above) {
      tcut_string += ">";
    }
    else {
      tcut_string += "<";
    }
    tcut_string += eqn_string;
    // Check if we need to replace the variable x
    size_t position = tcut_string.find("*x"); // want to make sure we don't remove the "x" in "TMath::Exp" or anything
    if (position != std::string::npos) {
      tcut_string.replace(position+1, 1, x_varname);// want to replace "x" with the actual variable name
    }
    cut = tcut_string.c_str();
  }

  TF1* eqn;
  TCut cut;
};
