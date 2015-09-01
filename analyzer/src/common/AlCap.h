namespace AlCap {
  const int    MAXNCHANWFD      = 10;
  const int    NCRATE           = 10;
  const int    NCHANWFD[NCRATE] = { 0, 0, 0,
				    0, 8, 4,
				    0, 8, 8,
				    0};
  const double TICKWFD [NCRATE] = { 0., 0.,                    0.,
  				    0., 62.003158141820534865, 4.,
  				    0., 2.000102776481803346,  4.,
  				    0.}; // ns
  const double TICKTDC          = 0.024415317011901430;   // ns
  const double SYNCPULSEFREQ    = 554.832644497609862810; // Hz
}
