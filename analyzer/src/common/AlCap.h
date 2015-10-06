namespace AlCap {
  const int    MAXNCHANWFD      = 10;
  const int    NCRATE           = 10;
  const int    NCHANWFD[NCRATE] = { 0, 0, 0,
			            0, 8, 4,
				    0, 8, 8,
				    0};
  const int    NCHANTDC         = 32;
  const double TICKWFD [NCRATE] = { 0., 0.,  0.,
  				    0., 62., 4.,
  				    0., 2.,  4.,
  				    0.}; // ns
  const double TICKTDC          = 1./(40.96);   // ns
  const int BITSWFDADC[NCRATE]  = { 0,  0,  0,
				    0, 14, 12,
				    0, 14, 12,
				    0 };
  const int BITSWFDCLK[NCRATE]  = { 0, 0, 0,
				    0, 31, 31,
				    0, 31, 31,
				    0 };
  const int BITSTDCCLK          = 21;
}
