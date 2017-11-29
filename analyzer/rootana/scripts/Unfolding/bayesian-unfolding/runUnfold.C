void runUnfold()
{
	gROOT->ProcessLine(".L libRooUnfold.so");
	gROOT->ProcessLine(".L RooUnfoldAlCap.cxx");
	RooUnfoldAlCap();
}
