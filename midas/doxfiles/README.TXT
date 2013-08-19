Midas Doxygen Documentation:
===========================

This directory contains the "related" files to the Midas documentation
as well as the doxygen configuration files for linux and Windows OS
(no build has been tested under cygwin yet!)

The current Doxygen version used is 1.3.5

These files combined with the images directory found on
      http://midas.psi.ch/download/doc
or
     http://midas.triumf.ca/download/doc
can be used to rebuild the full documentation through the doxygen package.

The configuration file midox.cfg can used on Linux or Window OS.

Midas Documentation Build Procedure:
===================================
Pre-requisite: 
1) Midas source code installed on your computer under <midas>
2) file midas-images.tar.gz downloaded to <midas/doxfiles/>
3) Doxygen utility already installed.

cd        <midas>/doxfiles/                 << Location for docmentation
tar      -zxvf midasdoc-images.tar.gz       << insert the images dir
doxygen   midox.cfg                         << generate the documentation
                                               with doxygen.

This will generate new subdirectories under :
<midas>/doxfiles/{html, latex, man, rtf}
The html/index.html and latex/refman.tex are the main documentation.
The latex/ directory contains a Makefile for ps and pdf generation.
