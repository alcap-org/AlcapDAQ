RooUnfold             $Id$
=========

RooUnfold is a framework for unfolding (AKA "deconvolution" or "unsmearing").
It currently implements several methods: iterative (Bayes),
singular value decomposition (SVD), unregularised matrix inversion,
bin-by-bin (simple correction factors), and an interface to ROOT's TUnfold.
It can be used from the ROOT prompt, or linked against the ROOT libraries.
RooUnfold requires ROOT 4 or later.

For the latest version and documentation, see here

  http://hepunx.rl.ac.uk/~adye/software/unfold/RooUnfold.html

RooUnfold was written by Tim Adye, Kerstin Tackmann, and Fergus Wilson.

Building
========

Use GNU make. Just type

  make

to build the RooUnfold shared library.

AlCap
=====

RooUnfoldAlCap.cxx runs Monte Carlo validation and Bayesian data unfolding for AlCap

$ root -l runPreProcessor.C

$ root -l runUnfold.C
