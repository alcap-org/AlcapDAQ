void runUnfold()
{
	gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/libRooUnfold.so");
	gROOT->ProcessLine(".L scripts/Unfolding/bayesian-unfolding/RooUnfoldAlCap.cxx");
	RooUnfoldAlCap();
}
