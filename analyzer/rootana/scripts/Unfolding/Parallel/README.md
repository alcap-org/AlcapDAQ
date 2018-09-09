Generate histograms for particle spectrums and unfolding

$ root -b runProof.C

For Al50 and Al100 processing, the output file name and location of the raw TMETree files needs to be changes in the runProof.C file. The number of workers should also be considered. The TMETree.C file would also have certain parts that need to be toggled. These are marked with the al100 and al50 commented lines within the SiL code brackets. The run numbers should also be toggled.
