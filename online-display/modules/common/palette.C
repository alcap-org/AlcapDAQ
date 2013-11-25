
void SetRainbowPalette()
{
  UInt_t Number = 10;
//                       black  violet        blue                 green  yellow        red    white
  Double_t Red[20] =   { 0.000, 1.000, 0.300, 0.000, 0.000, 0.000, 0.000, 1.000, 1.000, 1.000, 1.000 };
  Double_t Green[20] = { 0.000, 0.000, 0.000, 0.000, 0.600, 0.700, 1.000, 1.000, 0.500, 0.000, 1.000 };
  Double_t Blue[20]  = { 0.000, 1.000, 0.600, 1.000, 0.600, 0.400, 0.000, 0.000, 0.000, 0.000, 1.000 };
  Double_t Stops[20] = { 0.000, 0.100, 0.200, 0.300, 0.400, 0.450, 0.500, 0.600, 0.800, 0.900, 1.000 };

  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 50);
}
