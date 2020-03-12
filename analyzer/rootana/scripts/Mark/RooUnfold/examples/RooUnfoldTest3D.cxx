//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      3D test of RooUnfold package using toy MC generated according to PDFs
//      defined in RooUnfoldTestPdf.icc or RooUnfoldTestPdfRooFit.icc.
//      This is the main program. The actual tests are performed using the
//      RooUnfoldTestHarness3D class.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#include "RooUnfoldTestHarness3D.h"

RooUnfoldTestHarness3D* test3d= 0;

//==============================================================================
// Routine to run with parameters specified as a string
//==============================================================================

void RooUnfoldTest3D (const char* args= "")
{
// If run interactively, remove canvas and all histograms that might have been
// created with a previous invocation.
  delete test3d; test3d= 0;
  gDirectory->Clear();

  test3d= new RooUnfoldTestHarness3D ("RooUnfoldTest3D", args);
  test3d->Run();
}

#ifndef __CINT__

//==============================================================================
// Main program when run stand-alone
//==============================================================================

int main (int argc, char** argv) {
  RooUnfoldTestHarness3D maintest3d ("RooUnfoldTest3D", argc, argv);
  return maintest3d.Run();
}

#endif
