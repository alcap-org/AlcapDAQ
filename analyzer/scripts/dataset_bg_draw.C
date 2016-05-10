#include <limits>
#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TBox.h"

TMultiGraph* dataset_bg_draw(TLegend* &leg) {
  static const Double_t ymax = 20000;
  static const int nsets =  6;
  const char* names [nsets] = { "Al100", "Si16a", "Al50",
				"Ti50", "MT", "Si16b" };
  int nsubsets[nsets] = { 5, 1, 1, 2, 1, 2 };
  int runs[nsets][5][2] = {
    {{9410, 9412},   {9482, 9555},   {9559, 9585}, {9594, 9641}, {9655, 9682}},
    {{9735, 9743},   {0, 0},         {0, 0},       {0, 0},       {0, 0}      },
    {{10087, 10129}, {0, 0},         {0, 0},       {0, 0},       {0, 0}      },
    {{10149, 10156}, {10157, 10206}, {0, 0},       {0, 0},       {0, 0}      },
    {{10272, 10317}, {0, 0},         {0, 0},       {0, 0},       {0, 0}      },
    {{10352, 10355}, {10425, 10450}, {0, 0},       {0, 0},       {0, 0}      }
  };
  int colors  [nsets] = { 1, 2, 3, 4, 5, 6 };
  TMultiGraph* bg = new TMultiGraph();
  leg = new TLegend(0.9, 0.1, 1.0, 0.5);
  Double_t x[20], y[20];
  
  for (int i = 0; i < nsets; ++i) {
    for (int j = 0; j < nsubsets[i]; ++j) {
      x[4*j]   = x[4*j+1] = runs[i][j][0];
      x[4*j+2] = x[4*j+3] = runs[i][j][1];
      y[4*j]   = y[4*j+3] = -ymax;
      y[4*j+1] = y[4*j+2] = ymax;
    }
    TGraph* boxes = new TGraph(4*nsubsets[i], x, y);
    boxes->SetFillStyle(3004);
    boxes->SetFillColor(colors[i]);
    bg->Add(boxes, "F");
    leg->AddEntry(boxes, names[i]);
  }
  return bg;
}
