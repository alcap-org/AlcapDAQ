#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class RooUnfold-;
#pragma link C++ class RooUnfoldBayes+;
#pragma link C++ class RooUnfoldBasisSplines+;
#pragma link C++ class RooUnfoldSvd-;
#pragma link C++ class RooUnfoldBinByBin+;
#pragma link C++ class RooUnfoldResponse-;
#pragma link C++ class RooUnfoldErrors+;
#pragma link C++ class RooUnfoldParms+;
#pragma link C++ class RooUnfoldInvert+;
#ifndef NOTUNFOLD
#pragma link C++ class RooUnfoldTUnfold+;
#endif
#ifdef HAVE_DAGOSTINI
#pragma link C++ class RooUnfoldDagostini+;
#endif
#if defined(HAVE_TSVDUNFOLD) || ROOT_VERSION_CODE < ROOT_VERSION(5,34,99)
#pragma link C++ class TSVDUnfold_130729+;
#endif

#endif
