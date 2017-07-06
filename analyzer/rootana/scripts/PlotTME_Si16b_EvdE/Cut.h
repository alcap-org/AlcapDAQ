#include "TF1.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TLine.h"

#include <cstdlib>

struct Cut {
  Cut(std::string name, std::string eqn_string, std::string x_varname, std::string y_varname, bool keep_above, bool cut_on_x = false) {

    if (!cut_on_x) {
      eqn = new TF1(name.c_str(), eqn_string.c_str(), 0, 25000);
      eqn->SetLineColor(kBlack);
    }
    else {
      eqn = NULL;
      x_value = std::atoi(eqn_string.c_str());
      //      std::cout << "Can't create TF1s that cut on x so eqn set to NULL" << std::endl;
    }

    std::string tcut_string;
    if (!cut_on_x) {
      tcut_string = y_varname;
    }
    else {
      tcut_string = x_varname;
    }

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

  void Draw(TCanvas* c) {
    c->cd();
    if (eqn) {
      eqn->Draw("LSAME");
    }
    else {
      // need to draw a vertical line
      TLine* line = new TLine(x_value,0,x_value,c->GetUymax());
      line->SetLineColor(kBlack);
      line->SetLineWidth(2);
      line->Draw("LSAME");
    }
  }

  TF1* eqn;
  int x_value; // either eqn or x_value will be filled
  TCut cut;
};
