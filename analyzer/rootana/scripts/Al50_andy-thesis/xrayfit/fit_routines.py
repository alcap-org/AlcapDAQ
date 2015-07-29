"""
Helper module for fitting common pdfs
"""

import ROOT
from ROOT import TCanvas
from ROOT import RooFit, RooStats
from ROOT import RooArgSet, RooArgList, RooDataHist, RooWorkspace
from ROOT import gPad, gROOT

#import rootnotes

def fitGausPol1(Elo, Ehi, hist, name_prefix = None,
                area_lo = None, area_hi = None, 
                offset_lo = None, offset_hi = None,
                slope_lo = None, slope_hi = None,
                nbkg_lo = None, nbkg_hi = None,
                verbose = None):
    """ Fitting a gamma peak as a gaussian on top of a first order polynomial
    background. Some parameters are hardcoded.
    A name can be provided to make the fit parameters unique.
    """
    if verbose is not None:
        print "Fitting a Gaussian peak on top of a 1-st order polynomial\
        background to the", hist.GetName(), "in range:", Elo, "--", Ehi, "keV"
    
    if area_lo is None:
        area_lo = 1.
    if area_hi is None:
        area_hi = 1.E4
    if nbkg_lo is None:
        nbkg_lo = 0.
    if nbkg_hi is None:
        nbkg_hi = 1.E4
    if offset_lo is None:
        offset_lo = -10.
    if offset_hi is None:
        offset_hi = 100.
    if slope_lo is None:
        slope_lo = -0.1
    if slope_hi is None:
        slope_hi = 0.1
        
    ws = ROOT.RooWorkspace()
    bookstr = "Polynomial::pol1_bkg(" +\
    "edep[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", {bkg_offset[" + offset_lo.__str__() + "," +offset_hi.__str__()+ "]" +\
    ", bkg_slope[" + slope_lo.__str__() +"," + slope_hi.__str__() +"]"+\
    "} )"
    ws.factory(bookstr)
    if verbose is not None:
        print bookstr

    bookstr = "Gaussian::sig_pdf(" +\
    "edep[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", centroid[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", sigma[0.3, 10.])"
    if verbose is not None:
        print bookstr
    ws.factory(bookstr)
    
    bookstr = "SUM:model(area[" + area_lo.__str__() + "," + area_hi.__str__() + "]*sig_pdf"+\
    ", nbkg[" + nbkg_lo.__str__()+ "," + nbkg_hi.__str__() + "]*pol1_bkg)"
    ws.factory(bookstr)

    edep = ws.var("edep")
    pdf = ws.pdf("model") # pdf of the model
    centroid = ws.var("centroid")
    
    datahist = RooDataHist("tmp_datahist","tmp_datahist", RooArgList(edep), hist)
    
    model_config = RooStats.ModelConfig("model_config", ws)
    model_config.SetPdf(pdf)
    model_config.SetParametersOfInterest(RooArgSet(ws.var("area")))
    model_config.SetObservables(RooArgSet(ws.var("edep")))

    ws.defineSet("nuisParams", "a, nbkg")
    model_config.SetNuisanceParameters(RooArgSet(ws.var("nbkg")))
    
    pllCal = RooStats.ProfileLikelihoodCalculator(datahist, model_config)
    #fit_result = pllCal.fFitResult
    pllCal.SetConfidenceLevel(0.683)
    interval = pllCal.GetInterval()
    firstPOI = model_config.GetParametersOfInterest().first()

    #lowerlimit = interval.LowerLimit(firstPOI)
    #upperlimit = interval.UpperLimit(firstPOI)
    #if verbose is not None: 
        #print "Fit status:",fit_result.status()
    
    frame = edep.frame()
    datahist.plotOn(frame,
                    RooFit.MarkerSize(0.3),
                    RooFit.MarkerColor(ROOT.kAzure))
    pdf.plotOn(frame,
               RooFit.Components("sig_pdf"),
               RooFit.LineColor(ROOT.kOrange))
    pdf.plotOn(frame,
               RooFit.Components("pol1_bkg"),
               RooFit.LineStyle(ROOT.kDotted),
               RooFit.LineColor(ROOT.kMagenta))
    pdf.plotOn(frame,
               RooFit.LineColor(ROOT.kRed))
    c = TCanvas()
    c.SetWindowSize(400,300)
    frame.Draw()
    
    
    output = {}
    #output["fit_status"] = fit_result.status()
    output["area"] = firstPOI
    output["centroid"] = ws.var("centroid")
    output["nbkg"] = ws.var("nbkg")
    output["sigma"] = ws.var("sigma")
    output["bkg_slope"] = ws.var("bkg_slope")
    output["bkg_offset"] = ws.var("bkg_offset")
    output["canvas"] = c
    for param in output:
        if hasattr(output[param], 'GetName'):
            oldname = output[param].GetName()
            if name_prefix is None:
                newname = hist.GetName() + "_" + Elo.__str__()\
                + "_" + Ehi.__str__() + "_" + oldname
            else:
                newname = oldname
            output[param].SetName(newname)
    del ws
    return output

def fit2GausPol1(Elo, Ehi, Emid, hist, name_prefix = None,
                 area1_lo = None, area1_hi = None,
                 area2_lo = None, area2_hi = None,
                 offset_lo = None, offset_hi = None,
                 slope_lo = None, slope_hi = None,
                 nbkg_lo = None, nbkg_hi = None,
                 verbose = None, unit="keV"):
    """ Fitting two gaussian peaks on top of a first order polynomial
    background. Some parameters are hardcoded.
    A name can be provided to make the fit parameters unique.
    """
    if area1_lo is None:
        area1_lo = 1.
    if area1_hi is None:
        area1_hi = 1.E4
    if area2_lo is None:
        area2_lo = 1.
    if area2_hi is None:
        area2_hi = 1.E4        
    if nbkg_lo is None:
        nbkg_lo = 0.
    if nbkg_hi is None:
        nbkg_hi = 1.E4
    if offset_lo is None:
        offset_lo = -10.
    if offset_hi is None:
        offset_hi = 100.
    if slope_lo is None:
        slope_lo = -0.1
    if slope_hi is None:
        slope_hi = 0.1

    if verbose is not None: 
        print "Fitting two Gaussian peaks on top of a 1st order polynomial\
        background to the", hist.GetName(), "in range:", Elo, "--", Ehi, "keV"
    
    ws = ROOT.RooWorkspace()
    
    bookstr = "Polynomial::pol1_bkg(" +\
    "edep[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", {bkg_offset[" + offset_lo.__str__() + "," +offset_hi.__str__()+ "]" +\
    ", bkg_slope[" + slope_lo.__str__() +"," + slope_hi.__str__() +"]"+\
    "} )"
    if verbose is not None:
        print bookstr
    ws.factory(bookstr)

    bookstr = "Gaussian::peak1_pdf(" +\
    "edep[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", centroid1[" + Elo.__str__() + ", " + (Emid+2).__str__() +"]" +\
    ", sigma1[0.3, 10.])"
    if verbose is not None:
        print bookstr
    ws.factory(bookstr)
    
    bookstr = "Gaussian::peak2_pdf(" +\
    "edep[" + Elo.__str__() + ", " + Ehi.__str__() +"]" +\
    ", centroid2[" + (Emid-2).__str__() + ", " + Ehi.__str__() +"]" +\
    ", sigma2[0.3, 10.])"
    if verbose is not None:
        print bookstr
    ws.factory(bookstr)
    
    bookstr = "SUM:sum_pdf(area1[" + area1_lo.__str__() + "," + area1_hi.__str__() + "]*peak1_pdf"+\
    ", area2[" + area2_lo.__str__() + "," + area2_hi.__str__() + "]*peak2_pdf"+\
    ", nbkg[" + nbkg_lo.__str__()+ "," + nbkg_hi.__str__() + "]*pol1_bkg)"
    if verbose is not None:
        print bookstr

    ws.factory(bookstr)
    #ws.factory("SUM:sum_pdf(area1[0,10000]*peak1_pdf, area2[0,1E5]*peak2_pdf, nbkg[0,100000]*pol1_bkg)")

    edep = ws.var("edep")
    edep.setUnit("keV")
    pdf = ws.pdf("sum_pdf")
    centroid1 = ws.var("centroid1")
    centroid2 = ws.var("centroid2")
    
    datahist = RooDataHist("tmp_datahist","tmp_datahist", RooArgList(edep), hist)
    
    model_config = RooStats.ModelConfig("model_config", ws)
    model_config.SetPdf(pdf)
    model_config.SetParametersOfInterest(RooArgSet(ws.var("area1"),
                                                   ws.var("area2")))
    model_config.SetObservables(RooArgSet(ws.var("edep")))

    model_config.SetNuisanceParameters(RooArgSet(ws.var("nbkg")))
    
    pllCal = RooStats.ProfileLikelihoodCalculator(datahist, model_config)
    #fit_result = pllCal.fFitResult
    pllCal.SetConfidenceLevel(0.683)
    interval = pllCal.GetInterval()
    POIs = model_config.GetParametersOfInterest()

    #lowerlimit = interval.LowerLimit(firstPOI)
    #upperlimit = interval.UpperLimit(firstPOI)
    
    #if verbose is not None: 
        #print "Fit status:",fit_result.status()

    frame = edep.frame()
    frame.SetTitle("")
    datahist.plotOn(frame,
                    RooFit.MarkerSize(0.3),
                    RooFit.MarkerColor(ROOT.kAzure))
    pdf.plotOn(frame,
               RooFit.Components("peak1_pdf"),
               RooFit.LineColor(ROOT.kOrange))
    pdf.plotOn(frame,
               RooFit.Components("peak2_pdf"),
               RooFit.LineColor(ROOT.kOrange+2))
    pdf.plotOn(frame,
               RooFit.Components("pol1_bkg"),
               RooFit.LineStyle(ROOT.kDotted),
               RooFit.LineColor(ROOT.kMagenta))
    pdf.plotOn(frame,
               RooFit.LineColor(ROOT.kRed))
    c = TCanvas()
    c.SetWindowSize(400,250)
    frame.Draw()    
    
    output = {}
    #output["fit_status"] = fit_result.status()
    output["area1"] = POIs["area1"]
    output["area2"] = POIs["area2"]
    output["nbkg"] = ws.var("nbkg")
    output["centroid1"] = ws.var("centroid1")
    output["centroid2"] = ws.var("centroid2")
    output["sigma1"] = ws.var("sigma1")
    output["sigma2"] = ws.var("sigma2")
    output["bkg_slope"] = ws.var("bkg_slope")
    output["bkg_offset"] = ws.var("bkg_offset")
    output["canvas"] = c
    
    for param in output:
        if hasattr(output[param], 'GetName'):
            oldname = output[param].GetName()
            if name_prefix is None:
                newname = hist.GetName() + "_" + Elo.__str__()\
                + "_" + Ehi.__str__() + "_" + oldname
            else:
                newname = oldname
            output[param].SetName(newname)
    del ws        
    return output
