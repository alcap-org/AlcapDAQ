#!/usr/bin/perl

require "ctime.pl";
require "flush.pl";
use DBI;

my $run;
if( $#ARGV<0 ) { exit(0); } else { $run=$ARGV[0]; }

print <<EOF;
Content-type: text/html

<html>
<head>
<title>Run $run</title>
</head>
<body>
<h3> Run $run </h3>
<ul>
<li><a href="show_run_info.cgi?$run" target="plot_view_window">General info</a>
</ul>
<b>Online plots:</b>
<!-- <ul>
<li><a href="make_online_plot.cgi?$run+raw" target="plot_view_window">Raw Data (Time)</a>
<li><a href="make_online_plot.cgi?$run+fill" target="plot_view_window">Raw Data (Fill)</a>
<li><a href="make_online_plot.cgi?$run+wfdbanks" target="plot_view_window">WFD 
Banks</a>
<li><a href="make_online_plot.cgi?$run+wfdsize" target="plot_view_window">Size of WFD banks</a>
<li><a href="make_online_plot.cgi?$run+EMC" target="plot_view_window">EMC</a>
<li><a href="make_online_plot.cgi?$run+detector" target="plot_view_window">Lifetime</a>
<li><a href="make_online_plot.cgi?$run+calibration" target="plot_view_window">Calibration</a>
</ul> --!>

</body>
</html>
EOF

exit(0);

