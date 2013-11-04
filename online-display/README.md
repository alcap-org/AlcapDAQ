AlCap online display
====================

This is a copy of the online display used in the MuSun and
MuCap experiments. It read histograms from a file or from
the running online analyzer and displays them using ROOT
macros. Compile the display with the command "make". Run the
display with no arguments.

    ./online-display

This shows a main display canvas and a row of buttons. Clicking
the buttons will invoke a ROOT macro (see Adding a Macro, below).
The macro connects to the online analyzer and receives histograms
that can be manipulated or combined before displaying on the canvas.

Since the online analyzer updates the histograms it creates as more
data comes in, running the macros additional times will update the
displays. You can do this by clicking the button for the specific
macro again.

Adding a Macro
==============

An example macro is included, raw_parameter_display.C. This grabs
some histograms defined in the MuSun analysis and draws them on 
the canvas. The minimal macro would look like this:

    {
      TCanvas *MuSunCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
      MuSunCanvas->cd();
      TH1 *parameters_muSC = getHist("muSC_count_raw");
      parameters_muSC->Draw();
    }

This fetches the canvas, grabs the histogram with the getHist() function,
and draws it on the canvas.

To add macro, you must create in the online-display/ directory and 
register it in mainWindow.cpp:

    struct screen_info screens[] = {
        {"Raw", "raw_parameter_display.C"},
        // Add more displays here after writing a ROOT macro, eg:
        // {"FADC_display", "fadc_display.C"},
    };

Recompile after registering a macro. You don't need to recompile after
making changes to a macro, once it is registered. The first string,
"Raw" in this example, is the name that appears on the button in the
display. The second string is the filename of the ROOT macro.

Running the display on a file
=============================

It is also possible to run the display on a ROOT file that contains
histograms. For instance, the online analyzer writes the histograms
it has created to a file on disk somewhere. You must only provide the 
filename as an argument.

    ./online-display /path/to/analyzer/output/hist00231.root

From here, it is the same as in online mode.
