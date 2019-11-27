//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      2D test of RooUnfold package using toy MC generated according to PDFs
//      defined in RooUnfoldTestPdf.icc or RooUnfoldTestPdfRooFit.icc.
//      This is the main program. The actual tests are performed using the
//      RooUnfoldTestHarness2D class.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#include "RooUnfoldTestHarness2D.h"

RooUnfoldTestHarness2D* test2d= 0;

//==============================================================================
// Routine to run with parameters specified as a string
//==============================================================================

void RooUnfoldTest2D (const char* args= "")
{
// If run interactively, remove canvas and all histograms that might have been
// created with a previous invocation.
  delete test2d; test2d= 0;
  gDirectory->Clear();

  test2d= new RooUnfoldTestHarness2D ("RooUnfoldTest2D", args);
  test2d->Run();
}

#ifndef __CINT__

//==============================================================================
// Main program when run stand-alone
//==============================================================================

int main (int argc, char** argv) {
  RooUnfoldTestHarness2D maintest2d ("RooUnfoldTest2D", argc, argv);
  return maintest2d.Run();
}

#endif
