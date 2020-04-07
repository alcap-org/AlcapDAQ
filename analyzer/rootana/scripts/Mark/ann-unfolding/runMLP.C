void runMLP() {
//	gROOT->ProcessLine(".L R15b_NNClassifier.C");
//	R15b_NNClassifier("proton", 1000);
	gROOT->ProcessLine(".L R15b_Regression.C");
	R15b_Regression("proton", 1000);
}
