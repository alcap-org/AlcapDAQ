# A list of directories with source files.
DIRS += src/\
        src/TAP_generators/\
        src/TAP_generators/utils/\
        src/TAP_generators/template_fitting/\
        src/TDP_generators/\
        src/TME_generators/\
        src/examples/\
        src/framework/\
        src/plotters/\
        src/data_quality/\
        src/physics/

# A list of classes that need dictionary generation by root
# Make sure to add the relevant lines to the LibraryLinkDef.h file as well
#ROOT_DICT_CLASSES:=TDetectorPulse
ROOT_DICT_CLASSES:=TAnalysedPulseMapWrapper\
	TDetectorPulse\
	TGaussFitAnalysedPulse\
	TTemplateFitAnalysedPulse\
	TTemplateConvolveAnalysedPulse\
	TIntegralRatioAnalysedPulse\
	IdChannel\
	IdGenerator\
	IdSource\
        BankBranch\
        FlyWeight\
        TTemplate\
