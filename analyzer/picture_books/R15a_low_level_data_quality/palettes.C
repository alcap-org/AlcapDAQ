void SetUserPalette(Int_t palette = 0, Int_t nCont=255, Float_t alpha =1){
  switch (palette) {
  case 1: 
    // Rainbow palette
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;

  case 2:
    //Blue-White-Red palette
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.20, 0.50, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.25, 1.00, 1.00, 0.50 };
    Double_t green[NRGBs] = { 0.00, 0.25, 1.00, 0.25, 0.00 };
    Double_t blue[NRGBs]  = { 0.50, 1.00, 1.00, 0.25, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
 
  case 3:
    //pale-to-dark with color 1
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.20, 0.50, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.78, 0.60, 0.54, 0.62, 0.55 };
    Double_t green[NRGBs] = { 0.73, 0.69, 0.58, 0.36, 0.00 };
    Double_t blue[NRGBs]  = { 0.84, 0.71, 0.38, 0.09, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
   
  case 4:
    //purple-red-orange
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.30, 0.60, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.16, 0.32, 0.70, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 0.00, 0.01, 0.48, 0.90 };
    Double_t blue[NRGBs]  = { 0.16, 0.32, 0.00, 0.00, 0.65 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
    
  case 5:
    //Colourblind OK, Ugly
    TColor::InitializeColors();
    const Int_t NRGBs = 3;
    Double_t stops[NRGBs] = { 0.00 , 0.50, 1.00  };
    Double_t red[NRGBs]   = { 0.047, 0.55, 1.00 };
    Double_t green[NRGBs] = { 0.031, 0.53, 0.51 };
    Double_t blue[NRGBs]  = { 0.455, 0.00, 0.45 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
    
    default:
    std::cout << "Unrecognised User Palette" << std::endl;
    break;
    
  } //switch
  
}
