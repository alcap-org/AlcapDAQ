# A list of directories with source files.
DIRS += src/\
        src/TAP_generators/\
        src/TDP_generators/\
        src/TME_generators/\
        src/examples/\
        src/framework/\
        src/physics/\
        src/plotters/

# A list of classes that need dictionary generation by root
# Make sure to add the relevant lines to the LibraryLinkDef.h file as well
#ROOT_DICT_CLASSES:=TDetectorPulse
ROOT_DICT_CLASSES:=TAnalysedPulseMapWrapper\
	TDetectorPulse\
	IdChannel\
	IdGenerator\
	IdSource
